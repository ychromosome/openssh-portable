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
#include <string.h>

#include <openssl/core_names.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/params.h>

#include "crypto_api.h"
#include "log.h"


int
ssh_ed301_available(void)
{
	static int available = -1;
	EVP_PKEY_CTX *ctx;
	EVP_SIGNATURE *signature;
	int marked;

	if (available != -1)
		return available;
	marked = ERR_set_mark();
	ctx = EVP_PKEY_CTX_new_from_name(NULL, "Ed301-EdDSA", NULL);
	signature = EVP_SIGNATURE_fetch(NULL, "Ed301-EdDSA", NULL);
	available = ctx != NULL && signature != NULL;
	EVP_SIGNATURE_free(signature);
	EVP_PKEY_CTX_free(ctx);
	if (marked)
		ERR_pop_to_mark();
	else
		ERR_clear_error();
	return available;
}

int
crypto_sign_ed301_keypair(unsigned char *pk, unsigned char *sk)
{
	EVP_PKEY_CTX *ctx = NULL;
	EVP_PKEY *pkey = NULL;
	unsigned char public[crypto_sign_ed301_PUBLICKEYBYTES];
	unsigned char private[crypto_sign_ed301_SECRETKEYBYTES];
	size_t pklen = sizeof(public), sklen = crypto_sign_ed301_SEEDBYTES;
	int ret = -1;

	if (pk == NULL || sk == NULL) {
		debug3_f("missing output buffer");
		goto out;
	}
	if ((ctx = EVP_PKEY_CTX_new_from_name(NULL,
	    "Ed301-EdDSA", NULL)) == NULL) {
		debug3_f("EVP_PKEY_CTX_new_from_name failed");
		goto out;
	}
	if (EVP_PKEY_keygen_init(ctx) <= 0 ||
	    EVP_PKEY_generate(ctx, &pkey) <= 0) {
		debug3_f("EVP_PKEY_generate failed");
		goto out;
	}
	if (EVP_PKEY_get_raw_public_key(pkey, public, &pklen) != 1 ||
	    pklen != sizeof(public)) {
		debug3_f("EVP_PKEY_get_raw_public_key failed or wrong length");
		goto out;
	}
	if (EVP_PKEY_get_raw_private_key(pkey, private, &sklen) != 1 ||
	    sklen != crypto_sign_ed301_SEEDBYTES) {
		debug3_f("EVP_PKEY_get_raw_private_key failed or wrong length");
		goto out;
	}
	memcpy(private + sklen, public, sizeof(public));
	memcpy(pk, public, sizeof(public));
	memcpy(sk, private, sizeof(private));
	ret = 0;
 out:
	EVP_PKEY_free(pkey);
	EVP_PKEY_CTX_free(ctx);
	explicit_bzero(private, sizeof(private));
	explicit_bzero(public, sizeof(public));
	if (ret != 0) {
		if (pk != NULL)
			explicit_bzero(pk, crypto_sign_ed301_PUBLICKEYBYTES);
		if (sk != NULL)
			explicit_bzero(sk, crypto_sign_ed301_SECRETKEYBYTES);
	}
	return ret;
}

int
crypto_sign_ed301_seed_keypair(unsigned char *pk, unsigned char *sk,
    const unsigned char *seed)
{
	EVP_PKEY *pkey = NULL;
	unsigned char public[crypto_sign_ed301_PUBLICKEYBYTES];
	unsigned char private[crypto_sign_ed301_SECRETKEYBYTES];
	size_t pklen = sizeof(public);
	int ret = -1;

	if (pk == NULL || sk == NULL || seed == NULL) {
		debug3_f("missing key material or output buffer");
		goto out;
	}
	if ((pkey = EVP_PKEY_new_raw_private_key_ex(NULL, "Ed301-EdDSA",
	    NULL, seed, crypto_sign_ed301_SEEDBYTES)) == NULL) {
		debug3_f("EVP_PKEY_new_raw_private_key_ex failed");
		goto out;
	}
	if (EVP_PKEY_get_raw_public_key(pkey, public, &pklen) != 1 ||
	    pklen != sizeof(public)) {
		debug3_f("EVP_PKEY_get_raw_public_key failed or wrong length");
		goto out;
	}
	memcpy(private, seed, crypto_sign_ed301_SEEDBYTES);
	memcpy(private + crypto_sign_ed301_SEEDBYTES, public, sizeof(public));
	memcpy(pk, public, sizeof(public));
	memcpy(sk, private, sizeof(private));
	ret = 0;
 out:
	EVP_PKEY_free(pkey);
	explicit_bzero(private, sizeof(private));
	explicit_bzero(public, sizeof(public));
	if (ret != 0) {
		if (pk != NULL)
			explicit_bzero(pk, crypto_sign_ed301_PUBLICKEYBYTES);
		if (sk != NULL)
			explicit_bzero(sk, crypto_sign_ed301_SECRETKEYBYTES);
	}
	return ret;
}

int
crypto_sign_ed301_detached_ctx(unsigned char *sig,
    unsigned long long *siglenp, const unsigned char *m,
    unsigned long long mlen, const unsigned char *ctx, size_t ctxlen,
    const unsigned char *sk)
{
	EVP_PKEY *pkey = NULL;
	EVP_MD_CTX *mdctx = NULL;
	OSSL_PARAM params[2];
	unsigned char public[crypto_sign_ed301_PUBLICKEYBYTES];
	unsigned char signature[crypto_sign_ed301_BYTES];
	size_t pklen = sizeof(public), siglen = sizeof(signature);
	int ret = -1;

	if (siglenp != NULL)
		*siglenp = 0;
	if (sig == NULL || sk == NULL || mlen > SIZE_MAX ||
	    (m == NULL && mlen != 0) ||
	    ctxlen > crypto_sign_ed301_CONTEXTMAXBYTES ||
	    (ctx == NULL && ctxlen != 0)) {
		debug3_f("invalid signing argument");
		goto out;
	}
	/* Only the seed is imported; never trust the appended public key. */
	if ((pkey = EVP_PKEY_new_raw_private_key_ex(NULL, "Ed301-EdDSA",
	    NULL, sk, crypto_sign_ed301_SEEDBYTES)) == NULL) {
		debug3_f("EVP_PKEY_new_raw_private_key_ex failed");
		goto out;
	}
	if (EVP_PKEY_get_raw_public_key(pkey, public, &pklen) != 1 ||
	    pklen != sizeof(public)) {
		debug3_f("EVP_PKEY_get_raw_public_key failed or wrong length");
		goto out;
	}
	if (timingsafe_bcmp(public, sk + crypto_sign_ed301_SEEDBYTES,
	    sizeof(public)) != 0) {
		debug3_f("public key does not match seed");
		goto out;
	}
	if ((mdctx = EVP_MD_CTX_new()) == NULL) {
		debug3_f("EVP_MD_CTX_new failed");
		goto out;
	}
	params[0] = OSSL_PARAM_construct_octet_string(
	    OSSL_SIGNATURE_PARAM_CONTEXT_STRING, (void *)ctx, ctxlen);
	params[1] = OSSL_PARAM_construct_end();
	if (EVP_DigestSignInit_ex(mdctx, NULL, NULL, NULL,
	    NULL, pkey, params) != 1) {
		debug3_f("EVP_DigestSignInit_ex failed");
		goto out;
	}
	if (EVP_DigestSign(mdctx, signature, &siglen, m,
	    (size_t)mlen) != 1 || siglen != sizeof(signature)) {
		debug3_f("EVP_DigestSign failed or wrong length");
		goto out;
	}
	memcpy(sig, signature, sizeof(signature));
	if (siglenp != NULL)
		*siglenp = siglen;
	ret = 0;
 out:
	EVP_MD_CTX_free(mdctx);
	EVP_PKEY_free(pkey);
	explicit_bzero(signature, sizeof(signature));
	explicit_bzero(public, sizeof(public));
	if (ret != 0 && sig != NULL)
		explicit_bzero(sig, crypto_sign_ed301_BYTES);
	return ret;
}

int
crypto_sign_ed301_detached(unsigned char *sig, unsigned long long *siglenp,
    const unsigned char *m, unsigned long long mlen, const unsigned char *sk)
{
	return crypto_sign_ed301_detached_ctx(sig, siglenp, m, mlen,
	    NULL, 0, sk);
}

int
crypto_sign_ed301_verify_detached_ctx(const unsigned char *sig,
    const unsigned char *m, unsigned long long mlen,
    const unsigned char *ctx, size_t ctxlen, const unsigned char *pk)
{
	EVP_PKEY *pkey = NULL;
	EVP_MD_CTX *mdctx = NULL;
	OSSL_PARAM params[2];
	int ret = -1;

	if (sig == NULL || pk == NULL || mlen > SIZE_MAX ||
	    (m == NULL && mlen != 0) ||
	    ctxlen > crypto_sign_ed301_CONTEXTMAXBYTES ||
	    (ctx == NULL && ctxlen != 0)) {
		debug3_f("invalid verification argument");
		goto out;
	}
	if ((pkey = EVP_PKEY_new_raw_public_key_ex(NULL, "Ed301-EdDSA",
	    NULL, pk, crypto_sign_ed301_PUBLICKEYBYTES)) == NULL) {
		debug3_f("EVP_PKEY_new_raw_public_key_ex failed");
		goto out;
	}
	if ((mdctx = EVP_MD_CTX_new()) == NULL) {
		debug3_f("EVP_MD_CTX_new failed");
		goto out;
	}
	params[0] = OSSL_PARAM_construct_octet_string(
	    OSSL_SIGNATURE_PARAM_CONTEXT_STRING, (void *)ctx, ctxlen);
	params[1] = OSSL_PARAM_construct_end();
	if (EVP_DigestVerifyInit_ex(mdctx, NULL, NULL, NULL,
	    NULL, pkey, params) != 1) {
		debug3_f("EVP_DigestVerifyInit_ex failed");
		goto out;
	}
	if (EVP_DigestVerify(mdctx, sig, crypto_sign_ed301_BYTES,
	    m, (size_t)mlen) != 1) {
		debug3_f("EVP_DigestVerify failed");
		goto out;
	}
	ret = 0;
 out:
	EVP_MD_CTX_free(mdctx);
	EVP_PKEY_free(pkey);
	return ret;
}


int
crypto_sign_ed301_verify_detached(const unsigned char *sig,
    const unsigned char *m, unsigned long long mlen, const unsigned char *pk)
{
	return crypto_sign_ed301_verify_detached_ctx(sig, m, mlen, NULL, 0, pk);
}

#endif /* WITH_OPENSSL && USE_ED301 */
