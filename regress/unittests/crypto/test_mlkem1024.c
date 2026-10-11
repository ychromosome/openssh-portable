/*
 * ML-KEM-1024 known-answer and input-check tests.
 * Placed in the public domain.
 */
#include "includes.h"

#ifdef USE_MLKEM1024
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../test_helper/test_helper.h"
#include "crypto_api.h"
#include "sshbuf.h"

struct sshbuf *load_text_file(const char *);
struct sshbuf *get_json_object(struct sshbuf *);
u_char *get_json_hex(struct sshbuf *, const char *, size_t *);
void mlkem1024_tests(void);

static void
check_hash(const u_char *value, size_t size, struct sshbuf *v,
    const char *field)
{
	u_char digest[32], *expected;
	size_t n;

	expected = get_json_hex(v, field, &n);
	ASSERT_SIZE_T_EQ(n, sizeof(digest));
	sha3_256(digest, value, size);
	ASSERT_MEM_EQ(digest, expected, sizeof(digest));
	free(expected);
}

void
mlkem1024_tests(void)
{
	struct sshbuf *json, *array, *v, *text;
	const u_char *p;
	u_char pk[MLKEM1024_PUBLICKEYBYTES], sk[MLKEM1024_SECRETKEYBYTES];
	u_char ct[MLKEM1024_CIPHERTEXTBYTES], ss[MLKEM1024_BYTES];
	u_char recovered[MLKEM1024_BYTES], seed[32];
	u_char *keyseed, *encseed, *expected;
	char *lines, *line, *cursor;
	size_t n, length, count = 0;

	TEST_START("ML-KEM 1024 all NIST KATs");
	json = load_text_file("nistkats_mlkem_1024.json");
	p = sshbuf_ptr(json);
	length = sshbuf_len(json) - 1;
	ASSERT_CHAR_EQ(p[length], '\0');
	while (length > 0 && isspace(p[length - 1]))
		length--;
	ASSERT_CHAR_EQ(p[0], '[');
	ASSERT_CHAR_EQ(p[length - 1], ']');
	ASSERT_PTR_NE(array = sshbuf_from(p + 1, length - 2), NULL);
	while ((v = get_json_object(array)) != NULL) {
		test_subtest_info("vector %zu", count);
		keyseed = get_json_hex(v, "key_generation_seed", &n);
		ASSERT_SIZE_T_EQ(n, crypto_kem_mlkem1024_KEYPAIRSEEDBYTES);
		ASSERT_INT_EQ(crypto_kem_mlkem1024_keypair_seeded(pk, sk,
		    keyseed), 0);
		check_hash(pk, sizeof(pk), v, "sha3_256_hash_of_public_key");
		check_hash(sk, sizeof(sk), v, "sha3_256_hash_of_secret_key");
		encseed = get_json_hex(v, "encapsulation_seed", &n);
		ASSERT_SIZE_T_EQ(n, crypto_kem_mlkem1024_ENCSEEDBYTES);
		ASSERT_INT_EQ(crypto_kem_mlkem1024_enc_seeded(ct, ss, pk,
		    encseed), 0);
		check_hash(ct, sizeof(ct), v, "sha3_256_hash_of_ciphertext");
		expected = get_json_hex(v, "shared_secret", &n);
		ASSERT_SIZE_T_EQ(n, sizeof(ss));
		ASSERT_MEM_EQ(ss, expected, sizeof(ss));
		ASSERT_INT_EQ(crypto_kem_mlkem1024_dec(recovered, ct, sk), 0);
		ASSERT_MEM_EQ(recovered, expected, sizeof(recovered));
		freezero(keyseed, crypto_kem_mlkem1024_KEYPAIRSEEDBYTES);
		freezero(encseed, crypto_kem_mlkem1024_ENCSEEDBYTES);
		freezero(expected, n);
		sshbuf_free(v);
		count++;
	}
	ASSERT_SIZE_T_EQ(count, 100);
	sshbuf_free(array);
	sshbuf_free(json);
	TEST_DONE();
	fprintf(stderr, "ML-KEM 1024: %zu NIST KATs PASS\n", count);

	TEST_START("ML-KEM 1024 all FIPS203 modulus failures");
	text = load_text_file("invalid_modulus_mlkem_1024.txt");
	ASSERT_PTR_NE(lines = sshbuf_dup_string(text), NULL);
	cursor = lines;
	count = 0;
	memset(seed, 0, sizeof(seed));
	while ((line = strsep(&cursor, "\n")) != NULL) {
		if (*line == '\0')
			continue;
		test_subtest_info("modulus vector %zu", count);
		ASSERT_SIZE_T_EQ(strlen(line), sizeof(pk) * 2);
		hex2bin(pk, line, sizeof(pk));
		memset(ct, 0xa5, sizeof(ct));
		memset(ss, 0xa5, sizeof(ss));
		ASSERT_INT_EQ(crypto_kem_mlkem1024_enc_seeded(ct, ss, pk,
		    seed), -1);
		ASSERT_MEM_ZERO_EQ(ct, sizeof(ct));
		ASSERT_MEM_ZERO_EQ(ss, sizeof(ss));
		count++;
	}
	ASSERT_SIZE_T_EQ(count, 1040);
	free(lines);
	sshbuf_free(text);
	TEST_DONE();
	fprintf(stderr, "ML-KEM 1024: %zu modulus failures rejected\n", count);
	explicit_bzero(sk, sizeof(sk));
	explicit_bzero(ss, sizeof(ss));
	explicit_bzero(recovered, sizeof(recovered));
	explicit_bzero(seed, sizeof(seed));
}
#endif /* USE_MLKEM1024 */
