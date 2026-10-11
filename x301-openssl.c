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

#include <openssl/err.h>
#include <openssl/evp.h>

#include "crypto_api.h"
#include "log.h"

int
ssh_x301_available(void)
{
	static int available = -1;
	EVP_PKEY_CTX *ctx;
	EVP_KEYEXCH *exchange;
	int marked;

	if (available != -1)
		return available;
	marked = ERR_set_mark();
	ctx = EVP_PKEY_CTX_new_from_name(NULL, "X301", NULL);
	exchange = EVP_KEYEXCH_fetch(NULL, "X301", NULL);
	available = ctx != NULL && exchange != NULL;
	EVP_KEYEXCH_free(exchange);
	EVP_PKEY_CTX_free(ctx);
	if (marked)
		ERR_pop_to_mark();
	else
		ERR_clear_error();
	return available;
}

int
crypto_x301_keypair(unsigned char *pub, unsigned char *secret)
{
	EVP_PKEY_CTX *ctx = NULL;
	EVP_PKEY *pkey = NULL;
	unsigned char public[crypto_x301_PUBLICBYTES];
	unsigned char private[crypto_x301_SECRETBYTES];
	size_t publen = sizeof(public), secretlen = sizeof(private);
	int ret = -1;

	if (pub == NULL || secret == NULL) {
		debug3_f("missing output buffer");
		goto out;
	}
	if ((ctx = EVP_PKEY_CTX_new_from_name(NULL, "X301", NULL)) == NULL) {
		debug3_f("EVP_PKEY_CTX_new_from_name failed");
		goto out;
	}
	if (EVP_PKEY_keygen_init(ctx) <= 0 ||
	    EVP_PKEY_generate(ctx, &pkey) <= 0) {
		debug3_f("EVP_PKEY_generate failed");
		goto out;
	}
	if (EVP_PKEY_get_raw_public_key(pkey, public, &publen) != 1 ||
	    publen != sizeof(public)) {
		debug3_f("EVP_PKEY_get_raw_public_key failed or wrong length");
		goto out;
	}
	if (EVP_PKEY_get_raw_private_key(pkey, private, &secretlen) != 1 ||
	    secretlen != sizeof(private)) {
		debug3_f("EVP_PKEY_get_raw_private_key failed or wrong length");
		goto out;
	}
	memcpy(pub, public, sizeof(public));
	memcpy(secret, private, sizeof(private));
	ret = 0;
 out:
	EVP_PKEY_free(pkey);
	EVP_PKEY_CTX_free(ctx);
	explicit_bzero(private, sizeof(private));
	explicit_bzero(public, sizeof(public));
	if (ret != 0) {
		if (pub != NULL)
			explicit_bzero(pub, crypto_x301_PUBLICBYTES);
		if (secret != NULL)
			explicit_bzero(secret, crypto_x301_SECRETBYTES);
	}
	return ret;
}

int
crypto_x301_public(unsigned char *pub, const unsigned char *secret)
{
	EVP_PKEY *pkey = NULL;
	unsigned char public[crypto_x301_PUBLICBYTES];
	size_t publen = sizeof(public);
	int ret = -1;

	if (pub == NULL || secret == NULL) {
		debug3_f("missing secret or output buffer");
		goto out;
	}
	if ((pkey = EVP_PKEY_new_raw_private_key_ex(NULL, "X301",
	    NULL, secret, crypto_x301_SECRETBYTES)) == NULL) {
		debug3_f("EVP_PKEY_new_raw_private_key_ex failed");
		goto out;
	}
	if (EVP_PKEY_get_raw_public_key(pkey, public, &publen) != 1 ||
	    publen != sizeof(public)) {
		debug3_f("EVP_PKEY_get_raw_public_key failed or wrong length");
		goto out;
	}
	memcpy(pub, public, sizeof(public));
	ret = 0;
 out:
	EVP_PKEY_free(pkey);
	explicit_bzero(public, sizeof(public));
	if (ret != 0 && pub != NULL)
		explicit_bzero(pub, crypto_x301_PUBLICBYTES);
	return ret;
}

int
crypto_x301_shared(unsigned char *shared, const unsigned char *secret,
    const unsigned char *peer_pub)
{
	EVP_PKEY *pkey = NULL, *peer = NULL;
	EVP_PKEY_CTX *ctx = NULL;
	unsigned char result[crypto_x301_BYTES];
	unsigned char zero[crypto_x301_BYTES];
	size_t len = sizeof(result);
	int ret = -1;

	if (shared == NULL || secret == NULL || peer_pub == NULL) {
		debug3_f("missing key material or output buffer");
		goto out;
	}
	if ((pkey = EVP_PKEY_new_raw_private_key_ex(NULL, "X301", NULL,
	    secret, crypto_x301_SECRETBYTES)) == NULL ||
	    (peer = EVP_PKEY_new_raw_public_key_ex(NULL, "X301", NULL,
	    peer_pub, crypto_x301_PUBLICBYTES)) == NULL) {
		debug3_f("raw key import failed");
		goto out;
	}
	if ((ctx = EVP_PKEY_CTX_new_from_pkey(NULL, pkey, NULL)) == NULL) {
		debug3_f("EVP_PKEY_CTX_new_from_pkey failed");
		goto out;
	}
	if (EVP_PKEY_derive_init(ctx) <= 0 ||
	    EVP_PKEY_derive_set_peer(ctx, peer) <= 0 ||
	    EVP_PKEY_derive(ctx, result, &len) <= 0 || len != sizeof(result)) {
		debug3_f("EVP_PKEY_derive failed or wrong length");
		goto out;
	}
	/* Keep the Curve25519 template's unconditional all-zero check. */
	explicit_bzero(zero, sizeof(zero));
	if (timingsafe_bcmp(result, zero, sizeof(result)) == 0) {
		debug3_f("all-zero shared secret");
		goto out;
	}
	memcpy(shared, result, sizeof(result));
	ret = 0;
 out:
	EVP_PKEY_CTX_free(ctx);
	EVP_PKEY_free(peer);
	EVP_PKEY_free(pkey);
	explicit_bzero(result, sizeof(result));
	explicit_bzero(zero, sizeof(zero));
	if (ret != 0 && shared != NULL)
		explicit_bzero(shared, crypto_x301_BYTES);
	return ret;
}

#endif /* WITH_OPENSSL && USE_ED301 */
