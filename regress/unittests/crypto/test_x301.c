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

#ifdef WITH_ED301

#include <sys/types.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WITH_OPENSSL
#include <openssl/core_names.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/params.h>
#else
#include "ed301-rust.h"
#endif

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

void x301_tests(void);

struct x301_key {
	char *id;
	u_char secret[38];
	u_char public[38];
};

static const struct x301_key *
find_key(const struct x301_key *keys, size_t nkeys, const char *id)
{
	size_t i;

	for (i = 0; i < nkeys; i++) {
		if (strcmp(keys[i].id, id) == 0)
			return &keys[i];
	}
	ASSERT_INT_EQ(1, 0);
	return NULL;
}

static size_t
key_vectors(struct sshbuf *json, struct x301_key *keys, size_t capacity)
{
	struct sshbuf *array = get_json_array(json, "keys"), *v;
	u_char *secret, *public, derived[38];
	size_t secretlen, publen, count = 0;

	TEST_START("X301 secret-to-public vectors");
	while ((v = get_json_object(array)) != NULL) {
		ASSERT_SIZE_T_LT(count, capacity);
		keys[count].id = get_json_string(v, "id", 0);
		test_subtest_info("%s", keys[count].id);
		secret = get_json_hex(v, "secret_hex", &secretlen);
		public = get_json_hex(v, "public_hex", &publen);
		ASSERT_SIZE_T_EQ(secretlen, sizeof(keys[count].secret));
		ASSERT_SIZE_T_EQ(publen, sizeof(keys[count].public));
		memcpy(keys[count].secret, secret, secretlen);
		memcpy(keys[count].public, public, publen);
		ASSERT_INT_EQ(crypto_x301_public(derived, secret), 0);
		ASSERT_MEM_EQ(derived, public, sizeof(derived));
		freezero(secret, secretlen);
		free(public);
		sshbuf_free(v);
		count++;
	}
	ASSERT_SIZE_T_EQ(count, 7);
	sshbuf_free(array);
	TEST_DONE();
	fprintf(stderr, "X301 keys: %zu vectors PASS\n", count);
	return count;
}

static void
dh_vectors(struct sshbuf *json, const struct x301_key *keys, size_t nkeys)
{
	struct sshbuf *array = get_json_array(json, "dh"), *v;
	const struct x301_key *a, *b;
	char *id, *aid, *bid;
	u_char *expected, shared[38], reverse[38];
	size_t len, count = 0;

	TEST_START("X301 bidirectional DH vectors");
	while ((v = get_json_object(array)) != NULL) {
		id = get_json_string(v, "id", 0);
		test_subtest_info("%s", id);
		aid = get_json_string(v, "a", 0);
		bid = get_json_string(v, "b", 0);
		a = find_key(keys, nkeys, aid);
		b = find_key(keys, nkeys, bid);
		expected = get_json_hex(v, "shared_hex", &len);
		ASSERT_SIZE_T_EQ(len, sizeof(shared));
		ASSERT_INT_EQ(crypto_x301_shared(shared, a->secret,
		    b->public), 0);
		ASSERT_INT_EQ(crypto_x301_shared(reverse, b->secret,
		    a->public), 0);
		ASSERT_MEM_EQ(shared, expected, sizeof(shared));
		ASSERT_MEM_EQ(reverse, expected, sizeof(reverse));
		free(expected);
		free(id);
		free(aid);
		free(bid);
		sshbuf_free(v);
		count++;
	}
	ASSERT_SIZE_T_EQ(count, 4);
	sshbuf_free(array);
	explicit_bzero(shared, sizeof(shared));
	explicit_bzero(reverse, sizeof(reverse));
	TEST_DONE();
	fprintf(stderr, "X301 dh: %zu vectors PASS (both directions)\n", count);
}

static void
evaluation_vectors(struct sshbuf *json, const struct x301_key *keys,
    size_t nkeys)
{
	struct sshbuf *array = get_json_array(json, "evaluations"), *v;
	const struct x301_key *key;
	u_char *u, *expected, result[38];
	size_t ulen, len, count = 0;
	char *id, *kid;

	TEST_START("X301 curve and twist evaluations");
	while ((v = get_json_object(array)) != NULL) {
		id = get_json_string(v, "id", 0);
		test_subtest_info("%s", id);
		kid = get_json_string(v, "key", 0);
		key = find_key(keys, nkeys, kid);
		u = get_json_hex(v, "u_hex", &ulen);
		expected = get_json_hex(v, "result_hex", &len);
		ASSERT_SIZE_T_EQ(ulen, sizeof(result));
		ASSERT_SIZE_T_EQ(len, sizeof(result));
		ASSERT_INT_EQ(crypto_x301_shared(result, key->secret, u), 0);
		ASSERT_MEM_EQ(result, expected, sizeof(result));
		free(u);
		free(expected);
		free(kid);
		free(id);
		sshbuf_free(v);
		count++;
	}
	ASSERT_SIZE_T_EQ(count, 24);
	sshbuf_free(array);
	explicit_bzero(result, sizeof(result));
	TEST_DONE();
	fprintf(stderr, "X301 evaluations: %zu vectors PASS\n", count);
}

static void
error_vectors(struct sshbuf *json)
{
	struct sshbuf *array = get_json_array(json, "errors"), *v;
#ifdef WITH_OPENSSL
	EVP_PKEY *key;
#endif
	u_char *secret, *u, shared[38];
	size_t secretlen, ulen, count = 0, lengths = 0;
	char *id;

	TEST_START("X301 error vectors and zeroed failure output");
	while ((v = get_json_object(array)) != NULL) {
		id = get_json_string(v, "id", 0);
		test_subtest_info("%s", id);
		secret = get_json_hex(v, "secret_hex", &secretlen);
		u = get_json_hex(v, "u_hex", &ulen);
		if (secretlen != crypto_x301_SECRETBYTES) {
#ifdef WITH_OPENSSL
			key = EVP_PKEY_new_raw_private_key_ex(NULL, "X301",
			    NULL, secret, secretlen);
			ASSERT_PTR_EQ(key, NULL);
			EVP_PKEY_free(key);
#else
			memset(shared, 0xa5, sizeof(shared));
			ASSERT_INT_EQ(ed301_ssh_x_public(shared, secret, secretlen), -1);
			ASSERT_MEM_ZERO_EQ(shared, sizeof(shared));
#endif
			lengths++;
		} else if (ulen != crypto_x301_PUBLICBYTES) {
#ifdef WITH_OPENSSL
			key = EVP_PKEY_new_raw_public_key_ex(NULL, "X301",
			    NULL, u, ulen);
			ASSERT_PTR_EQ(key, NULL);
			EVP_PKEY_free(key);
#else
			memset(shared, 0xa5, sizeof(shared));
			ASSERT_INT_EQ(ed301_ssh_x_shared(shared, secret, secretlen,
			    u, ulen), -1);
			ASSERT_MEM_ZERO_EQ(shared, sizeof(shared));
#endif
			lengths++;
		} else {
			memset(shared, 0xa5, sizeof(shared));
			ASSERT_INT_EQ(crypto_x301_shared(shared, secret,
			    u), -1);
			ASSERT_MEM_ZERO_EQ(shared, sizeof(shared));
		}
		freezero(secret, secretlen);
		free(u);
		free(id);
		sshbuf_free(v);
		count++;
	}
	ASSERT_SIZE_T_EQ(count, 33);
	sshbuf_free(array);
	explicit_bzero(shared, sizeof(shared));
	TEST_DONE();
	fprintf(stderr, "X301 errors: %zu vectors PASS "
	    "(%zu variable-length backend cases)\n", count, lengths);
}

static void
weak_vectors(struct sshbuf *json, const struct x301_key *key)
{
	struct sshbuf *array = get_json_array(json, "weak_secrets"), *v;
	u_char *secret, public[38], shared[38];
	size_t len, count = 0;
	char *id;

	TEST_START("X301 all excluded weak-secret aliases");
	while ((v = get_json_object(array)) != NULL) {
		id = get_json_string(v, "id", 0);
		test_subtest_info("%s", id);
		secret = get_json_hex(v, "secret_hex", &len);
		ASSERT_SIZE_T_EQ(len, crypto_x301_SECRETBYTES);
		memset(public, 0xa5, sizeof(public));
		memset(shared, 0xa5, sizeof(shared));
		ASSERT_INT_EQ(crypto_x301_public(public, secret), -1);
		ASSERT_MEM_ZERO_EQ(public, sizeof(public));
		ASSERT_INT_EQ(crypto_x301_shared(shared, secret,
		    key->public), -1);
		ASSERT_MEM_ZERO_EQ(shared, sizeof(shared));
		freezero(secret, len);
		free(id);
		sshbuf_free(v);
		count++;
	}
	ASSERT_SIZE_T_EQ(count, 64);
	sshbuf_free(array);
	TEST_DONE();
	fprintf(stderr, "X301 weak_secrets: %zu vectors PASS "
	    "(public and shared)\n", count);
}

static void
iteration_vectors(struct sshbuf *json)
{
	struct sshbuf *array = get_json_array(json, "checkpoints"), *v;
	u_char k[38], u[38], result[38], *expected_k, *expected_u;
	size_t klen, ulen, count = 0;
	u_int i = 0, target;

	TEST_START("X301 iteration checkpoints through 1000");
	while ((v = get_json_object(array)) != NULL) {
		target = get_json_uint(v, "count");
		test_subtest_info("iteration %u", target);
		expected_k = get_json_hex(v, "k_hex", &klen);
		expected_u = get_json_hex(v, "u_hex", &ulen);
		ASSERT_SIZE_T_EQ(klen, sizeof(k));
		ASSERT_SIZE_T_EQ(ulen, sizeof(u));
		ASSERT_U_INT_GT(target, i);
		ASSERT_U_INT_LE(target, 1000);
		if (count == 0) {
			ASSERT_U_INT_EQ(target, 1);
			memcpy(k, expected_u, sizeof(k));
			memcpy(u, expected_u, sizeof(u));
		}
		for (; i < target; i++) {
			ASSERT_INT_EQ(crypto_x301_shared(result, k, u), 0);
			memcpy(u, k, sizeof(u));
			memcpy(k, result, sizeof(k));
		}
		ASSERT_MEM_EQ(k, expected_k, sizeof(k));
		ASSERT_MEM_EQ(u, expected_u, sizeof(u));
		free(expected_k);
		free(expected_u);
		sshbuf_free(v);
		count++;
	}
	ASSERT_SIZE_T_EQ(count, 4);
	ASSERT_U_INT_EQ(i, 1000);
	sshbuf_free(array);
	explicit_bzero(k, sizeof(k));
	explicit_bzero(u, sizeof(u));
	explicit_bzero(result, sizeof(result));
	TEST_DONE();
	fprintf(stderr, "X301 iteration: %zu checkpoints PASS "
	    "(1000 derives)\n", count);
}

static void
additional_tests(void)
{
	u_char public[38], secret[38], peer[38], peer_secret[38], derived[38];
	u_char shared[38], reverse[38];

	TEST_START("X301 random keypair consistency and uniqueness");
	ASSERT_INT_EQ(crypto_x301_keypair(public, secret), 0);
	ASSERT_INT_EQ(crypto_x301_keypair(peer, peer_secret), 0);
	ASSERT_MEM_NE(public, peer, sizeof(public));
	ASSERT_MEM_NE(secret, peer_secret, sizeof(secret));
	ASSERT_INT_EQ(crypto_x301_public(derived, secret), 0);
	ASSERT_MEM_EQ(derived, public, sizeof(public));
	ASSERT_INT_EQ(crypto_x301_public(derived, peer_secret), 0);
	ASSERT_MEM_EQ(derived, peer, sizeof(peer));
	TEST_DONE();

	TEST_START("X301 generated bidirectional shared secret");
	ASSERT_INT_EQ(crypto_x301_shared(shared, secret, peer), 0);
	ASSERT_INT_EQ(crypto_x301_shared(reverse, peer_secret, public), 0);
	ASSERT_MEM_EQ(shared, reverse, sizeof(shared));
	ASSERT_MEM_ZERO_NE(shared, sizeof(shared));
	TEST_DONE();

	TEST_START("X301 invalid pointers and zeroed failure outputs");
	memset(derived, 0xa5, sizeof(derived));
	ASSERT_INT_EQ(crypto_x301_public(derived, NULL), -1);
	ASSERT_MEM_ZERO_EQ(derived, sizeof(derived));
	ASSERT_INT_EQ(crypto_x301_shared(shared, NULL, peer), -1);
	ASSERT_MEM_ZERO_EQ(shared, sizeof(shared));
	memset(shared, 0xa5, sizeof(shared));
	ASSERT_INT_EQ(crypto_x301_shared(shared, secret, NULL), -1);
	ASSERT_MEM_ZERO_EQ(shared, sizeof(shared));
	ASSERT_INT_EQ(crypto_x301_keypair(derived, NULL), -1);
	ASSERT_MEM_ZERO_EQ(derived, sizeof(derived));
	ASSERT_INT_EQ(crypto_x301_keypair(NULL, peer_secret), -1);
	ASSERT_MEM_ZERO_EQ(peer_secret, sizeof(peer_secret));
	TEST_DONE();

	TEST_START("X301 in-place shared result");
	memcpy(reverse, secret, sizeof(reverse));
	ASSERT_INT_EQ(crypto_x301_shared(reverse, reverse, peer), 0);
	ASSERT_INT_EQ(crypto_x301_shared(shared, secret, peer), 0);
	ASSERT_MEM_EQ(reverse, shared, sizeof(shared));
	TEST_DONE();

	explicit_bzero(secret, sizeof(secret));
	explicit_bzero(peer_secret, sizeof(peer_secret));
	explicit_bzero(shared, sizeof(shared));
	explicit_bzero(reverse, sizeof(reverse));
	fprintf(stderr, "X301 additional: 4 tests PASS\n");
}

void
x301_tests(void)
{
	struct sshbuf *json;
	struct x301_key keys[7];
	size_t nkeys, i;

	if (!ssh_x301_available()) {
		fprintf(stderr, "SKIP X301: provider unavailable in default "
		    "OpenSSL context\n");
		return;
	}
	json = load_text_file("x301-v2.json");
	nkeys = key_vectors(json, keys, sizeof(keys) / sizeof(keys[0]));
	dh_vectors(json, keys, nkeys);
	evaluation_vectors(json, keys, nkeys);
	error_vectors(json);
	weak_vectors(json, &keys[0]);
	iteration_vectors(json);
	additional_tests();
	ASSERT_INT_EQ(ssh_x301_available(), 1);
	for (i = 0; i < nkeys; i++) {
		free(keys[i].id);
		explicit_bzero(keys[i].secret, sizeof(keys[i].secret));
	}
	sshbuf_free(json);
#ifdef WITH_OPENSSL
	ERR_clear_error();
#endif
}

#endif /* WITH_ED301 */
