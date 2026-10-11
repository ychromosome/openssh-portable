/*
 * Copyright (c) 2019 Markus Friedl.  All rights reserved.
 * Copyright (c) 2010 Damien Miller.  All rights reserved.
 * Copyright (c) 2013 Aris Adamantiadis.  All rights reserved.
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
#include <string.h>
#include <signal.h>

#include "sshkey.h"
#include "kex.h"
#include "sshbuf.h"
#include "digest.h"
#include "ssherr.h"
#include "ssh2.h"

#ifdef WITH_ED301

int
kexx301_shared_key_ext(const u_char key[crypto_x301_SECRETBYTES],
    const u_char pub[crypto_x301_PUBLICBYTES], struct sshbuf *out, int raw)
{
	u_char shared_key[crypto_x301_PUBLICBYTES];
	u_char zero[crypto_x301_PUBLICBYTES];
	int r;

	if (crypto_x301_shared(shared_key, key, pub) != 0) {
		r = SSH_ERR_KEY_INVALID_EC_VALUE;
		goto out;
	}

	/* Check for all-zero shared secret */
	explicit_bzero(zero, crypto_x301_PUBLICBYTES);
	if (timingsafe_bcmp(zero, shared_key, crypto_x301_PUBLICBYTES) == 0) {
		r = SSH_ERR_KEY_INVALID_EC_VALUE;
		goto out;
	}

#ifdef DEBUG_KEXECDH
	dump_digest("shared secret X301", shared_key, crypto_x301_PUBLICBYTES);
#endif
	if (raw)
		r = sshbuf_put(out, shared_key, crypto_x301_PUBLICBYTES);
	else
		r = sshbuf_put_bignum2_bytes(out, shared_key, crypto_x301_PUBLICBYTES);
 out:
	explicit_bzero(shared_key, sizeof(shared_key));
	return r;
}

int
kexx301_shared_key(const u_char key[crypto_x301_SECRETBYTES],
    const u_char pub[crypto_x301_PUBLICBYTES], struct sshbuf *out)
{
	return kexx301_shared_key_ext(key, pub, out, 0);
}

int
kex_x301_keypair(struct kex *kex)
{
	struct sshbuf *buf = NULL;
	u_char *cp = NULL;
	int r;

	if ((buf = sshbuf_new()) == NULL)
		return SSH_ERR_ALLOC_FAIL;
	if ((r = sshbuf_reserve(buf, crypto_x301_PUBLICBYTES, &cp)) != 0)
		goto out;
	if (crypto_x301_keypair(cp, kex->x301_client_key) != 0) {
		r = SSH_ERR_LIBCRYPTO_ERROR;
		goto out;
	}
#ifdef DEBUG_KEXECDH
	dump_digest("client public key X301:", cp, crypto_x301_PUBLICBYTES);
#endif
	kex->client_pub = buf;
	buf = NULL;
 out:
	if (r != 0)
		explicit_bzero(kex->x301_client_key,
		    sizeof(kex->x301_client_key));
	sshbuf_free(buf);
	return r;
}

int
kex_x301_enc(struct kex *kex, const struct sshbuf *client_blob,
   struct sshbuf **server_blobp, struct sshbuf **shared_secretp)
{
	struct sshbuf *server_blob = NULL;
	struct sshbuf *buf = NULL;
	const u_char *client_pub;
	u_char *server_pub;
	u_char server_key[crypto_x301_SECRETBYTES];
	int r;

	*server_blobp = NULL;
	*shared_secretp = NULL;

	if (sshbuf_len(client_blob) != crypto_x301_PUBLICBYTES) {
		r = SSH_ERR_SIGNATURE_INVALID;
		goto out;
	}
	client_pub = sshbuf_ptr(client_blob);
#ifdef DEBUG_KEXECDH
	dump_digest("client public key X301:", client_pub, crypto_x301_PUBLICBYTES);
#endif
	/* allocate space for encrypted KEM key and ECDH pub key */
	if ((server_blob = sshbuf_new()) == NULL) {
		r = SSH_ERR_ALLOC_FAIL;
		goto out;
	}
	if ((r = sshbuf_reserve(server_blob, crypto_x301_PUBLICBYTES,
	    &server_pub)) != 0)
		goto out;
	if (crypto_x301_keypair(server_pub, server_key) != 0) {
		r = SSH_ERR_LIBCRYPTO_ERROR;
		goto out;
	}
	/* allocate shared secret */
	if ((buf = sshbuf_new()) == NULL) {
		r = SSH_ERR_ALLOC_FAIL;
		goto out;
	}
	if ((r = kexx301_shared_key_ext(server_key, client_pub, buf, 0)) < 0)
		goto out;
#ifdef DEBUG_KEXECDH
	dump_digest("server public key X301:", server_pub, crypto_x301_PUBLICBYTES);
	dump_digest("encoded shared secret:", sshbuf_ptr(buf), sshbuf_len(buf));
#endif
	*server_blobp = server_blob;
	*shared_secretp = buf;
	server_blob = NULL;
	buf = NULL;
 out:
	explicit_bzero(server_key, sizeof(server_key));
	sshbuf_free(server_blob);
	sshbuf_free(buf);
	return r;
}

int
kex_x301_dec(struct kex *kex, const struct sshbuf *server_blob,
    struct sshbuf **shared_secretp)
{
	struct sshbuf *buf = NULL;
	const u_char *server_pub;
	int r;

	*shared_secretp = NULL;

	if (sshbuf_len(server_blob) != crypto_x301_PUBLICBYTES) {
		r = SSH_ERR_SIGNATURE_INVALID;
		goto out;
	}
	server_pub = sshbuf_ptr(server_blob);
#ifdef DEBUG_KEXECDH
	dump_digest("server public key X301:", server_pub, crypto_x301_PUBLICBYTES);
#endif
	/* shared secret */
	if ((buf = sshbuf_new()) == NULL) {
		r = SSH_ERR_ALLOC_FAIL;
		goto out;
	}
	if ((r = kexx301_shared_key_ext(kex->x301_client_key, server_pub,
	    buf, 0)) < 0)
		goto out;
#ifdef DEBUG_KEXECDH
	dump_digest("encoded shared secret:", sshbuf_ptr(buf), sshbuf_len(buf));
#endif
	*shared_secretp = buf;
	buf = NULL;
 out:
	explicit_bzero(kex->x301_client_key, sizeof(kex->x301_client_key));
	sshbuf_free(buf);
	return r;
}

#endif /* WITH_ED301 */
