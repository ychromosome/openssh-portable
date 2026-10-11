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
#include <limits.h>

#include "crypto_api.h"

#include <string.h>
#include <stdarg.h>

#include "log.h"
#include "sshbuf.h"
#define SSHKEY_INTERNAL
#include "sshkey.h"
#include "ssherr.h"

static void
ssh_ed301_cleanup(struct sshkey *k)
{
	freezero(k->ed301_pk, ED301_PK_SZ);
	freezero(k->ed301_sk, ED301_SK_SZ);
	k->ed301_pk = NULL;
	k->ed301_sk = NULL;
}

static int
ssh_ed301_equal(const struct sshkey *a, const struct sshkey *b)
{
	if (a->ed301_pk == NULL || b->ed301_pk == NULL)
		return 0;
	if (memcmp(a->ed301_pk, b->ed301_pk, ED301_PK_SZ) != 0)
		return 0;
	return 1;
}

static int
ssh_ed301_serialize_public(const struct sshkey *key, struct sshbuf *b,
    enum sshkey_serialize_rep opts)
{
	int r;

	if (key->ed301_pk == NULL)
		return SSH_ERR_INVALID_ARGUMENT;
	if ((r = sshbuf_put_string(b, key->ed301_pk, ED301_PK_SZ)) != 0)
		return r;

	return 0;
}

static int
ssh_ed301_serialize_private(const struct sshkey *key, struct sshbuf *b,
    enum sshkey_serialize_rep opts)
{
	int r;

	if ((r = sshbuf_put_string(b, key->ed301_pk, ED301_PK_SZ)) != 0 ||
	    (r = sshbuf_put_string(b, key->ed301_sk, ED301_SK_SZ)) != 0)
		return r;

	return 0;
}

static int
ssh_ed301_generate(struct sshkey *k, int bits)
{
	if (!ssh_ed301_available())
		return SSH_ERR_FEATURE_UNSUPPORTED;
	if ((k->ed301_pk = malloc(ED301_PK_SZ)) == NULL ||
	    (k->ed301_sk = malloc(ED301_SK_SZ)) == NULL)
		return SSH_ERR_ALLOC_FAIL;
	if (crypto_sign_ed301_keypair(k->ed301_pk, k->ed301_sk) != 0)
		return SSH_ERR_CRYPTO_ERROR;
	return 0;
}

static int
ssh_ed301_copy_public(const struct sshkey *from, struct sshkey *to)
{
	if (from->ed301_pk == NULL)
		return 0; /* XXX SSH_ERR_INTERNAL_ERROR ? */
	if ((to->ed301_pk = malloc(ED301_PK_SZ)) == NULL)
		return SSH_ERR_ALLOC_FAIL;
	memcpy(to->ed301_pk, from->ed301_pk, ED301_PK_SZ);
	return 0;
}

static int
ssh_ed301_deserialize_public(const char *ktype, struct sshbuf *b,
    struct sshkey *key)
{
	u_char *pk = NULL;
	size_t len = 0;
	int r;

	if ((r = sshbuf_get_string(b, &pk, &len)) != 0)
		return r;
	if (len != ED301_PK_SZ) {
		freezero(pk, len);
		return SSH_ERR_INVALID_FORMAT;
	}
	key->ed301_pk = pk;
	return 0;
}

static int
ssh_ed301_deserialize_private(const char *ktype, struct sshbuf *b,
    struct sshkey *key)
{
	int r;
	size_t sklen = 0;
	u_char *sk = NULL;
	u_char derived_pk[ED301_PK_SZ], derived_sk[ED301_SK_SZ];

	if ((r = ssh_ed301_deserialize_public(NULL, b, key)) != 0)
		goto out;
	if ((r = sshbuf_get_string(b, &sk, &sklen)) != 0)
		goto out;
	if (sklen != ED301_SK_SZ) {
		r = SSH_ERR_INVALID_FORMAT;
		goto out;
	}
	if (!ssh_ed301_available()) {
		r = SSH_ERR_FEATURE_UNSUPPORTED;
		goto out;
	}
	if (crypto_sign_ed301_seed_keypair(derived_pk, derived_sk, sk) != 0 ||
	    timingsafe_bcmp(key->ed301_pk, derived_pk, ED301_PK_SZ) != 0 ||
	    timingsafe_bcmp(sk + crypto_sign_ed301_SEEDBYTES, derived_pk,
	    ED301_PK_SZ) != 0) {
		r = SSH_ERR_INVALID_FORMAT;
		goto out;
	}
	key->ed301_sk = sk;
	sk = NULL; /* transferred */
	r = 0;
 out:
	freezero(sk, sklen);
	explicit_bzero(derived_pk, sizeof(derived_pk));
	explicit_bzero(derived_sk, sizeof(derived_sk));
	return r;
}

static int
ssh_ed301_sign(struct sshkey *key,
    u_char **sigp, size_t *lenp,
    const u_char *data, size_t datalen,
    const char *alg, const char *sk_provider, const char *sk_pin, u_int compat)
{
	u_char sig[crypto_sign_ed301_BYTES];
	unsigned long long siglen = 0;
	int r, ret;

	if (lenp != NULL)
		*lenp = 0;
	if (sigp != NULL)
		*sigp = NULL;

	if (key == NULL ||
	    sshkey_type_plain(key->type) != KEY_ED301 ||
	    key->ed301_sk == NULL)
		return SSH_ERR_INVALID_ARGUMENT;

	if (!ssh_ed301_available())
		return SSH_ERR_FEATURE_UNSUPPORTED;

	if ((ret = crypto_sign_ed301_detached(sig, &siglen, data, datalen,
	    key->ed301_sk)) != 0 || siglen != sizeof(sig)) {
		r = SSH_ERR_INVALID_ARGUMENT; /* XXX better error? */
		goto out;
	}
	if ((r = ssh_ed301_encode_store_sig(sig, siglen,
	    sigp, lenp)) != 0)
		goto out;

	/* success */
	r = 0;
 out:
	explicit_bzero(sig, sizeof(sig));
	return r;
}

int
ssh_ed301_encode_store_sig(const u_char *sig, size_t slen,
    u_char **sigp, size_t *lenp)
{
	struct sshbuf *b = NULL;
	int r = -1;
	size_t len;

	if (lenp != NULL)
		*lenp = 0;
	if (sigp != NULL)
		*sigp = NULL;

	if (slen != crypto_sign_ed301_BYTES)
		return SSH_ERR_INVALID_ARGUMENT;

	/* encode signature */
	if ((b = sshbuf_new()) == NULL) {
		r = SSH_ERR_ALLOC_FAIL;
		goto out;
	}
	if ((r = sshbuf_put_cstring(b, "ssh-ed301@eirindeval.de")) != 0 ||
	    (r = sshbuf_put_string(b, sig, slen)) != 0)
		goto out;
	len = sshbuf_len(b);
	if (sigp != NULL) {
		if ((*sigp = malloc(len)) == NULL) {
			r = SSH_ERR_ALLOC_FAIL;
			goto out;
		}
		memcpy(*sigp, sshbuf_ptr(b), len);
	}
	if (lenp != NULL)
		*lenp = len;
	/* success */
	r = 0;
 out:
	sshbuf_free(b);
	return r;
}

static int
ssh_ed301_verify(const struct sshkey *key,
    const u_char *sig, size_t siglen,
    const u_char *data, size_t dlen, const char *alg, u_int compat,
    struct sshkey_sig_details **detailsp)
{
	struct sshbuf *b = NULL;
	char *ktype = NULL;
	const u_char *sigblob;
	size_t len;
	int r, ret;

	if (key == NULL ||
	    sshkey_type_plain(key->type) != KEY_ED301 ||
	    key->ed301_pk == NULL ||
	    sig == NULL || siglen == 0)
		return SSH_ERR_INVALID_ARGUMENT;

	if ((b = sshbuf_from(sig, siglen)) == NULL)
		return SSH_ERR_ALLOC_FAIL;
	if ((r = sshbuf_get_cstring(b, &ktype, NULL)) != 0 ||
	    (r = sshbuf_get_string_direct(b, &sigblob, &len)) != 0)
		goto out;
	if (strcmp("ssh-ed301@eirindeval.de", ktype) != 0) {
		r = SSH_ERR_KEY_TYPE_MISMATCH;
		goto out;
	}
	if (sshbuf_len(b) != 0) {
		r = SSH_ERR_UNEXPECTED_TRAILING_DATA;
		goto out;
	}
	if (len != crypto_sign_ed301_BYTES) {
		r = SSH_ERR_INVALID_FORMAT;
		goto out;
	}
	if (!ssh_ed301_available()) {
		r = SSH_ERR_FEATURE_UNSUPPORTED;
		goto out;
	}
	if ((ret = crypto_sign_ed301_verify_detached(sigblob, data, dlen,
	    key->ed301_pk)) != 0) {
		debug2_f("crypto_sign_ed301_verify_detached failed: %d", ret);
	}
	if (ret != 0) {
		r = SSH_ERR_SIGNATURE_INVALID;
		goto out;
	}
	/* success */
	r = 0;
 out:
	sshbuf_free(b);
	free(ktype);
	return r;
}

/* Callback family shared by ordinary and certificate keys. */
static const struct sshkey_impl_funcs sshkey_ed301_funcs = {
	/* .size = */		NULL,
	/* .alloc = */		NULL,
	/* .cleanup = */	ssh_ed301_cleanup,
	/* .equal = */		ssh_ed301_equal,
	/* .ssh_serialize_public = */ ssh_ed301_serialize_public,
	/* .ssh_deserialize_public = */ ssh_ed301_deserialize_public,
	/* .ssh_serialize_private = */ ssh_ed301_serialize_private,
	/* .ssh_deserialize_private = */ ssh_ed301_deserialize_private,
	/* .generate = */	ssh_ed301_generate,
	/* .copy_public = */	ssh_ed301_copy_public,
	/* .sign = */		ssh_ed301_sign,
	/* .verify = */		ssh_ed301_verify,
};

const struct sshkey_impl sshkey_ed301_impl = {
	/* .name = */		"ssh-ed301@eirindeval.de",
	/* .shortname = */	"ED301",
	/* .sigalg = */		NULL,
	/* .type = */		KEY_ED301,
	/* .nid = */		0,
	/* .cert = */		0,
	/* .sigonly = */	0,
	/* .keybits = */	304,
	/* .funcs = */		&sshkey_ed301_funcs,
};

const struct sshkey_impl sshkey_ed301_cert_impl = {
	/* .name = */		"ssh-ed301-cert-v01@eirindeval.de",
	/* .shortname = */	"ED301-CERT",
	/* .sigalg = */		NULL,
	/* .type = */		KEY_ED301_CERT,
	/* .nid = */		0,
	/* .cert = */		1,
	/* .sigonly = */	0,
	/* .keybits = */	304,
	/* .funcs = */		&sshkey_ed301_funcs,
};

#endif /* WITH_ED301 */
