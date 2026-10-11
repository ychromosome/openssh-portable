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

#ifdef USE_MLKEM1024
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "log.h"
#include "crypto_api.h"
#include "libcrux_mlkem1024_internal.h"

int
crypto_kem_mlkem1024_keypair_seeded(
    uint8_t pk[crypto_kem_mlkem1024_PUBLICKEYBYTES],
    uint8_t sk[crypto_kem_mlkem1024_SECRETKEYBYTES], const uint8_t seed[64])
{
	Eurydice_arr_c7 rnd;
	libcrux_ml_kem_mlkem1024_MlKem1024KeyPair keypair;

	memcpy(rnd.data, seed, sizeof(rnd.data));
	keypair = libcrux_ml_kem_mlkem1024_portable_generate_key_pair(rnd);
	memcpy(pk, keypair.pk.data, sizeof(keypair.pk.data));
	memcpy(sk, keypair.sk.data, sizeof(keypair.sk.data));
	explicit_bzero(&keypair, sizeof(keypair));
	explicit_bzero(&rnd, sizeof(rnd));
	return 0;
}

int
crypto_kem_mlkem1024_keypair(uint8_t pk[crypto_kem_mlkem1024_PUBLICKEYBYTES],
    uint8_t sk[crypto_kem_mlkem1024_SECRETKEYBYTES])
{
	uint8_t seed[crypto_kem_mlkem1024_KEYPAIRSEEDBYTES];
	int r;

	arc4random_buf(seed, sizeof(seed));
	r = crypto_kem_mlkem1024_keypair_seeded(pk, sk, seed);
	explicit_bzero(seed, sizeof(seed));
	return r;
}

int
crypto_kem_mlkem1024_enc_seeded(uint8_t ct[crypto_kem_mlkem1024_CIPHERTEXTBYTES],
    uint8_t shared_secret[crypto_kem_mlkem1024_BYTES],
    const uint8_t pk[crypto_kem_mlkem1024_PUBLICKEYBYTES], const uint8_t seed[32])
{
	Eurydice_arr_d1 public;
	Eurydice_arr_ec rnd;
	tuple_25 enc;
	int r = -1;

	memcpy(public.data, pk, sizeof(public.data));
	if (!libcrux_ml_kem_mlkem1024_portable_validate_public_key(&public))
		goto out;
	memcpy(rnd.data, seed, sizeof(rnd.data));
	enc = libcrux_ml_kem_mlkem1024_portable_encapsulate(&public, rnd);
	memcpy(ct, enc.fst.data, sizeof(enc.fst.data));
	memcpy(shared_secret, enc.snd.data, sizeof(enc.snd.data));
	r = 0;
 out:
	explicit_bzero(&enc, sizeof(enc));
	explicit_bzero(&rnd, sizeof(rnd));
	if (r != 0) {
		explicit_bzero(ct, crypto_kem_mlkem1024_CIPHERTEXTBYTES);
		explicit_bzero(shared_secret, crypto_kem_mlkem1024_BYTES);
	}
	return r;
}

int
crypto_kem_mlkem1024_enc(uint8_t ct[crypto_kem_mlkem1024_CIPHERTEXTBYTES],
    uint8_t shared_secret[crypto_kem_mlkem1024_BYTES],
    const uint8_t pk[crypto_kem_mlkem1024_PUBLICKEYBYTES])
{
	uint8_t seed[crypto_kem_mlkem1024_ENCSEEDBYTES];
	int r;

	arc4random_buf(seed, sizeof(seed));
	r = crypto_kem_mlkem1024_enc_seeded(ct, shared_secret, pk, seed);
	explicit_bzero(seed, sizeof(seed));
	return r;
}

int
crypto_kem_mlkem1024_dec(uint8_t shared_secret[crypto_kem_mlkem1024_BYTES],
    const uint8_t ct[crypto_kem_mlkem1024_CIPHERTEXTBYTES],
    const uint8_t sk[crypto_kem_mlkem1024_SECRETKEYBYTES])
{
	Eurydice_arr_a8 secret;
	Eurydice_arr_d1 ciphertext;
	Eurydice_arr_ec ss;

	memcpy(secret.data, sk, sizeof(secret.data));
	memcpy(ciphertext.data, ct, sizeof(ciphertext.data));
	ss = libcrux_ml_kem_mlkem1024_portable_decapsulate(&secret, &ciphertext);
	memcpy(shared_secret, ss.data, sizeof(ss.data));
	explicit_bzero(&secret, sizeof(secret));
	explicit_bzero(&ss, sizeof(ss));
	return 0;
}
#endif /* USE_MLKEM1024 */
