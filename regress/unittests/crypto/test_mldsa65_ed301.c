/* ML-DSA-65 and ML-DSA-65/Ed301 profile checks. Public domain. */
#include "includes.h"

#ifdef USE_MLDSA
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../test_helper/test_helper.h"
#include "crypto_api.h"
#include "sshbuf.h"

struct sshbuf *load_text_file(const char *);
struct sshbuf *get_json_object(struct sshbuf *);
u_char *get_json_hex(struct sshbuf *, const char *, size_t *);
void mldsa65_tests(void);

static void
check_hash(const u_char *value, size_t len, struct sshbuf *v, const char *field)
{
	u_char hash[32], *expected;
	size_t n;

	expected = get_json_hex(v, field, &n);
	ASSERT_SIZE_T_EQ(n, sizeof(hash));
	sha3_256(hash, value, len);
	ASSERT_MEM_EQ(hash, expected, sizeof(hash));
	free(expected);
}

void
mldsa65_tests(void)
{
	struct sshbuf *json, *array, *v;
	u_char pk[MLDSA65_PUBLICKEYBYTES], sk[MLDSA65_SECRETKEYBYTES];
	u_char sig[MLDSA65_SIGBYTES], *seed, *rnd, *msg;
	size_t count = 0, n, msglen;

	TEST_START("ML-DSA 65 all NIST KATs");
	json = load_text_file("nistkats_mldsa65.json");
	ASSERT_CHAR_EQ(sshbuf_ptr(json)[0], '[');
	ASSERT_CHAR_EQ(sshbuf_ptr(json)[sshbuf_len(json) - 2], ']');
	ASSERT_PTR_NE(array = sshbuf_from(sshbuf_ptr(json) + 1,
	    sshbuf_len(json) - 3), NULL);
	while ((v = get_json_object(array)) != NULL) {
		test_subtest_info("vector %zu", count);
		seed = get_json_hex(v, "key_generation_seed", &n);
		ASSERT_SIZE_T_EQ(n, MLDSA65_SEEDBYTES);
		ASSERT_INT_EQ(crypto_sign_mldsa65_keypair_seeded(pk, sk,
		    seed), 0);
		check_hash(pk, sizeof(pk), v,
		    "sha3_256_hash_of_verification_key");
		check_hash(sk, sizeof(sk), v, "sha3_256_hash_of_signing_key");
		rnd = get_json_hex(v, "signing_randomness", &n);
		ASSERT_SIZE_T_EQ(n, MLDSA65_SEEDBYTES);
		msg = get_json_hex(v, "message", &msglen);
		ASSERT_INT_EQ(crypto_sign_mldsa65_seeded(sig, msg, msglen,
		    NULL, 0, sk, rnd), 0);
		check_hash(sig, sizeof(sig), v, "sha3_256_hash_of_signature");
		ASSERT_INT_EQ(crypto_sign_mldsa65_verify(sig, msg, msglen,
		    NULL, 0, pk), 0);
		freezero(seed, MLDSA65_SEEDBYTES);
		freezero(rnd, MLDSA65_SEEDBYTES);
		free(msg);
		sshbuf_free(v);
		count++;
	}
	ASSERT_SIZE_T_EQ(count, 100);
	sshbuf_free(array);
	sshbuf_free(json);
	explicit_bzero(sk, sizeof(sk));
	TEST_DONE();
	fprintf(stderr, "ML-DSA 65: %zu NIST KATs PASS\n", count);
}
#endif /* USE_MLDSA */

#ifdef WITH_MLDSA65_ED301
void mldsa65_ed301_tests(void);

void
mldsa65_ed301_tests(void)
{
	const u_char msg[] = "ML-DSA65/Ed301 component binding";
	const char prefix[] = "CompositeAlgorithmSignatures2025";
	const char label[] = "COMPSIG-MLDSA65-Ed301-SHA512";
	u_char pk[MLDSA65_ED301_PK_SZ], sk[MLDSA65_ED301_SK_SZ];
	u_char sig[MLDSA65_ED301_SIG_SZ], changed[MLDSA65_ED301_SIG_SZ];
	u_char mprime[sizeof(prefix) - 1 + sizeof(label) - 1 + 1 + 64];
	u_char hash[64], context[256];
	size_t offset = 0;

	if (!ssh_ed301_available()) {
		TEST_START("ML-DSA65/Ed301 keygen failure is transactional");
		memset(pk, 0xa5, sizeof(pk));
		memset(sk, 0xa5, sizeof(sk));
		ASSERT_INT_EQ(crypto_sign_mldsa65_ed301_keygen(pk, sk), -1);
		ASSERT_MEM_ZERO_EQ(pk, sizeof(pk));
		ASSERT_MEM_ZERO_EQ(sk, sizeof(sk));
		TEST_DONE();
		fprintf(stderr, "SKIP ML-DSA65/Ed301: provider unavailable\n");
		return;
	}
	TEST_START("ML-DSA65/Ed301 both components required");
	ASSERT_INT_EQ(crypto_sign_mldsa65_ed301_keygen(pk, sk), 0);
	ASSERT_INT_EQ(crypto_sign_mldsa65_ed301_sign(sig, msg, sizeof(msg),
	    NULL, 0, sk), 0);
	ASSERT_INT_EQ(crypto_sign_mldsa65_ed301_verify(sig, msg, sizeof(msg),
	    NULL, 0, pk), 0);
	memcpy(changed, sig, sizeof(sig));
	changed[0] ^= 1;
	ASSERT_INT_EQ(crypto_sign_mldsa65_ed301_verify(changed, msg,
	    sizeof(msg), NULL, 0, pk), -1);
	memcpy(changed, sig, sizeof(sig));
	changed[MLDSA65_SIGBYTES] ^= 1;
	ASSERT_INT_EQ(crypto_sign_mldsa65_ed301_verify(changed, msg,
	    sizeof(msg), NULL, 0, pk), -1);
	TEST_DONE();

	TEST_START("ML-DSA65/Ed301 independent Mprime and stripping protection");
	ASSERT_INT_EQ(crypto_hash_sha512(hash, msg, sizeof(msg)), 0);
	memcpy(mprime + offset, prefix, sizeof(prefix) - 1);
	offset += sizeof(prefix) - 1;
	memcpy(mprime + offset, label, sizeof(label) - 1);
	offset += sizeof(label) - 1;
	mprime[offset++] = 0;
	memcpy(mprime + offset, hash, sizeof(hash));
	ASSERT_INT_EQ(crypto_sign_mldsa65_verify(sig, mprime, sizeof(mprime),
	    (const u_char *)label, sizeof(label) - 1, pk), 0);
	ASSERT_INT_EQ(crypto_sign_ed301_verify_detached_ctx(
	    sig + MLDSA65_SIGBYTES,
	    mprime, sizeof(mprime), (const u_char *)label, sizeof(label) - 1,
	    pk + MLDSA65_PUBLICKEYBYTES), 0);
	ASSERT_INT_EQ(crypto_sign_mldsa65_verify(sig, mprime, sizeof(mprime),
	    NULL, 0, pk), -1);
	ASSERT_INT_EQ(crypto_sign_ed301_verify_detached(
	    sig + MLDSA65_SIGBYTES,
	    mprime, sizeof(mprime), pk + MLDSA65_PUBLICKEYBYTES), -1);
	ASSERT_INT_EQ(crypto_sign_ed301_verify_detached(
	    sig + MLDSA65_SIGBYTES,
	    msg, sizeof(msg), pk + MLDSA65_PUBLICKEYBYTES), -1);
	TEST_DONE();

	TEST_START("ML-DSA65/Ed301 invalid context clears signature");
	memset(context, 0, sizeof(context));
	memset(changed, 0xa5, sizeof(changed));
	ASSERT_INT_EQ(crypto_sign_mldsa65_ed301_sign(changed, msg, sizeof(msg),
	    context, sizeof(context), sk), -1);
	ASSERT_MEM_ZERO_EQ(changed, sizeof(changed));
	TEST_DONE();
	explicit_bzero(sk, sizeof(sk));
}
#endif /* WITH_MLDSA65_ED301 */
