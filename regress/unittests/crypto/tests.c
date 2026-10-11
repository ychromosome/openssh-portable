/* 	$OpenBSD: tests.c,v 1.2 2026/06/16 08:15:35 dtucker Exp $ */
/*
 * Regress test for crypto ergonomic API
 *
 * Placed in the public domain
 */

#include "includes.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "../test_helper/test_helper.h"
#include "sshbuf.h"

void mldsa_tests(void);
void mlkem_tests(void);
void ed25519_tests(void);
void mldsa_eddsa_tests(void);

#if defined(WITH_OPENSSL) && defined(USE_ED301)
void ed301_tests(void);
void x301_tests(void);
struct sshbuf *get_json_array(struct sshbuf *, const char *);
struct sshbuf *get_json_object(struct sshbuf *);
int get_json_boolean(struct sshbuf *, const char *);
u_int get_json_uint(struct sshbuf *, const char *);
u_char *get_json_hex(struct sshbuf *, const char *, size_t *);
#endif


struct sshbuf *load_text_file(const char *name);
char *get_json_string(struct sshbuf *content, const char *key, int consume);

static struct sshbuf *
load_file(const char *name)
{
	struct sshbuf *ret = NULL;

	ASSERT_INT_EQ(sshbuf_load_file(test_data_file(name), &ret), 0);
	ASSERT_PTR_NE(ret, NULL);
	return ret;
}

struct sshbuf *
load_text_file(const char *name)
{
	struct sshbuf *ret = load_file(name);
	const u_char *p;
	size_t len;

	/* Trim whitespace at EOL */
	for (p = sshbuf_ptr(ret); (len = sshbuf_len(ret)) > 0;) {
		len--;
		if (p[len] == '\r' || p[len] == '\t' ||
		    p[len] == ' ' || p[len] == '\n')
			ASSERT_INT_EQ(sshbuf_consume_end(ret, 1), 0);
		else
			break;
	}
	/* \0 terminate */
	ASSERT_INT_EQ(sshbuf_put_u8(ret, 0), 0);
	return ret;
}


/*
 * Simple JSON-ish parser for test vectors.
 * Extracts the value for a given key.
 * Errors cause ASSERT_* failures.
 */
char *
get_json_string(struct sshbuf *content, const char *key, int consume)
{
	struct sshbuf *tmp;
	char *k, *ret;
	size_t off, end_off;
	u_char c;

	tmp = sshbuf_fromb(content);
	ASSERT_PTR_NE(tmp, NULL);
	ASSERT_INT_GT(asprintf(&k, "\"%s\"", key), 0);
	if (sshbuf_find(tmp, 0, k, strlen(k), &off) != 0) {
		fprintf(stderr, "Key %s not found in JSON\n", k);
		ASSERT_INT_EQ(1, 0);
	}
	ASSERT_INT_EQ(sshbuf_consume(tmp, off + strlen(k)), 0);
	free(k);

	/* Skip colon, spaces, commas */
	while (sshbuf_len(tmp) > 0) {
		c = *sshbuf_ptr(tmp);
		if (isspace(c) || c == ':' || c == ',')
			ASSERT_INT_EQ(sshbuf_consume(tmp, 1), 0);
		else
			break;
	}
	/* Expect opening quote */
	ASSERT_INT_EQ(sshbuf_get_u8(tmp, &c), 0);
	ASSERT_CHAR_EQ(c, '"');
	/* Find closing quote */
	ASSERT_INT_EQ(sshbuf_find(tmp, 0, "\"", 1, &end_off), 0);
	ASSERT_PTR_NE(ret = malloc(end_off + 1), NULL);
	memcpy(ret, sshbuf_ptr(tmp), end_off);
	ret[end_off] = '\0';
	if (consume) {
		ASSERT_INT_EQ(sshbuf_consume(tmp, end_off), 0);
		ASSERT_INT_EQ(sshbuf_consume_upto_child(content, tmp), 0);
	}
	sshbuf_free(tmp);
	return ret;
}


#if defined(WITH_OPENSSL) && defined(USE_ED301)
/* Container and scalar helpers for the hash-bound Ed301/X301 vectors. */
static size_t
json_value_offset(struct sshbuf *content, const char *key)
{
	const u_char *p = sshbuf_ptr(content);
	size_t off, len = sshbuf_len(content);
	char *k;

	ASSERT_INT_GT(asprintf(&k, "\"%s\"", key), 0);
	ASSERT_INT_EQ(sshbuf_find(content, 0, k, strlen(k), &off), 0);
	off += strlen(k);
	free(k);
	while (off < len && isspace(p[off]))
		off++;
	ASSERT_SIZE_T_LT(off, len);
	ASSERT_CHAR_EQ(p[off++], ':');
	while (off < len && isspace(p[off]))
		off++;
	ASSERT_SIZE_T_LT(off, len);
	return off;
}

static size_t
json_container_length(const u_char *p, size_t len, u_char open, u_char close)
{
	size_t i, depth = 0;
	int quoted = 0, escaped = 0;

	ASSERT_SIZE_T_GT(len, 0);
	ASSERT_CHAR_EQ(p[0], open);
	for (i = 0; i < len; i++) {
		if (quoted) {
			if (escaped)
				escaped = 0;
			else if (p[i] == '\\')
				escaped = 1;
			else if (p[i] == '"')
				quoted = 0;
			continue;
		}
		if (p[i] == '"')
			quoted = 1;
		else if (p[i] == open)
			depth++;
		else if (p[i] == close && --depth == 0)
			return i + 1;
	}
	ASSERT_INT_EQ(1, 0);
	return 0;
}

struct sshbuf *
get_json_array(struct sshbuf *content, const char *key)
{
	struct sshbuf *ret;
	size_t off = json_value_offset(content, key), len;
	const u_char *p = sshbuf_ptr(content) + off;

	len = json_container_length(p, sshbuf_len(content) - off, '[', ']');
	ASSERT_PTR_NE(ret = sshbuf_from(p + 1, len - 2), NULL);
	return ret;
}

struct sshbuf *
get_json_object(struct sshbuf *array)
{
	struct sshbuf *ret;
	size_t len;
	const u_char *p;

	while (sshbuf_len(array) > 0) {
		p = sshbuf_ptr(array);
		if (!isspace(*p) && *p != ',')
			break;
		ASSERT_INT_EQ(sshbuf_consume(array, 1), 0);
	}
	if (sshbuf_len(array) == 0)
		return NULL;
	p = sshbuf_ptr(array);
	len = json_container_length(p, sshbuf_len(array), '{', '}');
	ASSERT_PTR_NE(ret = sshbuf_from(p, len), NULL);
	ASSERT_INT_EQ(sshbuf_consume(array, len), 0);
	return ret;
}

int
get_json_boolean(struct sshbuf *content, const char *key)
{
	size_t off = json_value_offset(content, key);
	size_t len = sshbuf_len(content) - off, n;
	const u_char *p = sshbuf_ptr(content) + off;
	int ret;

	if (len >= 4 && memcmp(p, "true", 4) == 0) {
		ret = 1;
		n = 4;
	} else {
		ASSERT_SIZE_T_GE(len, 5);
		ASSERT_MEM_EQ(p, "false", 5);
		ret = 0;
		n = 5;
	}
	ASSERT_SIZE_T_LT(n, len);
	ASSERT_INT_EQ(isspace(p[n]) || p[n] == ',' || p[n] == '}', 1);
	return ret;
}

u_int
get_json_uint(struct sshbuf *content, const char *key)
{
	size_t off = json_value_offset(content, key);
	const u_char *p = sshbuf_ptr(content);
	u_int n = 0;

	ASSERT_INT_NE(isdigit(p[off]), 0);
	while (off < sshbuf_len(content) && isdigit(p[off])) {
		ASSERT_U_INT_LE(n, 1000);
		n = n * 10 + p[off++] - '0';
	}
	ASSERT_SIZE_T_LT(off, sshbuf_len(content));
	ASSERT_INT_EQ(isspace(p[off]) || p[off] == ',' || p[off] == '}', 1);
	return n;
}

u_char *
get_json_hex(struct sshbuf *content, const char *key, size_t *lenp)
{
	char *hex = get_json_string(content, key, 0);
	u_char *ret;

	ASSERT_SIZE_T_EQ(strlen(hex) % 2, 0);
	*lenp = strlen(hex) / 2;
	ASSERT_PTR_NE(ret = malloc(*lenp == 0 ? 1 : *lenp), NULL);
	hex2bin(ret, hex, *lenp);
	free(hex);
	return ret;
}
#endif /* WITH_OPENSSL && USE_ED301 */

void
tests(void)
{
#ifdef USE_MLDSA
	mldsa_tests();
#endif
#ifdef USE_MLKEM768X25519
	mlkem_tests();
#endif
	ed25519_tests();
#if defined(WITH_OPENSSL) && defined(USE_ED301)
	ed301_tests();
	x301_tests();
#endif
#ifdef USE_MLDSA
	mldsa_eddsa_tests();
#endif
}

void
benchmarks(void)
{
	/* none */
}
