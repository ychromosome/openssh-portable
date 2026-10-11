/*
 * Copyright (c) 2023 Markus Friedl.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "includes.h"

#include <sys/types.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <endian.h>

#include "sshkey.h"
#include "kex.h"
#include "sshbuf.h"
#include "digest.h"
#include "ssherr.h"
#include "log.h"

#include "crypto_api.h"

#if defined(USE_MLKEM1024) && defined(WITH_ED301)

int
kex_kem_mlkem1024x301_keypair(struct kex *kex)
{
	struct sshbuf *buf = NULL;
	u_char *cp = NULL;
	size_t need;
	int r = SSH_ERR_INTERNAL_ERROR;

	if ((buf = sshbuf_new()) == NULL)
		return SSH_ERR_ALLOC_FAIL;
	need = MLKEM1024_PUBLICKEYBYTES + crypto_x301_PUBLICBYTES;
	if ((r = sshbuf_reserve(buf, need, &cp)) != 0)
		goto out;
	if (crypto_kem_mlkem1024_keypair(cp, kex->mlkem1024_client_key) != 0) {
		r = SSH_ERR_INTERNAL_ERROR;
		goto out;
	}
#ifdef DEBUG_KEXECDH
	dump_digest("client public key mlkem1024:", cp,
	    MLKEM1024_PUBLICKEYBYTES);
#endif
	cp += MLKEM1024_PUBLICKEYBYTES;
	if (crypto_x301_keypair(cp, kex->x301_client_key) != 0) {
		r = SSH_ERR_INTERNAL_ERROR;
		goto out;
	}
#ifdef DEBUG_KEXECDH
	dump_digest("client public key X301:", cp, crypto_x301_PUBLICBYTES);
#endif
	/* success */
	r = 0;
	kex->client_pub = buf;
	buf = NULL;
 out:
	if (r != 0) {
		explicit_bzero(kex->x301_client_key,
		    sizeof(kex->x301_client_key));
		explicit_bzero(kex->mlkem1024_client_key,
		    sizeof(kex->mlkem1024_client_key));
	}
	sshbuf_free(buf);
	return r;
}

int
kex_kem_mlkem1024x301_enc(struct kex *kex,
   const struct sshbuf *client_blob, struct sshbuf **server_blobp,
   struct sshbuf **shared_secretp)
{
	struct sshbuf *server_blob = NULL;
	struct sshbuf *buf = NULL;
	const u_char *client_pub;
	u_char server_pub[crypto_x301_PUBLICBYTES], server_key[crypto_x301_SECRETBYTES];
	u_char hash[SSH_DIGEST_MAX_LENGTH];
	u_char ct[MLKEM1024_CIPHERTEXTBYTES];
	u_char shared_secret[MLKEM1024_BYTES];
	size_t need;
	int r = SSH_ERR_INTERNAL_ERROR;

	*server_blobp = NULL;
	*shared_secretp = NULL;

	/* client_blob contains both KEM and ECDH client pubkeys */
	need = MLKEM1024_PUBLICKEYBYTES + crypto_x301_PUBLICBYTES;
	if (sshbuf_len(client_blob) != need) {
		r = SSH_ERR_SIGNATURE_INVALID;
		goto out;
	}
	client_pub = sshbuf_ptr(client_blob);
#ifdef DEBUG_KEXECDH
	dump_digest("client public key mlkem1024:", client_pub,
	    MLKEM1024_PUBLICKEYBYTES);
	dump_digest("client public key X301:",
	    client_pub + MLKEM1024_PUBLICKEYBYTES,
	    crypto_x301_PUBLICBYTES);
#endif

	/* allocate buffer for concatenation of KEM key and ECDH shared key */
	/* the buffer will be hashed and the result is the shared secret */
	if ((buf = sshbuf_new()) == NULL) {
		r = SSH_ERR_ALLOC_FAIL;
		goto out;
	}
	/* allocate space for encrypted KEM key and ECDH pub key */
	if ((server_blob = sshbuf_new()) == NULL) {
		r = SSH_ERR_ALLOC_FAIL;
		goto out;
	}
	/* generate and encrypt KEM key with client key */
	if (crypto_kem_mlkem1024_enc(ct, shared_secret, client_pub) != 0) {
		r = SSH_ERR_INTERNAL_ERROR;
		goto out;
	}
	/* generate ECDH key pair, store server pubkey after ciphertext */
	if (crypto_x301_keypair(server_pub, server_key) != 0) {
		r = SSH_ERR_INTERNAL_ERROR;
		goto out;
	}
	if ((r = sshbuf_put(buf, shared_secret, sizeof(shared_secret))) != 0 ||
	    (r = sshbuf_put(server_blob, ct, sizeof(ct))) != 0 ||
	    (r = sshbuf_put(server_blob, server_pub, sizeof(server_pub))) != 0)
		goto out;
	/* append ECDH shared key */
	client_pub += MLKEM1024_PUBLICKEYBYTES;
	if ((r = kexx301_shared_key_ext(server_key, client_pub, buf, 1)) < 0)
		goto out;
	if ((r = ssh_digest_buffer(kex->hash_alg, buf,
	    hash, sizeof(hash))) != 0)
		goto out;
#ifdef DEBUG_KEXECDH
	dump_digest("server public key X301:", server_pub, crypto_x301_PUBLICBYTES);
	dump_digest("server cipher text:", ct, sizeof(ct));
	dump_digest("server kem key:", shared_secret, sizeof(shared_secret));
	dump_digest("concatenation of KEM key and ECDH shared key:",
	    sshbuf_ptr(buf), sshbuf_len(buf));
#endif
	/* string-encoded hash is resulting shared secret */
	sshbuf_reset(buf);
	if ((r = sshbuf_put_string(buf, hash,
	    ssh_digest_bytes(kex->hash_alg))) != 0)
		goto out;
#ifdef DEBUG_KEXECDH
	dump_digest("encoded shared secret:", sshbuf_ptr(buf), sshbuf_len(buf));
#endif
	/* success */
	r = 0;
	*server_blobp = server_blob;
	*shared_secretp = buf;
	server_blob = NULL;
	buf = NULL;
 out:
	explicit_bzero(hash, sizeof(hash));
	explicit_bzero(server_key, sizeof(server_key));
	explicit_bzero(shared_secret, sizeof(shared_secret));
	sshbuf_free(server_blob);
	sshbuf_free(buf);
	return r;
}

int
kex_kem_mlkem1024x301_dec(struct kex *kex,
    const struct sshbuf *server_blob, struct sshbuf **shared_secretp)
{
	struct sshbuf *buf = NULL;
	u_char shared_secret[MLKEM1024_BYTES];
	const u_char *ciphertext, *server_pub;
	u_char hash[SSH_DIGEST_MAX_LENGTH];
	size_t need;
	int r;

	*shared_secretp = NULL;

	need = MLKEM1024_CIPHERTEXTBYTES + crypto_x301_PUBLICBYTES;
	if (sshbuf_len(server_blob) != need) {
		r = SSH_ERR_SIGNATURE_INVALID;
		goto out;
	}
	ciphertext = sshbuf_ptr(server_blob);
	server_pub = ciphertext + MLKEM1024_CIPHERTEXTBYTES;
	/* hash concatenation of KEM key and ECDH shared key */
	if ((buf = sshbuf_new()) == NULL) {
		r = SSH_ERR_ALLOC_FAIL;
		goto out;
	}
#ifdef DEBUG_KEXECDH
	dump_digest("server cipher text:", ciphertext,
	    MLKEM1024_CIPHERTEXTBYTES);
	dump_digest("server public key X301:", server_pub, crypto_x301_PUBLICBYTES);
#endif
	if (crypto_kem_mlkem1024_dec(shared_secret, ciphertext,
	    kex->mlkem1024_client_key) != 0) {
		r = SSH_ERR_INTERNAL_ERROR;
		goto out;
	}
	if ((r = sshbuf_put(buf, shared_secret, sizeof(shared_secret))) != 0)
		goto out;
	if ((r = kexx301_shared_key_ext(kex->x301_client_key, server_pub,
	    buf, 1)) < 0)
		goto out;
	if ((r = ssh_digest_buffer(kex->hash_alg, buf,
	    hash, sizeof(hash))) != 0)
		goto out;
#ifdef DEBUG_KEXECDH
	dump_digest("client kem key:", shared_secret, sizeof(shared_secret));
	dump_digest("concatenation of KEM key and ECDH shared key:",
	    sshbuf_ptr(buf), sshbuf_len(buf));
#endif
	sshbuf_reset(buf);
	if ((r = sshbuf_put_string(buf, hash,
	    ssh_digest_bytes(kex->hash_alg))) != 0)
		goto out;
#ifdef DEBUG_KEXECDH
	dump_digest("encoded shared secret:", sshbuf_ptr(buf), sshbuf_len(buf));
#endif
	/* success */
	r = 0;
	*shared_secretp = buf;
	buf = NULL;
 out:
	explicit_bzero(kex->x301_client_key, sizeof(kex->x301_client_key));
	explicit_bzero(kex->mlkem1024_client_key,
	    sizeof(kex->mlkem1024_client_key));
	explicit_bzero(hash, sizeof(hash));
	explicit_bzero(shared_secret, sizeof(shared_secret));
	sshbuf_free(buf);
	return r;
}
#endif /* USE_MLKEM1024 && WITH_ED301 */
