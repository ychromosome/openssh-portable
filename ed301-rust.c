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

#if defined(WITH_ED301) && defined(USE_ED301_RUST)
#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include "crypto_api.h"
#include "ed301-rust.h"

int ssh_ed301_available(void) { return 1; }
int ssh_x301_available(void) { return 1; }

int
crypto_sign_ed301_seed_keypair(unsigned char *pk, unsigned char *sk,
    const unsigned char *seed)
{
	u_char copied[38], public[38], private[76];
	int r = -1;

	if (pk == NULL || sk == NULL || seed == NULL)
		goto out;
	memcpy(copied, seed, sizeof(copied));
	if (ed301_ssh_seed_public(public, copied, sizeof(copied)) != 0)
		goto out;
	memcpy(private, copied, sizeof(copied));
	memcpy(private + sizeof(copied), public, sizeof(public));
	memcpy(pk, public, sizeof(public));
	memcpy(sk, private, sizeof(private));
	r = 0;
 out:
	explicit_bzero(copied, sizeof(copied));
	explicit_bzero(private, sizeof(private));
	if (r != 0) {
		if (pk != NULL)
			explicit_bzero(pk, crypto_sign_ed301_PUBLICKEYBYTES);
		if (sk != NULL)
			explicit_bzero(sk, crypto_sign_ed301_SECRETKEYBYTES);
	}
	return r;
}

int
crypto_sign_ed301_keypair(unsigned char *pk, unsigned char *sk)
{
	u_char seed[crypto_sign_ed301_SEEDBYTES];
	int r;

	arc4random_buf(seed, sizeof(seed));
	r = crypto_sign_ed301_seed_keypair(pk, sk, seed);
	explicit_bzero(seed, sizeof(seed));
	return r;
}

int
crypto_sign_ed301_detached_ctx(unsigned char *sig, unsigned long long *len,
    const unsigned char *msg, unsigned long long msglen,
    const unsigned char *ctx, size_t ctxlen, const unsigned char *sk)
{
	u_char result[crypto_sign_ed301_BYTES];
	int r = -1;

	if (len != NULL)
		*len = 0;
	if (sig == NULL || (unsigned long long)(size_t)msglen != msglen)
		goto out;
	if (ed301_ssh_sign_context(result, sk, crypto_sign_ed301_SECRETKEYBYTES,
	    msg, (size_t)msglen, ctx, ctxlen) != 0)
		goto out;
	memcpy(sig, result, sizeof(result));
	if (len != NULL)
		*len = sizeof(result);
	r = 0;
 out:
	explicit_bzero(result, sizeof(result));
	if (r != 0 && sig != NULL)
		explicit_bzero(sig, crypto_sign_ed301_BYTES);
	return r;
}

int
crypto_sign_ed301_detached(unsigned char *sig, unsigned long long *len,
    const unsigned char *msg, unsigned long long msglen,
    const unsigned char *sk)
{
	return crypto_sign_ed301_detached_ctx(sig, len, msg, msglen,
	    NULL, 0, sk);
}

int
crypto_sign_ed301_verify_detached_ctx(const unsigned char *sig,
    const unsigned char *msg, unsigned long long msglen,
    const unsigned char *ctx, size_t ctxlen, const unsigned char *pk)
{
	if ((unsigned long long)(size_t)msglen != msglen)
		return -1;
	return ed301_ssh_verify_context(pk, crypto_sign_ed301_PUBLICKEYBYTES,
	    sig, crypto_sign_ed301_BYTES, msg, (size_t)msglen, ctx, ctxlen);
}

int
crypto_sign_ed301_verify_detached(const unsigned char *sig,
    const unsigned char *msg, unsigned long long msglen,
    const unsigned char *pk)
{
	return crypto_sign_ed301_verify_detached_ctx(sig, msg, msglen,
	    NULL, 0, pk);
}

int
crypto_x301_public(unsigned char *public, const unsigned char *secret)
{
	return ed301_ssh_x_public(public, secret, crypto_x301_SECRETBYTES);
}

int
crypto_x301_shared(unsigned char *shared, const unsigned char *secret,
    const unsigned char *peer)
{
	return ed301_ssh_x_shared(shared, secret, crypto_x301_SECRETBYTES,
	    peer, crypto_x301_PUBLICBYTES);
}

int
crypto_x301_keypair(unsigned char *public, unsigned char *secret)
{
	u_char candidate[crypto_x301_SECRETBYTES];
	u_char result[crypto_x301_PUBLICBYTES];
	int r = -1;

	if (public == NULL || secret == NULL)
		goto out;
	do {
		arc4random_buf(candidate, sizeof(candidate));
	} while (ed301_ssh_x_is_weak(candidate, sizeof(candidate)) == -1);
	/* A public derivation failure is never retried or replaced. */
	if (ed301_ssh_x_public(result, candidate, sizeof(candidate)) != 0)
		goto out;
	memcpy(public, result, sizeof(result));
	memcpy(secret, candidate, sizeof(candidate));
	r = 0;
 out:
	explicit_bzero(candidate, sizeof(candidate));
	if (r != 0) {
		if (public != NULL)
			explicit_bzero(public, crypto_x301_PUBLICBYTES);
		if (secret != NULL)
			explicit_bzero(secret, crypto_x301_SECRETBYTES);
	}
	return r;
}
#endif /* WITH_ED301 && USE_ED301_RUST */
