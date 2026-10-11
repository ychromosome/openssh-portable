/*
 * Copyright (c) 2026 Damien Miller <djm@mindrot.org>
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

/* PROTOCOL.ed301, section 3 */

#include "includes.h"

#ifdef WITH_MLDSA65_ED301

#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "crypto_api.h"
#include "sshbuf.h"
#include "ssherr.h"
#include "digest.h"
#define SSHKEY_INTERNAL
#include "sshkey.h"
#include "log.h"

#define COMPOSITE_PREFIX "CompositeAlgorithmSignatures2025"
#define COMPOSITE_LABEL  "COMPSIG-MLDSA65-Ed301-SHA512"
#define SSH_MLDSA65_ED301_ALG_NAME   "ssh-mldsa65-ed301@eirindeval.de"

/*
 * raw_* functions implement the draft-ietf-lamps-pq-composite-sigs-18
 * composite signature scheme. These are exposed (i.e. not static) so
 * we can test them separately in unittests/crypto.
 */

int
crypto_sign_mldsa65_ed301_keygen(uint8_t pk[MLDSA65_ED301_PK_SZ],
    uint8_t sk[MLDSA65_ED301_SK_SZ])
{
	uint8_t mldsa_seed[MLDSA65_SEEDBYTES];
	uint8_t ed301_seed[crypto_sign_ed301_SEEDBYTES];
	int r;

	arc4random_buf(mldsa_seed, sizeof(mldsa_seed));
	arc4random_buf(ed301_seed, sizeof(ed301_seed));

	r = crypto_sign_mldsa65_ed301_keygen_seeded(pk, sk, mldsa_seed,
	    ed301_seed);
	explicit_bzero(mldsa_seed, sizeof(mldsa_seed));
	explicit_bzero(ed301_seed, sizeof(ed301_seed));
	return r;
}

int
crypto_sign_mldsa65_ed301_keygen_seeded(uint8_t pk[MLDSA65_ED301_PK_SZ],
    uint8_t sk[MLDSA65_ED301_SK_SZ],
    const uint8_t mldsa_seed[MLDSA65_SEEDBYTES],
    const uint8_t ed301_seed[crypto_sign_ed301_SEEDBYTES])
{
	uint8_t ed301_pk[crypto_sign_ed301_PUBLICKEYBYTES];
	uint8_t ed301_sk[crypto_sign_ed301_SECRETKEYBYTES];
	uint8_t mldsa_sk[MLDSA65_SECRETKEYBYTES];
	uint8_t public[MLDSA65_ED301_PK_SZ];
	int ret = -1;

	if (crypto_sign_mldsa65_keypair_seeded(public, mldsa_sk,
	    mldsa_seed) != 0)
		goto out;
	if (crypto_sign_ed301_seed_keypair(ed301_pk, ed301_sk,
	    ed301_seed) != 0)
		goto out;

	/* Serialize PK: mldsaPK || ed301PK */
	memcpy(public + MLDSA65_PUBLICKEYBYTES, ed301_pk,
	    crypto_sign_ed301_SEEDBYTES);

	memcpy(pk, public, sizeof(public));
	/* Serialize SK: mldsaSeed || ed301Seed */
	memcpy(sk, mldsa_seed, MLDSA65_SEEDBYTES);
	memcpy(sk + MLDSA65_SEEDBYTES, ed301_seed,
	    crypto_sign_ed301_SEEDBYTES);

	/* success */
	ret = 0;
 out:
	if (ret != 0) {
		explicit_bzero(pk, MLDSA65_ED301_PK_SZ);
		explicit_bzero(sk, MLDSA65_ED301_SK_SZ);
	}
	explicit_bzero(public, sizeof(public));
	explicit_bzero(mldsa_sk, sizeof(mldsa_sk));
	explicit_bzero(ed301_sk, sizeof(ed301_sk));
	return ret;
}

static int
construct_m_prime(uint8_t **m_primep, size_t *m_prime_lenp,
    const uint8_t *msg, size_t msglen,
    const uint8_t *ctx, size_t ctxlen)
{
	int r;
	uint8_t hash[64];
	struct sshbuf *m_prime;

	*m_primep = NULL;
	*m_prime_lenp = 0;

	if (ctxlen > 255 || (ctxlen != 0 && ctx == NULL) ||
	    (msglen != 0 && msg == NULL))
		return SSH_ERR_INVALID_ARGUMENT;
	if ((r = ssh_digest_memory(SSH_DIGEST_SHA512, msg, msglen,
	    hash, sizeof(hash))) != 0)
		return r;
	if ((m_prime = sshbuf_new()) == NULL)
		return SSH_ERR_ALLOC_FAIL;
	if ((r = sshbuf_put(m_prime, COMPOSITE_PREFIX,
	    sizeof(COMPOSITE_PREFIX) - 1)) != 0 ||
	    (r = sshbuf_put(m_prime, COMPOSITE_LABEL,
	    sizeof(COMPOSITE_LABEL) - 1)) != 0 ||
	    (r = sshbuf_put_u8(m_prime, (uint8_t)ctxlen)) != 0 ||
	    (r = sshbuf_put(m_prime, ctx, ctxlen)) != 0 ||
	    (r = sshbuf_put(m_prime, hash, sizeof(hash))) != 0) {
		sshbuf_free(m_prime);
		return r;
	}
	if ((*m_primep = malloc(sshbuf_len(m_prime))) == NULL) {
		sshbuf_free(m_prime);
		return SSH_ERR_ALLOC_FAIL;
	}
	memcpy(*m_primep, sshbuf_ptr(m_prime), sshbuf_len(m_prime));
	*m_prime_lenp = sshbuf_len(m_prime);
	/* success */
	sshbuf_free(m_prime);
	return 0;
}

int
crypto_sign_mldsa65_ed301_sign(uint8_t sig[MLDSA65_ED301_SIG_SZ],
    const uint8_t *msg, size_t msglen,
    const uint8_t *ctx, size_t ctxlen,
    const uint8_t sk[MLDSA65_ED301_SK_SZ])
{
	uint8_t *m_prime = NULL;
	size_t m_prime_len = 0;
	uint8_t mldsa_sk[MLDSA65_SECRETKEYBYTES];
	uint8_t mldsa_pk_dummy[MLDSA65_PUBLICKEYBYTES];
	uint8_t ed301_pk[crypto_sign_ed301_PUBLICKEYBYTES];
	uint8_t ed301_sk[crypto_sign_ed301_SECRETKEYBYTES];
	uint8_t mldsa_rnd[MLDSA65_SEEDBYTES];
	uint8_t signature[MLDSA65_ED301_SIG_SZ];
	unsigned long long smlen;
	int r = -1;

	if (construct_m_prime(&m_prime, &m_prime_len, msg, msglen,
	    ctx, ctxlen) != 0)
		goto out;

	/* Expand ML-DSA key from seed */
	if (crypto_sign_mldsa65_keypair_seeded(mldsa_pk_dummy,
	    mldsa_sk, sk) != 0)
		goto out;

	/* Sign with ML-DSA */
	arc4random_buf(mldsa_rnd, sizeof(mldsa_rnd));
	if (crypto_sign_mldsa65_seeded(signature, m_prime, m_prime_len,
	    (const uint8_t *)COMPOSITE_LABEL, sizeof(COMPOSITE_LABEL) - 1,
	    mldsa_sk, mldsa_rnd) != 0)
		goto out;

	/* Expand Ed301 key from seed */
	if (crypto_sign_ed301_seed_keypair(ed301_pk, ed301_sk,
	    sk + MLDSA65_SEEDBYTES) != 0)
		goto out;

	/* Sign with Ed301 */
	if (crypto_sign_ed301_detached_ctx(signature + MLDSA65_SIGBYTES,
	    &smlen, m_prime, m_prime_len,
	    (const uint8_t *)COMPOSITE_LABEL, sizeof(COMPOSITE_LABEL) - 1,
	    ed301_sk) != 0 ||
	    smlen != crypto_sign_ed301_BYTES)
		goto out;

	memcpy(sig, signature, sizeof(signature));
	r = 0;
 out:
	if (r != 0)
		explicit_bzero(sig, MLDSA65_ED301_SIG_SZ);
	explicit_bzero(signature, sizeof(signature));
	free(m_prime);
	explicit_bzero(mldsa_rnd, sizeof(mldsa_rnd));
	explicit_bzero(mldsa_sk, sizeof(mldsa_sk));
	explicit_bzero(ed301_sk, sizeof(ed301_sk));
	return r;
}

int
crypto_sign_mldsa65_ed301_verify(const uint8_t sig[MLDSA65_ED301_SIG_SZ],
    const uint8_t *msg, size_t msglen,
    const uint8_t *ctx, size_t ctxlen,
    const uint8_t pk[MLDSA65_ED301_PK_SZ])
{
	uint8_t *m_prime = NULL;
	size_t m_prime_len = 0;
	int r = -1;

	if (construct_m_prime(&m_prime, &m_prime_len, msg, msglen,
	    ctx, ctxlen) != 0)
		return -1;

	/* Verify ML-DSA */
	if (crypto_sign_mldsa65_verify(sig, m_prime, m_prime_len,
	    (const uint8_t *)COMPOSITE_LABEL, sizeof(COMPOSITE_LABEL) - 1,
	    pk) != 0)
		goto out;

	/* Verify Ed301 */
	if (crypto_sign_ed301_verify_detached_ctx(sig + MLDSA65_SIGBYTES,
	    m_prime, m_prime_len,
	    (const uint8_t *)COMPOSITE_LABEL, sizeof(COMPOSITE_LABEL) - 1,
	    pk + MLDSA65_PUBLICKEYBYTES) != 0)
		goto out;

	r = 0;
 out:
	free(m_prime);
	return r;
}

/* sshkey integration */

static void
ssh_mldsa65_ed301_cleanup(struct sshkey *k)
{
	freezero(k->mldsa65_ed301_pk, MLDSA65_ED301_PK_SZ);
	freezero(k->mldsa65_ed301_sk, MLDSA65_ED301_SK_SZ);
	k->mldsa65_ed301_pk = NULL;
	k->mldsa65_ed301_sk = NULL;
}

static int
ssh_mldsa65_ed301_equal(const struct sshkey *a, const struct sshkey *b)
{
	if (a->mldsa65_ed301_pk == NULL || b->mldsa65_ed301_pk == NULL)
		return 0;
	if (memcmp(a->mldsa65_ed301_pk, b->mldsa65_ed301_pk,
	    MLDSA65_ED301_PK_SZ) != 0)
		return 0;
	return 1;
}

static int
ssh_mldsa65_ed301_serialize_public(const struct sshkey *key, struct sshbuf *b,
    enum sshkey_serialize_rep opts)
{
	int r;

	if (key->mldsa65_ed301_pk == NULL)
		return SSH_ERR_INVALID_ARGUMENT;
	if ((r = sshbuf_put_string(b, key->mldsa65_ed301_pk,
	    MLDSA65_ED301_PK_SZ)) != 0)
		return r;

	return 0;
}

static int
ssh_mldsa65_ed301_serialize_private(const struct sshkey *key, struct sshbuf *b,
    enum sshkey_serialize_rep opts)
{
	int r;

	if (key->mldsa65_ed301_sk == NULL)
		return SSH_ERR_INVALID_ARGUMENT;
	if (!sshkey_is_cert(key)) {
		if ((r = ssh_mldsa65_ed301_serialize_public(key,
		    b, opts)) != 0)
			return r;
	}
	if ((r = sshbuf_put_string(b, key->mldsa65_ed301_sk,
	    MLDSA65_ED301_SK_SZ)) != 0)
		return r;

	return 0;
}

static int
ssh_mldsa65_ed301_deserialize_public(const char *ktype, struct sshbuf *b,
    struct sshkey *key)
{
	u_char *pk = NULL;
	size_t len = 0;
	int r;

	if ((r = sshbuf_get_string(b, &pk, &len)) != 0)
		return r;
	if (len != MLDSA65_ED301_PK_SZ) {
		freezero(pk, len);
		return SSH_ERR_INVALID_FORMAT;
	}
	key->mldsa65_ed301_pk = pk;
	return 0;
}

static int
ssh_mldsa65_ed301_deserialize_private(const char *ktype, struct sshbuf *b,
    struct sshkey *key)
{
	int r;
	size_t sklen = 0;
	u_char *sk = NULL;
	u_char pk[MLDSA65_ED301_PK_SZ], derived[MLDSA65_ED301_SK_SZ];

	if (!ssh_ed301_available())
		return SSH_ERR_FEATURE_UNSUPPORTED;
	if (!sshkey_is_cert(key)) {
		if ((r = ssh_mldsa65_ed301_deserialize_public(ktype,
		    b, key)) != 0)
			return r;
	}
	if ((r = sshbuf_get_string(b, &sk, &sklen)) != 0)
		goto out;
	if (sklen != MLDSA65_ED301_SK_SZ) {
		r = SSH_ERR_INVALID_FORMAT;
		goto out;
	}
	if (crypto_sign_mldsa65_ed301_keygen_seeded(pk, derived, sk,
	    sk + MLDSA65_SEEDBYTES) != 0) {
		r = SSH_ERR_CRYPTO_ERROR;
		goto out;
	}
	if (key->mldsa65_ed301_pk == NULL ||
	    timingsafe_bcmp(pk, key->mldsa65_ed301_pk, sizeof(pk)) != 0) {
		r = SSH_ERR_INVALID_FORMAT;
		goto out;
	}
	key->mldsa65_ed301_sk = sk;
	sk = NULL; /* transferred */
	r = 0;
 out:
	explicit_bzero(derived, sizeof(derived));
	explicit_bzero(pk, sizeof(pk));
	freezero(sk, sklen);
	return r;
}

static int
ssh_mldsa65_ed301_generate(struct sshkey *k, int bits)
{
	if (!ssh_ed301_available())
		return SSH_ERR_FEATURE_UNSUPPORTED;
	ssh_mldsa65_ed301_cleanup(k);
	if ((k->mldsa65_ed301_pk = malloc(MLDSA65_ED301_PK_SZ)) == NULL ||
	    (k->mldsa65_ed301_sk = malloc(MLDSA65_ED301_SK_SZ)) == NULL) {
		ssh_mldsa65_ed301_cleanup(k);
		return SSH_ERR_ALLOC_FAIL;
	}
	if (crypto_sign_mldsa65_ed301_keygen(k->mldsa65_ed301_pk,
	    k->mldsa65_ed301_sk) != 0) {
		ssh_mldsa65_ed301_cleanup(k);
		return SSH_ERR_CRYPTO_ERROR;
	}
	return 0;
}

static int
ssh_mldsa65_ed301_copy_public(const struct sshkey *from, struct sshkey *to)
{
	if (from->mldsa65_ed301_pk == NULL)
		return SSH_ERR_INVALID_ARGUMENT;
	if ((to->mldsa65_ed301_pk = malloc(MLDSA65_ED301_PK_SZ)) == NULL)
		return SSH_ERR_ALLOC_FAIL;
	memcpy(to->mldsa65_ed301_pk, from->mldsa65_ed301_pk,
	    MLDSA65_ED301_PK_SZ);
	return 0;
}

static int
ssh_mldsa65_ed301_sign(struct sshkey *key,
    u_char **sigp, size_t *lenp, const u_char *data, size_t datalen,
    const char *alg, const char *sk_provider, const char *sk_pin,
    u_int compat)
{
	u_char sig[MLDSA65_ED301_SIG_SZ];
	struct sshbuf *b = NULL;
	int r = SSH_ERR_INTERNAL_ERROR;

	if (lenp != NULL)
		*lenp = 0;
	if (sigp != NULL)
		*sigp = NULL;

	if (key == NULL ||
	    sshkey_type_plain(key->type) != KEY_MLDSA65_ED301 ||
	    key->mldsa65_ed301_sk == NULL)
		return SSH_ERR_INVALID_ARGUMENT;

	if (!ssh_ed301_available())
		return SSH_ERR_FEATURE_UNSUPPORTED;
	if (crypto_sign_mldsa65_ed301_sign(sig, data, datalen, NULL, 0,
	    key->mldsa65_ed301_sk) != 0) {
		r = SSH_ERR_CRYPTO_ERROR;
		goto out;
	}

	if ((b = sshbuf_new()) == NULL) {
		r = SSH_ERR_ALLOC_FAIL;
		goto out;
	}
	if ((r = sshbuf_put_cstring(b, SSH_MLDSA65_ED301_ALG_NAME)) != 0 ||
	    (r = sshbuf_put_string(b, sig, sizeof(sig))) != 0)
		goto out;

	if (sigp != NULL) {
		if ((*sigp = malloc(sshbuf_len(b))) == NULL) {
			r = SSH_ERR_ALLOC_FAIL;
			goto out;
		}
		memcpy(*sigp, sshbuf_ptr(b), sshbuf_len(b));
	}
	if (lenp != NULL)
		*lenp = sshbuf_len(b);
	r = 0;
 out:
	sshbuf_free(b);
	explicit_bzero(sig, sizeof(sig));
	return r;
}

static int
ssh_mldsa65_ed301_verify(const struct sshkey *key,
    const u_char *sig, size_t siglen, const u_char *data, size_t dlen,
    const char *alg, u_int compat, struct sshkey_sig_details **detailsp)
{
	struct sshbuf *b = NULL;
	char *ktype = NULL;
	const u_char *sigblob;
	size_t len;
	int r;

	if (key == NULL ||
	    sshkey_type_plain(key->type) != KEY_MLDSA65_ED301 ||
	    key->mldsa65_ed301_pk == NULL ||
	    sig == NULL || siglen == 0)
		return SSH_ERR_INVALID_ARGUMENT;

	if (!ssh_ed301_available())
		return SSH_ERR_FEATURE_UNSUPPORTED;
	if ((b = sshbuf_from(sig, siglen)) == NULL)
		return SSH_ERR_ALLOC_FAIL;
	if ((r = sshbuf_get_cstring(b, &ktype, NULL)) != 0 ||
	    (r = sshbuf_get_string_direct(b, &sigblob, &len)) != 0)
		goto out;
	if (strcmp(SSH_MLDSA65_ED301_ALG_NAME, ktype) != 0) {
		r = SSH_ERR_KEY_TYPE_MISMATCH;
		goto out;
	}
	if (sshbuf_len(b) != 0) {
		r = SSH_ERR_UNEXPECTED_TRAILING_DATA;
		goto out;
	}
	if (len != MLDSA65_ED301_SIG_SZ) {
		r = SSH_ERR_INVALID_FORMAT;
		goto out;
	}

	if (crypto_sign_mldsa65_ed301_verify(sigblob, data, dlen, NULL, 0,
	    key->mldsa65_ed301_pk) != 0) {
		r = SSH_ERR_SIGNATURE_INVALID;
		goto out;
	}

	r = 0;
 out:
	sshbuf_free(b);
	free(ktype);
	return r;
}

const struct sshkey_impl_funcs sshkey_mldsa65_ed301_funcs = {
	/* .size = */		NULL,
	/* .alloc = */		NULL,
	/* .cleanup = */	ssh_mldsa65_ed301_cleanup,
	/* .equal = */		ssh_mldsa65_ed301_equal,
	/* .ssh_serialize_public = */ ssh_mldsa65_ed301_serialize_public,
	/* .ssh_deserialize_public = */ ssh_mldsa65_ed301_deserialize_public,
	/* .ssh_serialize_private = */ ssh_mldsa65_ed301_serialize_private,
	/* .ssh_deserialize_private = */ ssh_mldsa65_ed301_deserialize_private,
	/* .generate = */	ssh_mldsa65_ed301_generate,
	/* .copy_public = */	ssh_mldsa65_ed301_copy_public,
	/* .sign = */		ssh_mldsa65_ed301_sign,
	/* .verify = */		ssh_mldsa65_ed301_verify,
};

const struct sshkey_impl sshkey_mldsa65_ed301_impl = {
	/* .name = */		"ssh-mldsa65-ed301@eirindeval.de",
	/* .shortname = */	"MLDSA65-ED301",
	/* .sigalg = */		NULL,
	/* .type = */		KEY_MLDSA65_ED301,
	/* .nid = */		0,
	/* .cert = */		0,
	/* .sigonly = */	0,
	/* .keybits = */	560, /* size of the private key */
	/* .funcs = */		&sshkey_mldsa65_ed301_funcs,
};

const struct sshkey_impl sshkey_mldsa65_ed301_cert_impl = {
	/* .name = */		"ssh-mldsa65-ed301-cert-v01@eirindeval.de",
	/* .shortname = */	"MLDSA65-ED301-CERT",
	/* .sigalg = */		NULL,
	/* .type = */		KEY_MLDSA65_ED301_CERT,
	/* .nid = */		0,
	/* .cert = */		1,
	/* .sigonly = */	0,
	/* .keybits = */	560, /* size of the private key */
	/* .funcs = */		&sshkey_mldsa65_ed301_funcs,
};
#endif /* WITH_MLDSA65_ED301 */
