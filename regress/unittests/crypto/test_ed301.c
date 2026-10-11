/*
 * Copyright (c) 2026 Martin Wolf
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "includes.h"

#if defined(WITH_OPENSSL) && defined(USE_ED301)

#include <sys/types.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/core_names.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/params.h>

#include "../test_helper/test_helper.h"
#include "crypto_api.h"
#include "sshbuf.h"

/* In tests.c. */
struct sshbuf *load_text_file(const char *);
char *get_json_string(struct sshbuf *, const char *, int);
struct sshbuf *get_json_array(struct sshbuf *, const char *);
struct sshbuf *get_json_object(struct sshbuf *);
int get_json_boolean(struct sshbuf *, const char *);
u_int get_json_uint(struct sshbuf *, const char *);
u_char *get_json_hex(struct sshbuf *, const char *, size_t *);

void ed301_tests(void);

/* Only variable-length vectors use EVP directly: the public API is fixed. */
static int
verify_lengths(const u_char *sig, size_t siglen, const u_char *msg,
    size_t msglen, const u_char *context, size_t contextlen,
    const u_char *pk, size_t pklen)
{
	EVP_PKEY *pkey = NULL;
	EVP_MD_CTX *mdctx = NULL;
	OSSL_PARAM params[2];
	int ret = -1;

	if ((pkey = EVP_PKEY_new_raw_public_key_ex(NULL, "Ed301-EdDSA",
	    NULL, pk, pklen)) == NULL ||
	    (mdctx = EVP_MD_CTX_new()) == NULL)
		goto out;
	params[0] = OSSL_PARAM_construct_octet_string(
	    OSSL_SIGNATURE_PARAM_CONTEXT_STRING, (void *)context, contextlen);
	params[1] = OSSL_PARAM_construct_end();
	if (EVP_DigestVerifyInit_ex(mdctx, NULL, NULL, NULL,
	    NULL, pkey, params) == 1 &&
	    EVP_DigestVerify(mdctx, sig, siglen, msg, msglen) == 1)
		ret = 0;
 out:
	EVP_MD_CTX_free(mdctx);
	EVP_PKEY_free(pkey);
	return ret;
}

static void
signing_vectors(struct sshbuf *json)
{
	struct sshbuf *array = get_json_array(json, "signing"), *v, *trace;
	u_char pk[38], sk[76], sig[76], again[76];
	u_char *seed, *msg, *context, *expected_pk, *expected_sig;
	size_t seedlen, msglen, contextlen, pklen, siglen, count = 0;
	unsigned long long len;
	char *id;

	TEST_START("Ed301 signing vectors");
	while ((v = get_json_object(array)) != NULL) {
		id = get_json_string(v, "id", 0);
		test_subtest_info("%s", id);
		seed = get_json_hex(v, "seed_hex", &seedlen);
		msg = get_json_hex(v, "message_hex", &msglen);
		context = get_json_hex(v, "context_hex", &contextlen);
		/* Scope nested trace keys to this one vector. */
		ASSERT_PTR_NE(trace = sshbuf_fromb(v), NULL);
		expected_pk = get_json_hex(trace, "public_key", &pklen);
		expected_sig = get_json_hex(trace, "signature", &siglen);
		ASSERT_SIZE_T_EQ(seedlen, sizeof(pk));
		ASSERT_SIZE_T_EQ(pklen, sizeof(pk));
		ASSERT_SIZE_T_EQ(siglen, sizeof(sig));
		ASSERT_INT_EQ(crypto_sign_ed301_seed_keypair(pk, sk, seed), 0);
		ASSERT_MEM_EQ(pk, expected_pk, sizeof(pk));
		ASSERT_MEM_EQ(sk, seed, seedlen);
		ASSERT_MEM_EQ(sk + seedlen, pk, sizeof(pk));
		ASSERT_INT_EQ(crypto_sign_ed301_detached_ctx(sig, &len,
		    msg, msglen, context, contextlen, sk), 0);
		ASSERT_U64_EQ(len, sizeof(sig));
		ASSERT_MEM_EQ(sig, expected_sig, sizeof(sig));
		ASSERT_INT_EQ(crypto_sign_ed301_detached_ctx(again, &len,
		    msg, msglen, context, contextlen, sk), 0);
		ASSERT_MEM_EQ(sig, again, sizeof(sig));
		ASSERT_INT_EQ(crypto_sign_ed301_verify_detached_ctx(sig,
		    msg, msglen, context, contextlen, pk), 0);
		if (contextlen == 0) {
			ASSERT_INT_EQ(crypto_sign_ed301_detached(again, &len,
			    msg, msglen, sk), 0);
			ASSERT_MEM_EQ(sig, again, sizeof(sig));
			ASSERT_INT_EQ(crypto_sign_ed301_verify_detached(sig,
			    msg, msglen, pk), 0);
		}
		free(id);
		freezero(seed, seedlen);
		free(msg);
		free(context);
		free(expected_pk);
		free(expected_sig);
		sshbuf_free(trace);
		sshbuf_free(v);
		count++;
	}
	ASSERT_SIZE_T_EQ(count, 9);
	sshbuf_free(array);
	explicit_bzero(sk, sizeof(sk));
	TEST_DONE();
	fprintf(stderr, "Ed301 signing: %zu vectors PASS\n", count);
}

static void
verification_vectors(struct sshbuf *json)
{
	struct sshbuf *array = get_json_array(json, "verification"), *v;
	u_char *pk, *sig, *msg, *context;
	size_t pklen, siglen, msglen, contextlen, count = 0, lengths = 0;
	int accepted, r;
	char *id;

	TEST_START("Ed301 verification vectors");
	while ((v = get_json_object(array)) != NULL) {
		id = get_json_string(v, "id", 0);
		test_subtest_info("%s", id);
		accepted = get_json_boolean(v, "accepted");
		pk = get_json_hex(v, "public_key_hex", &pklen);
		sig = get_json_hex(v, "signature_hex", &siglen);
		msg = get_json_hex(v, "message_hex", &msglen);
		context = get_json_hex(v, "context_hex", &contextlen);
		if (pklen != crypto_sign_ed301_PUBLICKEYBYTES ||
		    siglen != crypto_sign_ed301_BYTES) {
			r = verify_lengths(sig, siglen, msg, msglen,
			    context, contextlen, pk, pklen);
			lengths++;
		} else {
			r = crypto_sign_ed301_verify_detached_ctx(sig,
			    msg, msglen, context, contextlen, pk);
		}
		ASSERT_INT_EQ(r, accepted ? 0 : -1);
		free(id);
		free(pk);
		free(sig);
		free(msg);
		free(context);
		sshbuf_free(v);
		count++;
	}
	ASSERT_SIZE_T_EQ(count, 61);
	sshbuf_free(array);
	TEST_DONE();
	fprintf(stderr, "Ed301 verification: %zu vectors PASS "
	    "(%zu variable-length EVP cases)\n", count, lengths);
}

static void
point_vectors(struct sshbuf *json)
{
	struct sshbuf *array = get_json_array(json, "point_decoding"), *v;
	EVP_PKEY *key;
	u_char *encoded, sig[76] = {0};
	size_t len, count = 0;
	char *id;
	int decoded;

	TEST_START("Ed301 point encodings and public-key policy");
	while ((v = get_json_object(array)) != NULL) {
		id = get_json_string(v, "id", 0);
		test_subtest_info("%s", id);
		decoded = get_json_boolean(v, "accepted");
		encoded = get_json_hex(v, "encoded_hex", &len);
		key = EVP_PKEY_new_raw_public_key_ex(NULL, "Ed301-EdDSA",
		    NULL, encoded, len);
		/* A decoded identity/torsion point is not a permitted key. */
		if (decoded && strcmp(id, "base") == 0)
			ASSERT_PTR_NE(key, NULL);
		else
			ASSERT_PTR_EQ(key, NULL);
		if (!decoded && len == crypto_sign_ed301_PUBLICKEYBYTES)
			ASSERT_INT_EQ(crypto_sign_ed301_verify_detached(sig,
			    (const u_char *)"", 0, encoded), -1);
		EVP_PKEY_free(key);
		free(encoded);
		free(id);
		sshbuf_free(v);
		count++;
	}
	ASSERT_SIZE_T_EQ(count, 15);
	sshbuf_free(array);
	TEST_DONE();
	fprintf(stderr, "Ed301 point_decoding: %zu vectors PASS "
	    "(public-key policy, including decoded torsion)\n", count);
}

static void
signing_error_vectors(struct sshbuf *json)
{
	struct sshbuf *array = get_json_array(json, "signing_errors"), *v;
	EVP_PKEY *key;
	u_char pk[38], sk[76], sig[76], *seed, *msg, *context;
	size_t seedlen, msglen, contextlen, count = 0;
	unsigned long long len;
	char *id;

	TEST_START("Ed301 signing error vectors");
	while ((v = get_json_object(array)) != NULL) {
		id = get_json_string(v, "id", 0);
		test_subtest_info("%s", id);
		seed = get_json_hex(v, "seed_hex", &seedlen);
		msg = get_json_hex(v, "message_hex", &msglen);
		context = get_json_hex(v, "context_hex", &contextlen);
		if (seedlen != crypto_sign_ed301_SEEDBYTES) {
			key = EVP_PKEY_new_raw_private_key_ex(NULL,
			    "Ed301-EdDSA", NULL, seed, seedlen);
			ASSERT_PTR_EQ(key, NULL);
			EVP_PKEY_free(key);
		} else {
			ASSERT_INT_EQ(crypto_sign_ed301_seed_keypair(pk,
			    sk, seed), 0);
			memset(sig, 0xa5, sizeof(sig));
			len = 123;
			ASSERT_INT_EQ(crypto_sign_ed301_detached_ctx(sig,
			    &len, msg, msglen, context, contextlen, sk), -1);
			ASSERT_U64_EQ(len, 0);
			ASSERT_MEM_ZERO_EQ(sig, sizeof(sig));
		}
		freezero(seed, seedlen);
		free(msg);
		free(context);
		free(id);
		sshbuf_free(v);
		count++;
	}
	ASSERT_SIZE_T_EQ(count, 4);
	sshbuf_free(array);
	explicit_bzero(sk, sizeof(sk));
	TEST_DONE();
	fprintf(stderr, "Ed301 signing_errors: %zu vectors PASS\n", count);
}

static void
negative_tests(void)
{
	u_char seed[38] = {0}, pk[38], sk[76], bad[76], sig[76];
	u_char context[256], pk2[38], sk2[76], derived[38], expanded[76];
	const u_char msg[] = "OpenSSH Ed301", changed[] = "OpenSSH Ed30x";
	unsigned long long len;

	ASSERT_INT_EQ(crypto_sign_ed301_seed_keypair(pk, sk, seed), 0);
	memset(context, 0xa5, sizeof(context));

	TEST_START("Ed301 double-public-key protection");
	memcpy(bad, sk, sizeof(bad));
	bad[38] ^= 1;
	memset(sig, 0xa5, sizeof(sig));
	len = 123;
	ASSERT_INT_EQ(crypto_sign_ed301_detached(sig, &len, msg,
	    sizeof(msg), bad), -1);
	ASSERT_U64_EQ(len, 0);
	ASSERT_MEM_ZERO_EQ(sig, sizeof(sig));
	TEST_DONE();

	TEST_START("Ed301 changed message");
	ASSERT_INT_EQ(crypto_sign_ed301_detached(sig, &len, msg,
	    sizeof(msg), sk), 0);
	ASSERT_INT_EQ(crypto_sign_ed301_verify_detached(sig, changed,
	    sizeof(changed), pk), -1);
	TEST_DONE();

	TEST_START("Ed301 changed context");
	ASSERT_INT_EQ(crypto_sign_ed301_detached_ctx(sig, &len, msg,
	    sizeof(msg), context, 1, sk), 0);
	context[0] ^= 1;
	ASSERT_INT_EQ(crypto_sign_ed301_verify_detached_ctx(sig, msg,
	    sizeof(msg), context, 1, pk), -1);
	TEST_DONE();

	TEST_START("Ed301 255-byte context");
	ASSERT_INT_EQ(crypto_sign_ed301_detached_ctx(sig, &len, msg,
	    sizeof(msg), context, 255, sk), 0);
	ASSERT_INT_EQ(crypto_sign_ed301_verify_detached_ctx(sig, msg,
	    sizeof(msg), context, 255, pk), 0);
	TEST_DONE();

	TEST_START("Ed301 256-byte context and zeroed failure output");
	ASSERT_INT_EQ(crypto_sign_ed301_verify_detached_ctx(sig, msg,
	    sizeof(msg), context, sizeof(context), pk), -1);
	ASSERT_INT_EQ(crypto_sign_ed301_detached_ctx(sig, &len, msg,
	    sizeof(msg), context, sizeof(context), sk), -1);
	ASSERT_MEM_ZERO_EQ(sig, sizeof(sig));
	ASSERT_U64_EQ(len, 0);
	TEST_DONE();

	TEST_START("Ed301 random keypair consistency and uniqueness");
	ASSERT_INT_EQ(crypto_sign_ed301_keypair(pk, sk), 0);
	ASSERT_INT_EQ(crypto_sign_ed301_keypair(pk2, sk2), 0);
	ASSERT_MEM_NE(pk, pk2, sizeof(pk));
	ASSERT_MEM_NE(sk, sk2, sizeof(sk));
	ASSERT_INT_EQ(crypto_sign_ed301_seed_keypair(derived, expanded, sk), 0);
	ASSERT_MEM_EQ(derived, pk, sizeof(pk));
	ASSERT_MEM_EQ(expanded, sk, sizeof(sk));
	ASSERT_INT_EQ(crypto_sign_ed301_detached(sig, NULL, msg,
	    sizeof(msg), sk), 0);
	ASSERT_INT_EQ(crypto_sign_ed301_verify_detached(sig, msg,
	    sizeof(msg), pk), 0);
	TEST_DONE();

	TEST_START("Ed301 invalid pointers and zeroed key outputs");
	ASSERT_INT_EQ(crypto_sign_ed301_seed_keypair(pk2, sk2, NULL), -1);
	ASSERT_MEM_ZERO_EQ(pk2, sizeof(pk2));
	ASSERT_MEM_ZERO_EQ(sk2, sizeof(sk2));
	ASSERT_INT_EQ(crypto_sign_ed301_detached_ctx(sig, &len, NULL, 1,
	    NULL, 0, sk), -1);
	ASSERT_MEM_ZERO_EQ(sig, sizeof(sig));
	ASSERT_U64_EQ(len, 0);
	ASSERT_INT_EQ(crypto_sign_ed301_detached_ctx(sig, &len, msg,
	    sizeof(msg), NULL, 1, sk), -1);
	ASSERT_MEM_ZERO_EQ(sig, sizeof(sig));
	ASSERT_INT_EQ(crypto_sign_ed301_verify_detached_ctx(sig, msg,
	    sizeof(msg), NULL, 1, pk), -1);
	TEST_DONE();

	TEST_START("Ed301 in-place seed keypair");
	ASSERT_INT_EQ(crypto_sign_ed301_seed_keypair(pk2, sk2, sk), 0);
	ASSERT_INT_EQ(crypto_sign_ed301_seed_keypair(pk2, sk2, sk2), 0);
	ASSERT_MEM_EQ(pk2, pk, sizeof(pk));
	ASSERT_MEM_EQ(sk2, sk, sizeof(sk));
	TEST_DONE();

	explicit_bzero(seed, sizeof(seed));
	explicit_bzero(sk, sizeof(sk));
	explicit_bzero(sk2, sizeof(sk2));
	explicit_bzero(expanded, sizeof(expanded));
	explicit_bzero(bad, sizeof(bad));
	fprintf(stderr, "Ed301 additional: 8 tests PASS\n");
}

void
ed301_tests(void)
{
	struct sshbuf *json;

	if (!ssh_ed301_available()) {
		fprintf(stderr, "SKIP Ed301: provider unavailable in default "
		    "OpenSSL context\n");
		return;
	}
	json = load_text_file("ed301-eddsa-v2.json");
	signing_vectors(json);
	verification_vectors(json);
	point_vectors(json);
	signing_error_vectors(json);
	negative_tests();
	ASSERT_INT_EQ(ssh_ed301_available(), 1);
	sshbuf_free(json);
	ERR_clear_error();
}

#endif /* WITH_OPENSSL && USE_ED301 */
