/* 	$OpenBSD: test_sshkey.c,v 1.34 2026/06/14 04:08:06 djm Exp $ */
/*
 * Regress test for sshkey.h key management API
 *
 * Placed in the public domain
 */

#include "includes.h"

#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef WITH_OPENSSL
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/rsa.h>
#endif

#include "../test_helper/test_helper.h"

#include "ssherr.h"
#include "sshbuf.h"
#define SSHBUF_INTERNAL 1	/* access internals for testing */
#include "sshkey.h"
#include "crypto_api.h"

#include "authfile.h"
#include "common.h"
#include "ssh2.h"

void sshkey_tests(void);
void sshkey_benchmarks(void);

static void
put_opt(struct sshbuf *b, const char *name, const char *value)
{
	struct sshbuf *sect;

	sect = sshbuf_new();
	ASSERT_PTR_NE(sect, NULL);
	ASSERT_INT_EQ(sshbuf_put_cstring(b, name), 0);
	if (value != NULL)
		ASSERT_INT_EQ(sshbuf_put_cstring(sect, value), 0);
	ASSERT_INT_EQ(sshbuf_put_stringb(b, sect), 0);
	sshbuf_free(sect);
}

#ifdef WITH_OPENSSL
static void
build_cert(struct sshbuf *b, struct sshkey *k, const char *type,
    struct sshkey *sign_key, struct sshkey *ca_key,
    const char *sig_alg)
{
	struct sshbuf *ca_buf, *pk, *principals, *critopts, *exts;
	u_char *sigblob;
	size_t siglen;

	ca_buf = sshbuf_new();
	ASSERT_PTR_NE(ca_buf, NULL);
	ASSERT_INT_EQ(sshkey_putb(ca_key, ca_buf), 0);

	/*
	 * Get the public key serialisation by rendering the key and skipping
	 * the type string. This is a bit of a hack :/
	 */
	pk = sshbuf_new();
	ASSERT_PTR_NE(pk, NULL);
	ASSERT_INT_EQ(sshkey_putb_plain(k, pk), 0);
	ASSERT_INT_EQ(sshbuf_skip_string(pk), 0);

	principals = sshbuf_new();
	ASSERT_PTR_NE(principals, NULL);
	ASSERT_INT_EQ(sshbuf_put_cstring(principals, "gsamsa"), 0);
	ASSERT_INT_EQ(sshbuf_put_cstring(principals, "gregor"), 0);

	critopts = sshbuf_new();
	ASSERT_PTR_NE(critopts, NULL);
	put_opt(critopts, "force-command", "/usr/local/bin/nethack");
	put_opt(critopts, "source-address", "192.168.0.0/24,127.0.0.1,::1");

	exts = sshbuf_new();
	ASSERT_PTR_NE(exts, NULL);
	put_opt(critopts, "permit-X11-forwarding", NULL);

	ASSERT_INT_EQ(sshbuf_put_cstring(b, type), 0);
	ASSERT_INT_EQ(sshbuf_put_cstring(b, "noncenoncenonce!"), 0); /* nonce */
	ASSERT_INT_EQ(sshbuf_putb(b, pk), 0); /* public key serialisation */
	ASSERT_INT_EQ(sshbuf_put_u64(b, 1234), 0); /* serial */
	ASSERT_INT_EQ(sshbuf_put_u32(b, SSH2_CERT_TYPE_USER), 0); /* type */
	ASSERT_INT_EQ(sshbuf_put_cstring(b, "gregor"), 0); /* key ID */
	ASSERT_INT_EQ(sshbuf_put_stringb(b, principals), 0); /* principals */
	ASSERT_INT_EQ(sshbuf_put_u64(b, 0), 0); /* start */
	ASSERT_INT_EQ(sshbuf_put_u64(b, 0xffffffffffffffffULL), 0); /* end */
	ASSERT_INT_EQ(sshbuf_put_stringb(b, critopts), 0); /* options */
	ASSERT_INT_EQ(sshbuf_put_stringb(b, exts), 0); /* extensions */
	ASSERT_INT_EQ(sshbuf_put_string(b, NULL, 0), 0); /* reserved */
	ASSERT_INT_EQ(sshbuf_put_stringb(b, ca_buf), 0); /* signature key */
	ASSERT_INT_EQ(sshkey_sign(sign_key, &sigblob, &siglen,
	    sshbuf_ptr(b), sshbuf_len(b), sig_alg, NULL, NULL, 0), 0);
	ASSERT_INT_EQ(sshbuf_put_string(b, sigblob, siglen), 0); /* signature */

	free(sigblob);
	sshbuf_free(ca_buf);
	sshbuf_free(exts);
	sshbuf_free(critopts);
	sshbuf_free(principals);
	sshbuf_free(pk);
}
#endif /* WITH_OPENSSL */

static void
signature_test(struct sshkey *k, struct sshkey *bad, const char *sig_alg,
    const u_char *d, size_t l)
{
	size_t len;
	u_char *sig;

	ASSERT_INT_EQ(sshkey_sign(k, &sig, &len, d, l, sig_alg,
	    NULL, NULL, 0), 0);
	ASSERT_SIZE_T_GT(len, 8);
	ASSERT_PTR_NE(sig, NULL);
	ASSERT_INT_EQ(sshkey_verify(k, sig, len, d, l, NULL, 0, NULL), 0);
	ASSERT_INT_NE(sshkey_verify(bad, sig, len, d, l, NULL, 0, NULL), 0);
	/* Fuzz test is more comprehensive, this is just a smoke test */
	sig[len - 5] ^= 0x10;
	ASSERT_INT_NE(sshkey_verify(k, sig, len, d, l, NULL, 0, NULL), 0);
	free(sig);
}

static void
signature_bench(const char *name, int ktype, int bits, const char *sig_alg,
    const u_char *d, size_t l)
{
	struct sshkey *k;
	size_t len;
	u_char *sig;
	char testname[256];

	snprintf(testname, sizeof(testname), "sign %s", name);
	TEST_START(testname);
	ASSERT_INT_EQ(sshkey_generate(ktype, bits, &k), 0);
	ASSERT_PTR_NE(k, NULL);

	BENCH_START(testname);
	ASSERT_INT_EQ(sshkey_sign(k, &sig, &len, d, l, sig_alg,
	    NULL, NULL, 0), 0);
	free(sig);
	BENCH_FINISH("sign");

	sshkey_free(k);
	TEST_DONE();
}

static void
verify_bench(const char *name, int ktype, int bits, const char *sig_alg,
    const u_char *d, size_t l)
{
	struct sshkey *k;
	size_t len;
	u_char *sig;
	char testname[256];

	snprintf(testname, sizeof(testname), "verify %s", name);
	TEST_START(testname);
	ASSERT_INT_EQ(sshkey_generate(ktype, bits, &k), 0);
	ASSERT_PTR_NE(k, NULL);

	ASSERT_INT_EQ(sshkey_sign(k, &sig, &len, d, l, sig_alg,
	    NULL, NULL, 0), 0);
	BENCH_START(testname);
	ASSERT_INT_EQ(sshkey_verify(k, sig, len, d, l, NULL, 0, NULL), 0);
	BENCH_FINISH("verify");

	free(sig);
	sshkey_free(k);
	TEST_DONE();
}

static void
banana(u_char *s, size_t l)
{
	size_t o;
	const u_char the_banana[] = { 'b', 'a', 'n', 'a', 'n', 'a' };

	for (o = 0; o < l; o += sizeof(the_banana)) {
		if (l - o < sizeof(the_banana)) {
			memcpy(s + o, "nanananana", l - o);
			break;
		}
		memcpy(s + o, the_banana, sizeof(the_banana));
	}
}

static void
signature_tests(struct sshkey *k, struct sshkey *bad, const char *sig_alg)
{
	u_char i, buf[2049];
	size_t lens[] = {
		1, 2, 7, 8, 9, 15, 16, 17, 31, 32, 33, 127, 128, 129,
		255, 256, 257, 1023, 1024, 1025, 2047, 2048, 2049
	};

	for (i = 0; i < (sizeof(lens)/sizeof(lens[0])); i++) {
		test_subtest_info("%s key, banana length %zu",
		    sshkey_type(k), lens[i]);
		banana(buf, lens[i]);
		signature_test(k, bad, sig_alg, buf, lens[i]);
	}
}

static void
signature_benchmark(const char *name, int ktype, int bits,
    const char *sig_alg, int bench_verify)
{
	u_char buf[256];

	banana(buf, sizeof(buf));
	if (bench_verify)
		verify_bench(name, ktype, bits, sig_alg, buf, sizeof(buf));
	else
		signature_bench(name, ktype, bits, sig_alg, buf, sizeof(buf));
}

static struct sshkey *
get_private(const char *n)
{
	struct sshbuf *b;
	struct sshkey *ret;

	b = load_file(n);
	ASSERT_INT_EQ(sshkey_parse_private_fileblob(b, "", &ret, NULL), 0);
	sshbuf_free(b);
	return ret;
}


#ifdef WITH_ED301
#ifdef WITH_OPENSSL
static void
ed301_pkcs8_tests(void)
{
	struct sshkey *key = NULL, *parsed = NULL, *roundtrip = NULL;
	struct sshkey *other = NULL;
	struct sshbuf *pem, *file;
	const char *passphrases[] = { "", "encrypted Ed301 PKCS8" };
	const u_char msg[] = "Ed301 PKCS8 roundtrip";
	u_char *sig = NULL;
	size_t siglen, i;

	if (!ssh_ed301_available()) {
		TEST_START("Ed301 PKCS8 unavailable without provider");
		ASSERT_PTR_NE(pem = sshbuf_new(), NULL);
		ASSERT_INT_EQ(sshkey_parse_private_fileblob_type(pem,
		    KEY_ED301, "", &parsed, NULL),
		    SSH_ERR_FEATURE_UNSUPPORTED);
		ASSERT_PTR_EQ(parsed, NULL);
		sshbuf_free(pem);
		TEST_DONE();
		return;
	}
	ASSERT_INT_EQ(sshkey_generate(KEY_ED301, 304, &key), 0);
	ASSERT_PTR_NE(pem = sshbuf_new(), NULL);
	ASSERT_PTR_NE(file = sshbuf_new(), NULL);
	for (i = 0; i < sizeof(passphrases) / sizeof(*passphrases); i++) {
		TEST_START("Ed301 OpenSSH / PKCS8 private roundtrip");
		test_subtest_info("encrypted %zu", i);
		ASSERT_INT_EQ(sshkey_private_to_fileblob(key, pem,
		    passphrases[i], "test", SSHKEY_PRIVATE_PKCS8,
		    NULL, 0), 0);
		ASSERT_INT_EQ(sshkey_parse_private_fileblob_type(pem,
		    KEY_ED301, passphrases[i], &parsed, NULL), 0);
		ASSERT_INT_EQ(sshkey_equal_public(key, parsed), 1);
		if (i != 0) {
			ASSERT_INT_EQ(sshkey_parse_private_fileblob_type(pem,
			    KEY_ED301, "wrong passphrase", &roundtrip, NULL),
			    SSH_ERR_KEY_WRONG_PASSPHRASE);
			ASSERT_PTR_EQ(roundtrip, NULL);
		}
		ASSERT_INT_EQ(sshkey_private_to_fileblob(parsed, file, "",
		    "test", SSHKEY_PRIVATE_OPENSSH, NULL, 0), 0);
		ASSERT_INT_EQ(sshkey_parse_private_fileblob(file, "",
		    &roundtrip, NULL), 0);
		ASSERT_INT_EQ(sshkey_equal_public(key, roundtrip), 1);
		ASSERT_INT_EQ(sshkey_sign(roundtrip, &sig, &siglen,
		    msg, sizeof(msg), NULL, NULL, NULL, 0), 0);
		ASSERT_INT_EQ(sshkey_verify(key, sig, siglen, msg,
		    sizeof(msg), NULL, 0, NULL), 0);
		free(sig);
		sshkey_free(parsed);
		sshkey_free(roundtrip);
		parsed = roundtrip = NULL;
		sshbuf_reset(pem);
		sshbuf_reset(file);
		TEST_DONE();
	}
	TEST_START("Ed301 refuses traditional PEM and wrong PKCS8 type");
	ASSERT_INT_EQ(sshkey_shield_private(key), 0);
	ASSERT_INT_EQ(sshkey_private_to_fileblob(key, pem, "", "test",
	    SSHKEY_PRIVATE_PEM, NULL, 0), SSH_ERR_INVALID_FORMAT);
	ASSERT_INT_EQ(sshkey_generate(KEY_ED25519, 256, &other), 0);
	ASSERT_INT_EQ(sshkey_private_to_fileblob(other, pem, "", "test",
	    SSHKEY_PRIVATE_PKCS8, NULL, 0), 0);
	ASSERT_INT_EQ(sshkey_parse_private_fileblob_type(pem, KEY_ED301,
	    "", &parsed, NULL), SSH_ERR_INVALID_FORMAT);
	ASSERT_PTR_EQ(parsed, NULL);
	sshkey_free(other);
	sshkey_free(key);
	sshbuf_free(pem);
	sshbuf_free(file);
	TEST_DONE();
}
#endif /* WITH_OPENSSL */

static void
ed301_negative_tests(void)
{
	struct sshkey *key = NULL, *other = NULL, *parsed = NULL;
	struct sshbuf *b, *file, *decoded;
	u_char pk[ED301_PK_SZ + 1], sk[ED301_SK_SZ + 1], raw[76];
	u_char *sig = NULL, *other_sig = NULL;
	const u_char *sigptr;
	const u_char msg[] = "Ed301 SSH negative checks";
	u_char changed[sizeof(msg)];
	size_t siglen, other_len, rawlen;
	char *name, *encoded, *end, *base64;
	int i;

	TEST_START("Ed301 public parsing without provider validation");
	ASSERT_PTR_NE(b = sshbuf_new(), NULL);
	memset(pk, 0, sizeof(pk));
	ASSERT_INT_EQ(sshbuf_put_cstring(b, "ssh-ed301@eirindeval.de"), 0);
	ASSERT_INT_EQ(sshbuf_put_string(b, pk, ED301_PK_SZ), 0);
	ASSERT_INT_EQ(sshkey_fromb(b, &parsed), 0);
	ASSERT_INT_EQ(parsed->type, KEY_ED301);
	ASSERT_INT_EQ(sshkey_names_valid2("ssh-ed301@eirindeval.de",
	    0, 0), 1);
	ASSERT_INT_EQ(sshkey_names_valid2("ssh-ed301*", 1, 0), 1);
	sshkey_free(parsed);
	parsed = NULL;
	sshbuf_free(b);
	TEST_DONE();

	if (!ssh_ed301_available()) {
		fprintf(stderr, "SKIP Ed301 negative crypto tests: "
		    "provider unavailable\n");
		return;
	}
	ASSERT_INT_EQ(sshkey_generate(KEY_ED301, 304, &key), 0);

	TEST_START("Ed301 private public/seed consistency and lengths");
	for (i = 0; i < 7; i++) {
		test_subtest_info("mutation %d", i);
		memcpy(pk, key->ed301_pk, ED301_PK_SZ);
		memcpy(sk, key->ed301_sk, ED301_SK_SZ);
		pk[ED301_PK_SZ] = sk[ED301_SK_SZ] = 0;
		if (i == 0)
			pk[0] ^= 1;
		if (i == 1)
			sk[crypto_sign_ed301_SEEDBYTES] ^= 1;
		if (i == 2)
			sk[0] ^= 1;
		ASSERT_PTR_NE(b = sshbuf_new(), NULL);
		ASSERT_INT_EQ(sshbuf_put_cstring(b,
		    "ssh-ed301@eirindeval.de"), 0);
		ASSERT_INT_EQ(sshbuf_put_string(b, pk,
		    i == 3 ? ED301_PK_SZ - 1 :
		    i == 4 ? ED301_PK_SZ + 1 : ED301_PK_SZ), 0);
		ASSERT_INT_EQ(sshbuf_put_string(b, sk,
		    i == 5 ? ED301_SK_SZ - 1 :
		    i == 6 ? ED301_SK_SZ + 1 : ED301_SK_SZ), 0);
		ASSERT_INT_EQ(sshkey_private_deserialize(b, &parsed),
		    SSH_ERR_INVALID_FORMAT);
		ASSERT_PTR_EQ(parsed, NULL);
		sshbuf_free(b);
	}
	TEST_DONE();

	TEST_START("Ed301 private file rejects trailing decoded data");
	ASSERT_PTR_NE(file = sshbuf_new(), NULL);
	ASSERT_INT_EQ(sshkey_private_to_fileblob(key, file, "", "test",
	    SSHKEY_PRIVATE_OPENSSH, NULL, 0), 0);
	ASSERT_PTR_NE(encoded = sshbuf_dup_string(file), NULL);
	ASSERT_PTR_NE(end = strstr(encoded,
	    "-----END OPENSSH PRIVATE KEY-----"), NULL);
	*end = '\0';
	ASSERT_PTR_NE(decoded = sshbuf_new(), NULL);
	ASSERT_INT_EQ(sshbuf_b64tod(decoded, encoded +
	    strlen("-----BEGIN OPENSSH PRIVATE KEY-----\n")), 0);
	ASSERT_INT_EQ(sshbuf_put_u8(decoded, 0xa5), 0);
	ASSERT_PTR_NE(base64 = sshbuf_dtob64_string(decoded, 0), NULL);
	sshbuf_reset(file);
	ASSERT_INT_EQ(sshbuf_putf(file,
	    "-----BEGIN OPENSSH PRIVATE KEY-----\n%s\n"
	    "-----END OPENSSH PRIVATE KEY-----\n", base64), 0);
	ASSERT_INT_EQ(sshkey_parse_private_fileblob(file, "", &parsed,
	    NULL), SSH_ERR_INVALID_FORMAT);
	ASSERT_PTR_EQ(parsed, NULL);
	freezero(encoded, strlen(encoded));
	freezero(base64, strlen(base64));
	sshbuf_free(file);
	sshbuf_free(decoded);
	TEST_DONE();

	ASSERT_INT_EQ(sshkey_sign(key, &sig, &siglen, msg, sizeof(msg),
	    NULL, NULL, NULL, 0), 0);
	ASSERT_PTR_NE(b = sshbuf_from(sig, siglen), NULL);
	ASSERT_INT_EQ(sshbuf_get_cstring(b, &name, NULL), 0);
	ASSERT_INT_EQ(sshbuf_get_string_direct(b, &sigptr, &rawlen), 0);
	ASSERT_SIZE_T_EQ(rawlen, sizeof(raw));
	memcpy(raw, sigptr, sizeof(raw));
	free(name);
	sshbuf_free(b);

	TEST_START("Ed301 signature names, lengths and trailing data");
	for (i = 0; i < 4; i++) {
		test_subtest_info("mutation %d", i);
		ASSERT_PTR_NE(b = sshbuf_new(), NULL);
		ASSERT_INT_EQ(sshbuf_put_cstring(b, i == 0 ? "ssh-ed25519" :
		    "ssh-ed301@eirindeval.de"), 0);
		memcpy(sk, raw, sizeof(raw));
		sk[sizeof(raw)] = 0;
		ASSERT_INT_EQ(sshbuf_put_string(b, sk,
		    i == 1 ? 75 : i == 2 ? 77 : 76), 0);
		if (i == 3)
			ASSERT_INT_EQ(sshbuf_put_u8(b, 1), 0);
		ASSERT_INT_EQ(sshkey_verify(key, sshbuf_ptr(b), sshbuf_len(b),
		    msg, sizeof(msg), NULL, 0, NULL),
		    i == 0 ? SSH_ERR_KEY_TYPE_MISMATCH :
		    i == 3 ? SSH_ERR_UNEXPECTED_TRAILING_DATA :
		    SSH_ERR_INVALID_FORMAT);
		sshbuf_free(b);
	}
	TEST_DONE();

	TEST_START("Ed301 changed message and cross-family signatures");
	memcpy(changed, msg, sizeof(msg));
	changed[0] ^= 1;
	ASSERT_INT_EQ(sshkey_verify(key, sig, siglen, changed,
	    sizeof(changed), NULL, 0, NULL), SSH_ERR_SIGNATURE_INVALID);
	ASSERT_INT_EQ(sshkey_generate(KEY_ED25519, 256, &other), 0);
	ASSERT_INT_EQ(sshkey_sign(other, &other_sig, &other_len, msg,
	    sizeof(msg), NULL, NULL, NULL, 0), 0);
	ASSERT_INT_EQ(sshkey_verify(key, other_sig, other_len, msg,
	    sizeof(msg), NULL, 0, NULL), SSH_ERR_KEY_TYPE_MISMATCH);
	ASSERT_INT_EQ(sshkey_verify(other, sig, siglen, msg,
	    sizeof(msg), NULL, 0, NULL), SSH_ERR_KEY_TYPE_MISMATCH);
	TEST_DONE();

	free(sig);
	free(other_sig);
	sshkey_free(key);
	sshkey_free(other);
	explicit_bzero(sk, sizeof(sk));
}
#endif /* WITH_ED301 */

#ifdef WITH_MLDSA65_ED301
static void
mldsa65_ed301_sshkey_tests(void)
{
	struct sshkey *key = NULL, *parsed = NULL, *ca = NULL;
	struct sshbuf *b, *file, *rawsig;
	const u_char msg[] = "ML-DSA65/Ed301 SSH key roundtrip";
	u_char pk[MLDSA65_ED301_PK_SZ + 1], sk[MLDSA65_ED301_SK_SZ + 1];
	u_char raw[MLDSA65_ED301_SIG_SZ + 1];
	u_char *sig;
	const u_char *p;
	size_t siglen, n, i;

	if (!ssh_ed301_available()) {
		TEST_START("ML-DSA65/Ed301 unavailable key generation");
		ASSERT_INT_EQ(sshkey_generate(KEY_MLDSA65_ED301, 560, &key),
		    SSH_ERR_FEATURE_UNSUPPORTED);
		ASSERT_PTR_EQ(key, NULL);
		TEST_DONE();
		fprintf(stderr, "SKIP ML-DSA65/Ed301 sshkey: "
		    "provider unavailable\n");
		return;
	}
	TEST_START("ML-DSA65/Ed301 key and encrypted private roundtrip");
	ASSERT_INT_EQ(sshkey_generate(KEY_MLDSA65_ED301, 560, &key), 0);
	ASSERT_INT_EQ(sshkey_size(key), 560);
	ASSERT_PTR_NE(file = sshbuf_new(), NULL);
	ASSERT_INT_EQ(sshkey_private_to_fileblob(key, file, "test-passphrase",
	    "test", SSHKEY_PRIVATE_OPENSSH, NULL, 0), 0);
	ASSERT_INT_EQ(sshkey_parse_private_fileblob(file, "test-passphrase",
	    &parsed, NULL), 0);
	ASSERT_INT_EQ(sshkey_equal_public(key, parsed), 1);
	ASSERT_INT_EQ(sshkey_sign(parsed, &sig, &siglen, msg, sizeof(msg),
	    NULL, NULL, NULL, 0), 0);
	ASSERT_INT_EQ(sshkey_verify(key, sig, siglen, msg, sizeof(msg),
	    NULL, 0, NULL), 0);
	sshkey_free(parsed);
	parsed = NULL;
	sshbuf_free(file);
	TEST_DONE();

	TEST_START("ML-DSA65/Ed301 private lengths and full PK consistency");
	for (i = 0; i < 8; i++) {
		memcpy(pk, key->mldsa65_ed301_pk, MLDSA65_ED301_PK_SZ);
		memcpy(sk, key->mldsa65_ed301_sk, MLDSA65_ED301_SK_SZ);
		pk[MLDSA65_ED301_PK_SZ] = sk[MLDSA65_ED301_SK_SZ] = 0;
		if (i == 0) pk[0] ^= 1;
		if (i == 1) pk[MLDSA65_PUBLICKEYBYTES] ^= 1;
		if (i == 2) sk[0] ^= 1;
		if (i == 3) sk[MLDSA65_SEEDBYTES] ^= 1;
		ASSERT_PTR_NE(b = sshbuf_new(), NULL);
		ASSERT_INT_EQ(sshbuf_put_cstring(b,
		    "ssh-mldsa65-ed301@eirindeval.de"), 0);
		ASSERT_INT_EQ(sshbuf_put_string(b, pk,
		    i == 4 ? MLDSA65_ED301_PK_SZ - 1 :
		    i == 5 ? MLDSA65_ED301_PK_SZ + 1 : MLDSA65_ED301_PK_SZ), 0);
		ASSERT_INT_EQ(sshbuf_put_string(b, sk,
		    i == 6 ? MLDSA65_ED301_SK_SZ - 1 :
		    i == 7 ? MLDSA65_ED301_SK_SZ + 1 : MLDSA65_ED301_SK_SZ), 0);
		ASSERT_INT_EQ(sshkey_private_deserialize(b, &parsed),
		    SSH_ERR_INVALID_FORMAT);
		ASSERT_PTR_EQ(parsed, NULL);
		sshbuf_free(b);
	}
	TEST_DONE();

	TEST_START("ML-DSA65/Ed301 signature names, lengths and trailing data");
	ASSERT_PTR_NE(rawsig = sshbuf_from(sig, siglen), NULL);
	ASSERT_INT_EQ(sshbuf_skip_string(rawsig), 0);
	ASSERT_INT_EQ(sshbuf_get_string_direct(rawsig, &p, &n), 0);
	ASSERT_SIZE_T_EQ(n, MLDSA65_ED301_SIG_SZ);
	memcpy(raw, p, n);
	raw[n] = 0;
	for (i = 0; i < 4; i++) {
		ASSERT_PTR_NE(b = sshbuf_new(), NULL);
		ASSERT_INT_EQ(sshbuf_put_cstring(b,
		    i == 0 ? "ssh-ed301@eirindeval.de" :
		    "ssh-mldsa65-ed301@eirindeval.de"), 0);
		ASSERT_INT_EQ(sshbuf_put_string(b, raw,
		    i == 1 ? n - 1 : i == 2 ? n + 1 : n), 0);
		if (i == 3) ASSERT_INT_EQ(sshbuf_put_u8(b, 1), 0);
		ASSERT_INT_EQ(sshkey_verify(key, sshbuf_ptr(b), sshbuf_len(b),
		    msg, sizeof(msg), NULL, 0, NULL),
		    i == 0 ? SSH_ERR_KEY_TYPE_MISMATCH :
		    i == 3 ? SSH_ERR_UNEXPECTED_TRAILING_DATA :
		    SSH_ERR_INVALID_FORMAT);
		sshbuf_free(b);
	}
	sshbuf_free(rawsig);
	free(sig);
	TEST_DONE();

	TEST_START("ML-DSA65/Ed301 certified private seed matches subject");
	ASSERT_INT_EQ(sshkey_generate(KEY_ED301, 304, &ca), 0);
	ASSERT_INT_EQ(sshkey_to_certified(key), 0);
	key->cert->type = SSH2_CERT_TYPE_USER;
	key->cert->valid_after = 1;
	key->cert->valid_before = ~(uint64_t)0;
	ASSERT_INT_EQ(sshkey_certify(key, ca, NULL, NULL, NULL), 0);
	ASSERT_PTR_NE(b = sshbuf_new(), NULL);
	ASSERT_INT_EQ(sshkey_private_serialize(key, b), 0);
	ASSERT_INT_EQ(sshkey_private_deserialize(b, &parsed), 0);
	ASSERT_INT_EQ(sshkey_equal_public(key, parsed), 1);
	sshkey_free(parsed);
	parsed = NULL;
	sshbuf_reset(b);
	key->mldsa65_ed301_sk[0] ^= 1;
	ASSERT_INT_EQ(sshkey_private_serialize(key, b), 0);
	ASSERT_INT_EQ(sshkey_private_deserialize(b, &parsed),
	    SSH_ERR_INVALID_FORMAT);
	ASSERT_PTR_EQ(parsed, NULL);
	sshbuf_free(b);
	sshkey_free(ca);
	sshkey_free(key);
	explicit_bzero(sk, sizeof(sk));
	TEST_DONE();
}
#endif /* WITH_MLDSA65_ED301 */

void
sshkey_tests(void)
{
	struct sshkey *k1 = NULL, *k2 = NULL, *k3 = NULL, *k4 = NULL;
	struct sshkey *kr = NULL, *kd = NULL, *ke = NULL, *kf = NULL;
	struct sshbuf *b = NULL;

	TEST_START("new invalid");
	k1 = sshkey_new(-42);
	ASSERT_PTR_EQ(k1, NULL);
	TEST_DONE();

	TEST_START("new/free KEY_UNSPEC");
	k1 = sshkey_new(KEY_UNSPEC);
	ASSERT_PTR_NE(k1, NULL);
	sshkey_free(k1);
	k1 = NULL;
	TEST_DONE();

#ifdef WITH_OPENSSL
	TEST_START("new/free KEY_RSA");
	k1 = sshkey_new(KEY_RSA);
	ASSERT_PTR_NE(k1, NULL);
	ASSERT_PTR_NE(k1->pkey, NULL);
	sshkey_free(k1);
	k1 = NULL;
	TEST_DONE();

	TEST_START("new/free KEY_ECDSA");
	k1 = sshkey_new(KEY_ECDSA);
	ASSERT_PTR_NE(k1, NULL);
	ASSERT_PTR_EQ(k1->pkey, NULL);  /* Can't allocate without NID */
	sshkey_free(k1);
	k1 = NULL;
	TEST_DONE();
#endif /* WITH_OPENSSL */

	TEST_START("new/free KEY_ED25519");
	k1 = sshkey_new(KEY_ED25519);
	ASSERT_PTR_NE(k1, NULL);
	/* These should be blank until key loaded or generated */
	ASSERT_PTR_EQ(k1->ed25519_sk, NULL);
	ASSERT_PTR_EQ(k1->ed25519_pk, NULL);
	sshkey_free(k1);
	k1 = NULL;
	TEST_DONE();

	TEST_START("new/free KEY_MLDSA44_ED25519");
	k1 = sshkey_new(KEY_MLDSA44_ED25519);
	ASSERT_PTR_NE(k1, NULL);
	ASSERT_PTR_EQ(k1->mldsa_ed25519_sk, NULL);
	ASSERT_PTR_EQ(k1->mldsa_ed25519_pk, NULL);
	sshkey_free(k1);
	k1 = NULL;
	TEST_DONE();

#ifdef WITH_OPENSSL
	TEST_START("generate KEY_RSA too small modulus");
	ASSERT_INT_EQ(sshkey_generate(KEY_RSA, 128, &k1),
	    SSH_ERR_KEY_LENGTH);
	ASSERT_PTR_EQ(k1, NULL);
	TEST_DONE();

	TEST_START("generate KEY_RSA too large modulus");
	ASSERT_INT_EQ(sshkey_generate(KEY_RSA, 1 << 20, &k1),
	    SSH_ERR_KEY_LENGTH);
	ASSERT_PTR_EQ(k1, NULL);
	TEST_DONE();


	TEST_START("generate KEY_ECDSA wrong bits");
	ASSERT_INT_EQ(sshkey_generate(KEY_ECDSA, 42, &k1),
	    SSH_ERR_KEY_LENGTH);
	ASSERT_PTR_EQ(k1, NULL);
	sshkey_free(k1);
	k1 = NULL;
	TEST_DONE();

	TEST_START("generate KEY_RSA");
	ASSERT_INT_EQ(sshkey_generate(KEY_RSA, 767, &kr),
	    SSH_ERR_KEY_LENGTH);
	ASSERT_INT_EQ(sshkey_generate(KEY_RSA, 1024, &kr), 0);
	ASSERT_PTR_NE(kr, NULL);
	ASSERT_PTR_NE(EVP_PKEY_get0_RSA(kr->pkey), NULL);
	ASSERT_PTR_NE(rsa_n(kr), NULL);
	ASSERT_PTR_NE(rsa_e(kr), NULL);
	ASSERT_PTR_NE(rsa_p(kr), NULL);
	ASSERT_INT_EQ(BN_num_bits(rsa_n(kr)), 1024);
	TEST_DONE();


	TEST_START("generate KEY_ECDSA");
	ASSERT_INT_EQ(sshkey_generate(KEY_ECDSA, 256, &ke), 0);
	ASSERT_PTR_NE(ke, NULL);
	ASSERT_PTR_NE(EVP_PKEY_get0_EC_KEY(ke->pkey), NULL);
	ASSERT_PTR_NE(EC_KEY_get0_public_key(EVP_PKEY_get0_EC_KEY(ke->pkey)),
	    NULL);
	ASSERT_PTR_NE(EC_KEY_get0_private_key(EVP_PKEY_get0_EC_KEY(ke->pkey)),
	    NULL);
	TEST_DONE();
#endif /* WITH_OPENSSL */

	TEST_START("generate KEY_ED25519");
	ASSERT_INT_EQ(sshkey_generate(KEY_ED25519, 256, &kf), 0);
	ASSERT_PTR_NE(kf, NULL);
	ASSERT_INT_EQ(kf->type, KEY_ED25519);
	ASSERT_PTR_NE(kf->ed25519_pk, NULL);
	ASSERT_PTR_NE(kf->ed25519_sk, NULL);
	TEST_DONE();

	TEST_START("generate KEY_MLDSA44_ED25519");
	ASSERT_INT_EQ(sshkey_generate(KEY_MLDSA44_ED25519, 256, &k1), 0);
	ASSERT_PTR_NE(k1, NULL);
	ASSERT_INT_EQ(k1->type, KEY_MLDSA44_ED25519);
	ASSERT_PTR_NE(k1->mldsa_ed25519_pk, NULL);
	ASSERT_PTR_NE(k1->mldsa_ed25519_sk, NULL);
	sshkey_free(k1);
	k1 = NULL;
	TEST_DONE();

#ifdef WITH_OPENSSL
	TEST_START("demote KEY_RSA");
	ASSERT_INT_EQ(sshkey_from_private(kr, &k1), 0);
	ASSERT_PTR_NE(k1, NULL);
	ASSERT_PTR_NE(kr, k1);
	ASSERT_INT_EQ(k1->type, KEY_RSA);
	ASSERT_PTR_NE(EVP_PKEY_get0_RSA(k1->pkey), NULL);
	ASSERT_PTR_NE(rsa_n(k1), NULL);
	ASSERT_PTR_NE(rsa_e(k1), NULL);
	ASSERT_PTR_EQ(rsa_p(k1), NULL);
	TEST_DONE();

	TEST_START("equal KEY_RSA/demoted KEY_RSA");
	ASSERT_INT_EQ(sshkey_equal(kr, k1), 1);
	sshkey_free(k1);
	k1 = NULL;
	TEST_DONE();


	TEST_START("demote KEY_ECDSA");
	ASSERT_INT_EQ(sshkey_from_private(ke, &k1), 0);
	ASSERT_PTR_NE(k1, NULL);
	ASSERT_PTR_NE(ke, k1);
	ASSERT_INT_EQ(k1->type, KEY_ECDSA);
	ASSERT_PTR_NE(EVP_PKEY_get0_EC_KEY(k1->pkey), NULL);
	ASSERT_INT_EQ(k1->ecdsa_nid, ke->ecdsa_nid);
	ASSERT_PTR_NE(EC_KEY_get0_public_key(EVP_PKEY_get0_EC_KEY(ke->pkey)),
	   NULL);
	ASSERT_PTR_EQ(EC_KEY_get0_private_key(EVP_PKEY_get0_EC_KEY(k1->pkey)),
	   NULL);
	TEST_DONE();

	TEST_START("equal KEY_ECDSA/demoted KEY_ECDSA");
	ASSERT_INT_EQ(sshkey_equal(ke, k1), 1);
	sshkey_free(k1);
	k1 = NULL;
	TEST_DONE();
#endif /* WITH_OPENSSL */

	TEST_START("demote KEY_ED25519");
	ASSERT_INT_EQ(sshkey_from_private(kf, &k1), 0);
	ASSERT_PTR_NE(k1, NULL);
	ASSERT_PTR_NE(kf, k1);
	ASSERT_INT_EQ(k1->type, KEY_ED25519);
	ASSERT_PTR_NE(k1->ed25519_pk, NULL);
	ASSERT_PTR_EQ(k1->ed25519_sk, NULL);
	TEST_DONE();

	TEST_START("equal KEY_ED25519/demoted KEY_ED25519");
	ASSERT_INT_EQ(sshkey_equal(kf, k1), 1);
	sshkey_free(k1);
	k1 = NULL;
	TEST_DONE();

	TEST_START("demote KEY_MLDSA44_ED25519");
	ASSERT_INT_EQ(sshkey_generate(KEY_MLDSA44_ED25519, 256, &k2), 0);
	ASSERT_INT_EQ(sshkey_from_private(k2, &k1), 0);
	ASSERT_PTR_NE(k1, NULL);
	ASSERT_PTR_NE(k2, k1);
	ASSERT_INT_EQ(k1->type, KEY_MLDSA44_ED25519);
	ASSERT_PTR_NE(k1->mldsa_ed25519_pk, NULL);
	ASSERT_PTR_EQ(k1->mldsa_ed25519_sk, NULL);
	TEST_DONE();

	TEST_START("equal KEY_MLDSA44_ED25519/demoted KEY_MLDSA44_ED25519");
	ASSERT_INT_EQ(sshkey_equal(k2, k1), 1);
	sshkey_free(k1);
	sshkey_free(k2);
	k1 = k2 = NULL;
	TEST_DONE();

#ifdef WITH_OPENSSL
	TEST_START("equal mismatched key types");
	ASSERT_INT_EQ(sshkey_equal(kd, kr), 0);
	ASSERT_INT_EQ(sshkey_equal(kd, ke), 0);
	ASSERT_INT_EQ(sshkey_equal(kr, ke), 0);
	ASSERT_INT_EQ(sshkey_equal(ke, kf), 0);
	ASSERT_INT_EQ(sshkey_equal(kd, kf), 0);
	TEST_DONE();

	TEST_START("equal different keys");
	ASSERT_INT_EQ(sshkey_generate(KEY_RSA, 1024, &k1), 0);
	ASSERT_INT_EQ(sshkey_equal(kr, k1), 0);
	sshkey_free(k1);
	k1 = NULL;
	ASSERT_INT_EQ(sshkey_generate(KEY_ECDSA, 256, &k1), 0);
	ASSERT_INT_EQ(sshkey_equal(ke, k1), 0);
	sshkey_free(k1);
	k1 = NULL;
	ASSERT_INT_EQ(sshkey_generate(KEY_ED25519, 256, &k1), 0);
	ASSERT_INT_EQ(sshkey_equal(kf, k1), 0);
	sshkey_free(k1);
	k1 = NULL;
	ASSERT_INT_EQ(sshkey_generate(KEY_MLDSA44_ED25519, 256, &k1), 0);
	ASSERT_INT_EQ(sshkey_generate(KEY_MLDSA44_ED25519, 256, &k2), 0);
	ASSERT_INT_EQ(sshkey_equal(k2, k1), 0);
	sshkey_free(k1);
	sshkey_free(k2);
	k1 = k2 = NULL;
	TEST_DONE();
#endif /* WITH_OPENSSL */

	sshkey_free(kr);
	sshkey_free(kd);
	sshkey_free(ke);
	sshkey_free(kf);
	kr = kd = ke = kf = NULL;

	TEST_START("certify key");
	ASSERT_INT_EQ(sshkey_load_public(test_data_file("ed25519_1.pub"),
	    &k1, NULL), 0);
	k2 = get_private("ed25519_2");
	ASSERT_INT_EQ(sshkey_to_certified(k1), 0);
	ASSERT_PTR_NE(k1->cert, NULL);
	k1->cert->type = SSH2_CERT_TYPE_USER;
	k1->cert->serial = 1234;
	k1->cert->key_id = strdup("estragon");
	ASSERT_PTR_NE(k1->cert->key_id, NULL);
	k1->cert->principals = calloc(4, sizeof(*k1->cert->principals));
	ASSERT_PTR_NE(k1->cert->principals, NULL);
	k1->cert->principals[0] = strdup("estragon");
	k1->cert->principals[1] = strdup("vladimir");
	k1->cert->principals[2] = strdup("pozzo");
	k1->cert->principals[3] = strdup("lucky");
	ASSERT_PTR_NE(k1->cert->principals[0], NULL);
	ASSERT_PTR_NE(k1->cert->principals[1], NULL);
	ASSERT_PTR_NE(k1->cert->principals[2], NULL);
	ASSERT_PTR_NE(k1->cert->principals[3], NULL);
	k1->cert->nprincipals = 4;
	k1->cert->valid_after = 0;
	k1->cert->valid_before = (uint64_t)-1;
	sshbuf_free(k1->cert->critical);
	k1->cert->critical = sshbuf_new();
	ASSERT_PTR_NE(k1->cert->critical, NULL);
	sshbuf_free(k1->cert->extensions);
	k1->cert->extensions = sshbuf_new();
	ASSERT_PTR_NE(k1->cert->extensions, NULL);
	put_opt(k1->cert->critical, "force-command", "/usr/bin/true");
	put_opt(k1->cert->critical, "source-address", "127.0.0.1");
	put_opt(k1->cert->extensions, "permit-X11-forwarding", NULL);
	put_opt(k1->cert->extensions, "permit-agent-forwarding", NULL);
	ASSERT_INT_EQ(sshkey_from_private(k2, &k1->cert->signature_key), 0);
	ASSERT_INT_EQ(sshkey_certify(k1, k2, NULL, NULL, NULL), 0);
	b = sshbuf_new();
	ASSERT_PTR_NE(b, NULL);
	ASSERT_INT_EQ(sshkey_putb(k1, b), 0);
	ASSERT_INT_EQ(sshkey_from_blob(sshbuf_ptr(b), sshbuf_len(b), &k3), 0);

	sshkey_free(k1);
	sshkey_free(k2);
	sshkey_free(k3);
	k1 = k2 = k3 = NULL;
	sshbuf_reset(b);
	TEST_DONE();

	TEST_START("certify key MLDSA44-ED25519");
	ASSERT_INT_EQ(sshkey_load_public(
	    test_data_file("mldsa44_ed25519_1.pub"), &k1, NULL), 0);
	k2 = get_private("mldsa44_ed25519_2");
	ASSERT_INT_EQ(sshkey_to_certified(k1), 0);
	ASSERT_PTR_NE(k1->cert, NULL);
	k1->cert->type = SSH2_CERT_TYPE_USER;
	k1->cert->serial = 1234;
	k1->cert->key_id = strdup("estragon");
	ASSERT_PTR_NE(k1->cert->key_id, NULL);
	k1->cert->principals = calloc(4, sizeof(*k1->cert->principals));
	ASSERT_PTR_NE(k1->cert->principals, NULL);
	k1->cert->principals[0] = strdup("estragon");
	k1->cert->principals[1] = strdup("vladimir");
	k1->cert->principals[2] = strdup("pozzo");
	k1->cert->principals[3] = strdup("lucky");
	ASSERT_PTR_NE(k1->cert->principals[0], NULL);
	ASSERT_PTR_NE(k1->cert->principals[1], NULL);
	ASSERT_PTR_NE(k1->cert->principals[2], NULL);
	ASSERT_PTR_NE(k1->cert->principals[3], NULL);
	k1->cert->nprincipals = 4;
	k1->cert->valid_after = 0;
	k1->cert->valid_before = (uint64_t)-1;
	sshbuf_free(k1->cert->critical);
	k1->cert->critical = sshbuf_new();
	ASSERT_PTR_NE(k1->cert->critical, NULL);
	sshbuf_free(k1->cert->extensions);
	k1->cert->extensions = sshbuf_new();
	ASSERT_PTR_NE(k1->cert->extensions, NULL);
	put_opt(k1->cert->critical, "force-command", "/usr/bin/true");
	put_opt(k1->cert->critical, "source-address", "127.0.0.1");
	put_opt(k1->cert->extensions, "permit-X11-forwarding", NULL);
	put_opt(k1->cert->extensions, "permit-agent-forwarding", NULL);
	ASSERT_INT_EQ(sshkey_from_private(k2, &k1->cert->signature_key), 0);
	ASSERT_INT_EQ(sshkey_certify(k1, k2, NULL, NULL, NULL), 0);
	b = sshbuf_new();
	ASSERT_PTR_NE(b, NULL);
	ASSERT_INT_EQ(sshkey_putb(k1, b), 0);
	ASSERT_INT_EQ(sshkey_from_blob(sshbuf_ptr(b), sshbuf_len(b), &k3), 0);
	sshkey_free(k1);
	sshkey_free(k2);
	sshkey_free(k3);
	k1 = k2 = k3 = NULL;
	sshbuf_reset(b);
	TEST_DONE();

#ifdef WITH_OPENSSL
	TEST_START("sign and verify RSA");
	k1 = get_private("rsa_1");
	ASSERT_INT_EQ(sshkey_load_public(test_data_file("rsa_2.pub"), &k2,
	    NULL), 0);
	signature_tests(k1, k2, "ssh-rsa");
	sshkey_free(k1);
	sshkey_free(k2);
	k1 = k2 = NULL;
	TEST_DONE();

	TEST_START("sign and verify RSA-SHA256");
	k1 = get_private("rsa_1");
	ASSERT_INT_EQ(sshkey_load_public(test_data_file("rsa_2.pub"), &k2,
	    NULL), 0);
	signature_tests(k1, k2, "rsa-sha2-256");
	sshkey_free(k1);
	sshkey_free(k2);
	k1 = k2 = NULL;
	TEST_DONE();

	TEST_START("sign and verify RSA-SHA512");
	k1 = get_private("rsa_1");
	ASSERT_INT_EQ(sshkey_load_public(test_data_file("rsa_2.pub"), &k2,
	    NULL), 0);
	signature_tests(k1, k2, "rsa-sha2-512");
	sshkey_free(k1);
	sshkey_free(k2);
	k1 = k2 = NULL;
	TEST_DONE();


	TEST_START("sign and verify ECDSA");
	k1 = get_private("ecdsa_1");
	ASSERT_INT_EQ(sshkey_load_public(test_data_file("ecdsa_2.pub"), &k2,
	    NULL), 0);
	signature_tests(k1, k2, NULL);
	sshkey_free(k1);
	sshkey_free(k2);
	k1 = k2 = NULL;
	TEST_DONE();
#endif /* WITH_OPENSSL */

	TEST_START("sign and verify ED25519");
	k1 = get_private("ed25519_1");
	ASSERT_INT_EQ(sshkey_load_public(test_data_file("ed25519_2.pub"), &k2,
	    NULL), 0);
	signature_tests(k1, k2, NULL);
	sshkey_free(k1);
	sshkey_free(k2);
	k1 = k2 = NULL;
	TEST_DONE();

	TEST_START("sign and verify MLDSA44-ED25519");
	k1 = get_private("mldsa44_ed25519_1");
	ASSERT_INT_EQ(sshkey_load_public(
	    test_data_file("mldsa44_ed25519_2.pub"), &k2, NULL), 0);
	signature_tests(k1, k2, NULL);
	sshkey_free(k1);
	sshkey_free(k2);
	k1 = k2 = NULL;
	TEST_DONE();

#ifdef WITH_ED301
	if (ssh_ed301_available()) {
		TEST_START("new/free KEY_ED301");
		k1 = sshkey_new(KEY_ED301);
		ASSERT_PTR_NE(k1, NULL);
		ASSERT_PTR_EQ(k1->ed301_sk, NULL);
		ASSERT_PTR_EQ(k1->ed301_pk, NULL);
		sshkey_free(k1);
		k1 = NULL;
		TEST_DONE();

		TEST_START("generate KEY_ED301");
		ASSERT_INT_EQ(sshkey_generate(KEY_ED301, 304, &k1), 0);
		ASSERT_INT_EQ(k1->type, KEY_ED301);
		ASSERT_INT_EQ(sshkey_size(k1), 304);
		ASSERT_PTR_NE(k1->ed301_pk, NULL);
		ASSERT_PTR_NE(k1->ed301_sk, NULL);
		TEST_DONE();

		TEST_START("demote/equal KEY_ED301");
		ASSERT_INT_EQ(sshkey_from_private(k1, &k2), 0);
		ASSERT_PTR_NE(k1, k2);
		ASSERT_PTR_EQ(k2->ed301_sk, NULL);
		ASSERT_INT_EQ(sshkey_equal(k1, k2), 1);
		sshkey_free(k1);
		sshkey_free(k2);
		k1 = k2 = NULL;
		TEST_DONE();

		TEST_START("equal different ED301 keys");
		ASSERT_INT_EQ(sshkey_generate(KEY_ED301, 304, &k1), 0);
		ASSERT_INT_EQ(sshkey_generate(KEY_ED301, 304, &k2), 0);
		ASSERT_INT_EQ(sshkey_equal(k1, k2), 0);
		sshkey_free(k1);
		sshkey_free(k2);
		k1 = k2 = NULL;
		TEST_DONE();
		TEST_START("certify key ED301");
		ASSERT_INT_EQ(sshkey_load_public(test_data_file("ed301_1.pub"),
		    &k1, NULL), 0);
		k2 = get_private("ed301_2");
		ASSERT_INT_EQ(sshkey_to_certified(k1), 0);
		ASSERT_PTR_NE(k1->cert, NULL);
		k1->cert->type = SSH2_CERT_TYPE_USER;
		k1->cert->serial = 1234;
		k1->cert->key_id = strdup("estragon");
		ASSERT_PTR_NE(k1->cert->key_id, NULL);
		k1->cert->principals = calloc(4, sizeof(*k1->cert->principals));
		ASSERT_PTR_NE(k1->cert->principals, NULL);
		k1->cert->principals[0] = strdup("estragon");
		k1->cert->principals[1] = strdup("vladimir");
		k1->cert->principals[2] = strdup("pozzo");
		k1->cert->principals[3] = strdup("lucky");
		ASSERT_PTR_NE(k1->cert->principals[0], NULL);
		ASSERT_PTR_NE(k1->cert->principals[1], NULL);
		ASSERT_PTR_NE(k1->cert->principals[2], NULL);
		ASSERT_PTR_NE(k1->cert->principals[3], NULL);
		k1->cert->nprincipals = 4;
		k1->cert->valid_after = 0;
		k1->cert->valid_before = (uint64_t)-1;
		sshbuf_free(k1->cert->critical);
		k1->cert->critical = sshbuf_new();
		ASSERT_PTR_NE(k1->cert->critical, NULL);
		sshbuf_free(k1->cert->extensions);
		k1->cert->extensions = sshbuf_new();
		ASSERT_PTR_NE(k1->cert->extensions, NULL);
		put_opt(k1->cert->critical, "force-command", "/usr/bin/true");
		put_opt(k1->cert->critical, "source-address", "127.0.0.1");
		put_opt(k1->cert->extensions, "permit-X11-forwarding", NULL);
		put_opt(k1->cert->extensions, "permit-agent-forwarding", NULL);
		ASSERT_INT_EQ(sshkey_from_private(k2,
		    &k1->cert->signature_key), 0);
		ASSERT_INT_EQ(sshkey_certify(k1, k2, NULL, NULL, NULL), 0);
		b = sshbuf_new();
		ASSERT_PTR_NE(b, NULL);
		ASSERT_INT_EQ(sshkey_putb(k1, b), 0);
		ASSERT_INT_EQ(sshkey_from_blob(sshbuf_ptr(b), sshbuf_len(b),
		    &k3), 0);

		sshkey_free(k1);
		sshkey_free(k2);
		sshkey_free(k3);
		k1 = k2 = k3 = NULL;
		sshbuf_reset(b);
		TEST_DONE();

		TEST_START("sign and verify ED301");
		k1 = get_private("ed301_1");
		ASSERT_INT_EQ(sshkey_load_public(
		    test_data_file("ed301_2.pub"), &k2,
		    NULL), 0);
		signature_tests(k1, k2, NULL);
		sshkey_free(k1);
		sshkey_free(k2);
		k1 = k2 = NULL;
		TEST_DONE();

	} else
		fprintf(stderr, "SKIP Ed301 sshkey tests: "
		    "provider unavailable\n");
#endif


#ifdef WITH_ED301
	ed301_negative_tests();
#ifdef WITH_MLDSA65_ED301
	mldsa65_ed301_sshkey_tests();
#endif
#ifdef WITH_OPENSSL
	ed301_pkcs8_tests();
#endif
#endif

#ifdef WITH_OPENSSL
	TEST_START("nested certificate");
	ASSERT_INT_EQ(sshkey_load_cert(test_data_file("rsa_1"), &k1), 0);
	ASSERT_INT_EQ(sshkey_load_public(test_data_file("rsa_1.pub"), &k2,
	    NULL), 0);
	k3 = get_private("rsa_1");
	build_cert(b, k2, "ssh-rsa-cert-v01@openssh.com", k3, k1, NULL);
	ASSERT_INT_EQ(sshkey_from_blob(sshbuf_ptr(b), sshbuf_len(b), &k4),
	    SSH_ERR_KEY_CERT_INVALID_SIGN_KEY);
	ASSERT_PTR_EQ(k4, NULL);
	sshkey_free(k1);
	sshkey_free(k2);
	sshkey_free(k3);
	k1 = k2 = k3 = NULL;
	sshbuf_free(b);
	TEST_DONE();
#endif /* WITH_OPENSSL */
}

void
sshkey_benchmarks(void)
{
	struct sshkey *k = NULL;

#ifdef WITH_OPENSSL
	BENCH_START("generate RSA-1024");
	TEST_START("generate KEY_RSA");
	ASSERT_INT_EQ(sshkey_generate(KEY_RSA, 1024, &k), 0);
	ASSERT_PTR_NE(k, NULL);
	sshkey_free(k);
	k = NULL;
	TEST_DONE();
	BENCH_FINISH("keys");

	BENCH_START("generate RSA-2048");
	TEST_START("generate KEY_RSA");
	ASSERT_INT_EQ(sshkey_generate(KEY_RSA, 2048, &k), 0);
	ASSERT_PTR_NE(k, NULL);
	sshkey_free(k);
	k = NULL;
	TEST_DONE();
	BENCH_FINISH("keys");

	BENCH_START("generate ECDSA-256");
	TEST_START("generate KEY_ECDSA");
	ASSERT_INT_EQ(sshkey_generate(KEY_ECDSA, 256, &k), 0);
	ASSERT_PTR_NE(k, NULL);
	sshkey_free(k);
	k = NULL;
	TEST_DONE();
	BENCH_FINISH("keys");

	BENCH_START("generate ECDSA-384");
	TEST_START("generate KEY_ECDSA");
	ASSERT_INT_EQ(sshkey_generate(KEY_ECDSA, 384, &k), 0);
	ASSERT_PTR_NE(k, NULL);
	sshkey_free(k);
	k = NULL;
	TEST_DONE();
	BENCH_FINISH("keys");

	BENCH_START("generate ECDSA-521");
	TEST_START("generate KEY_ECDSA");
	ASSERT_INT_EQ(sshkey_generate(KEY_ECDSA, 521, &k), 0);
	ASSERT_PTR_NE(k, NULL);
	sshkey_free(k);
	k = NULL;
	TEST_DONE();
	BENCH_FINISH("keys");
#endif /* WITH_OPENSSL */

	BENCH_START("generate ED25519");
	TEST_START("generate KEY_ED25519");
	ASSERT_INT_EQ(sshkey_generate(KEY_ED25519, 256, &k), 0);
	ASSERT_PTR_NE(k, NULL);
	sshkey_free(k);
	k = NULL;
	TEST_DONE();
	BENCH_FINISH("keys");

	BENCH_START("generate MLDSA44-ED25519");
	TEST_START("generate KEY_MLDSA44_ED25519");
	ASSERT_INT_EQ(sshkey_generate(KEY_MLDSA44_ED25519, 256, &k), 0);
	ASSERT_PTR_NE(k, NULL);
	sshkey_free(k);
	k = NULL;
	TEST_DONE();
	BENCH_FINISH("keys");

#ifdef WITH_OPENSSL
	/* sign */
	signature_benchmark("RSA-1024/SHA1", KEY_RSA, 1024, "ssh-rsa", 0);
	signature_benchmark("RSA-1024/SHA256", KEY_RSA, 1024, "rsa-sha2-256", 0);
	signature_benchmark("RSA-1024/SHA512", KEY_RSA, 1024, "rsa-sha2-512", 0);
	signature_benchmark("RSA-2048/SHA1", KEY_RSA, 2048, "ssh-rsa", 0);
	signature_benchmark("RSA-2048/SHA256", KEY_RSA, 2048, "rsa-sha2-256", 0);
	signature_benchmark("RSA-2048/SHA512", KEY_RSA, 2048, "rsa-sha2-512", 0);
	signature_benchmark("ECDSA-256", KEY_ECDSA, 256, NULL, 0);
	signature_benchmark("ECDSA-384", KEY_ECDSA, 384, NULL, 0);
	signature_benchmark("ECDSA-521", KEY_ECDSA, 521, NULL, 0);
	signature_benchmark("ED25519", KEY_ED25519, 0, NULL, 0);
	signature_benchmark("MLDSA44-ED25519", KEY_MLDSA44_ED25519, 0, NULL, 0);

	/* verify */
	signature_benchmark("RSA-1024/SHA1", KEY_RSA, 1024, "ssh-rsa", 1);
	signature_benchmark("RSA-1024/SHA256", KEY_RSA, 1024, "rsa-sha2-256", 1);
	signature_benchmark("RSA-1024/SHA512", KEY_RSA, 1024, "rsa-sha2-512", 1);
	signature_benchmark("RSA-2048/SHA1", KEY_RSA, 2048, "ssh-rsa", 1);
	signature_benchmark("RSA-2048/SHA256", KEY_RSA, 2048, "rsa-sha2-256", 1);
	signature_benchmark("RSA-2048/SHA512", KEY_RSA, 2048, "rsa-sha2-512", 1);
	signature_benchmark("ECDSA-256", KEY_ECDSA, 256, NULL, 1);
	signature_benchmark("ECDSA-384", KEY_ECDSA, 384, NULL, 1);
	signature_benchmark("ECDSA-521", KEY_ECDSA, 521, NULL, 1);
#endif /* WITH_OPENSSL */
	signature_benchmark("ED25519", KEY_ED25519, 0, NULL, 1);
	signature_benchmark("MLDSA44-ED25519", KEY_MLDSA44_ED25519, 0, NULL, 1);
}
