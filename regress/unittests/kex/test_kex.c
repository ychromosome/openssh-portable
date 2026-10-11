/* 	$OpenBSD: test_kex.c,v 1.12 2025/08/21 05:55:30 djm Exp $ */
/*
 * Regress test KEX
 *
 * Placed in the public domain
 */

#include "includes.h"

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../test_helper/test_helper.h"

#include "ssherr.h"
#include "ssh_api.h"
#include "crypto_api.h"
#include "sshbuf.h"
#include "packet.h"
#include "myproposal.h"
#include "log.h"
#include "digest.h"

void kex_tests(void);
#if defined(WITH_ED301) && defined(USE_MLKEM1024)
static void mlkem1024x301_tests(void);
#endif
#ifdef WITH_ED301
static void x301_kex_negative_tests(void);
#endif
static int do_debug = 0;

static int
do_send_and_receive(struct ssh *from, struct ssh *to)
{
	u_char type;
	size_t len;
	const u_char *buf;
	int r;

	for (;;) {
		if ((r = ssh_packet_next(from, &type)) != 0) {
			fprintf(stderr, "ssh_packet_next: %s\n", ssh_err(r));
			return r;
		}
		if (type != 0)
			return 0;
		buf = ssh_output_ptr(from, &len);
		if (do_debug)
			printf("%zu", len);
		if (len == 0)
			return 0;
		if ((r = ssh_output_consume(from, len)) != 0 ||
		    (r = ssh_input_append(to, buf, len)) != 0)
			return r;
	}
}

static void
run_kex(struct ssh *client, struct ssh *server)
{
	int r = 0;

	while (!server->kex->done || !client->kex->done) {
		if (do_debug)
			printf(" S:");
		if ((r = do_send_and_receive(server, client)))
			break;
		if (do_debug)
			printf(" C:");
		if ((r = do_send_and_receive(client, server)))
			break;
	}
	if (do_debug)
		printf("done: %s\n", ssh_err(r));
	ASSERT_INT_EQ(r, 0);
	ASSERT_INT_EQ(server->kex->done, 1);
	ASSERT_INT_EQ(client->kex->done, 1);
}

static void
do_kex_with_key(char *kex, char *cipher, char *mac,
    struct sshkey *key, int keytype, int bits)
{
	struct ssh *client = NULL, *server = NULL, *server2 = NULL;
	struct sshkey *private, *public;
	struct sshbuf *state;
	struct kex_params kex_params;
	char *myproposal[PROPOSAL_MAX] = { KEX_CLIENT };
	char *keyname = NULL;

	if (key != NULL) {
		private = key;
		keytype = key->type;
	} else {
		TEST_START("sshkey_generate");
		ASSERT_INT_EQ(sshkey_generate(keytype, bits, &private), 0);
		TEST_DONE();
	}

	TEST_START("sshkey_from_private");
	ASSERT_INT_EQ(sshkey_from_private(private, &public), 0);
	TEST_DONE();

	TEST_START("ssh_init");
	memcpy(kex_params.proposal, myproposal, sizeof(myproposal));
	if (kex != NULL)
		kex_params.proposal[PROPOSAL_KEX_ALGS] = kex;
	if (cipher != NULL) {
		kex_params.proposal[PROPOSAL_ENC_ALGS_CTOS] = cipher;
		kex_params.proposal[PROPOSAL_ENC_ALGS_STOC] = cipher;
	}
	if (mac != NULL) {
		kex_params.proposal[PROPOSAL_MAC_ALGS_CTOS] = mac;
		kex_params.proposal[PROPOSAL_MAC_ALGS_STOC] = mac;
	}
	keyname = strdup(sshkey_ssh_name(private));
	ASSERT_PTR_NE(keyname, NULL);
	kex_params.proposal[PROPOSAL_SERVER_HOST_KEY_ALGS] = keyname;
	ASSERT_INT_EQ(ssh_init(&client, 0, &kex_params), 0);
	ASSERT_INT_EQ(ssh_init(&server, 1, &kex_params), 0);
	ASSERT_PTR_NE(client, NULL);
	ASSERT_PTR_NE(server, NULL);
	TEST_DONE();

	TEST_START("ssh_add_hostkey");
	ASSERT_INT_EQ(ssh_add_hostkey(server, private), 0);
	ASSERT_INT_EQ(ssh_add_hostkey(client, public), 0);
	TEST_DONE();

	TEST_START("kex");
	run_kex(client, server);
	TEST_DONE();

	TEST_START("rekeying client");
	ASSERT_INT_EQ(kex_send_kexinit(client), 0);
	run_kex(client, server);
	TEST_DONE();

	TEST_START("rekeying server");
	ASSERT_INT_EQ(kex_send_kexinit(server), 0);
	run_kex(client, server);
	TEST_DONE();

	TEST_START("ssh_packet_get_state");
	state = sshbuf_new();
	ASSERT_PTR_NE(state, NULL);
	ASSERT_INT_EQ(ssh_packet_get_state(server, state), 0);
	ASSERT_INT_GE(sshbuf_len(state), 1);
	TEST_DONE();

	TEST_START("ssh_packet_set_state");
	server2 = NULL;
	ASSERT_INT_EQ(ssh_init(&server2, 1, NULL), 0);
	ASSERT_PTR_NE(server2, NULL);
	ASSERT_INT_EQ(ssh_add_hostkey(server2, private), 0);
	ASSERT_INT_EQ(ssh_packet_set_state(server2, state), 0);
	ASSERT_INT_EQ(sshbuf_len(state), 0);
	sshbuf_free(state);
	ASSERT_PTR_NE(server2->kex, NULL);
	/* XXX we need to set the callbacks */
#ifdef WITH_OPENSSL
	server2->kex->kex[KEX_DH_GRP1_SHA1] = kex_gen_server;
	server2->kex->kex[KEX_DH_GRP14_SHA1] = kex_gen_server;
	server2->kex->kex[KEX_DH_GRP14_SHA256] = kex_gen_server;
	server2->kex->kex[KEX_DH_GRP16_SHA512] = kex_gen_server;
	server2->kex->kex[KEX_DH_GRP18_SHA512] = kex_gen_server;
	server2->kex->kex[KEX_DH_GEX_SHA1] = kexgex_server;
	server2->kex->kex[KEX_DH_GEX_SHA256] = kexgex_server;
	server2->kex->kex[KEX_ECDH_SHA2] = kex_gen_server;
#endif /* WITH_OPENSSL */
#ifdef WITH_ED301
#ifdef USE_MLKEM1024
	server2->kex->kex[KEX_MLKEM1024_X301_SHA512] = kex_gen_server;
#endif
	server2->kex->kex[KEX_X301_SHA512] = kex_gen_server;
#endif
	server2->kex->kex[KEX_C25519_SHA256] = kex_gen_server;
	server2->kex->kex[KEX_KEM_SNTRUP761X25519_SHA512] = kex_gen_server;
	server2->kex->kex[KEX_KEM_MLKEM768X25519_SHA256] = kex_gen_server;
	server2->kex->load_host_public_key = server->kex->load_host_public_key;
	server2->kex->load_host_private_key = server->kex->load_host_private_key;
	server2->kex->sign = server->kex->sign;
	TEST_DONE();

	TEST_START("rekeying server2");
	ASSERT_INT_EQ(kex_send_kexinit(server2), 0);
	run_kex(client, server2);
	ASSERT_INT_EQ(kex_send_kexinit(client), 0);
	run_kex(client, server2);
	TEST_DONE();

	TEST_START("cleanup");
	sshkey_free(public);
	if (key == NULL)
		sshkey_free(private);
	ssh_free(client);
	ssh_free(server);
	ssh_free(server2);
	free(keyname);
	TEST_DONE();
}

static void
do_kex(char *kex)
{
	struct sshkey *key = NULL;
	char name[256];

	if (test_is_benchmark()) {
		snprintf(name, sizeof(name), "generate %s", kex);
		TEST_START(name);
		ASSERT_INT_EQ(sshkey_generate(KEY_ED25519, 0, &key), 0);
		TEST_DONE();
		snprintf(name, sizeof(name), "KEX %s", kex);
		BENCH_START(name);
		/*
		 * NB. use a cipher/MAC here that requires minimal bits from
		 * the KEX to avoid DH-GEX taking forever.
		 */
		do_kex_with_key(kex, "aes128-ctr", "hmac-sha2-256", key,
		    KEY_ED25519, 256);
		BENCH_FINISH("kex");
		sshkey_free(key);
		return;
	}

#ifdef WITH_OPENSSL
	do_kex_with_key(kex, NULL, NULL, NULL, KEY_RSA, 2048);
	do_kex_with_key(kex, NULL, NULL, NULL, KEY_ECDSA, 256);
#endif /* WITH_OPENSSL */
	do_kex_with_key(kex, NULL, NULL, NULL, KEY_ED25519, 256);
#ifdef WITH_ED301
	if (strcmp(kex, KEX_X301_SHA512_NAME) == 0 ||
	    strcmp(kex, KEX_MLKEM1024_X301_SHA512_NAME) == 0)
		do_kex_with_key(kex, NULL, NULL, NULL,
		    KEY_MLDSA44_ED25519, 512);
#endif
#ifdef WITH_ED301
	if (ssh_ed301_available()) {
		do_kex_with_key(kex, NULL, NULL, NULL, KEY_ED301, 304);
#ifdef WITH_MLDSA65_ED301
		do_kex_with_key(kex, NULL, NULL, NULL, KEY_MLDSA65_ED301, 560);
#endif
	}
	else
		fprintf(stderr, "SKIP Ed301 host-key KEX test: "
		    "provider unavailable\n");
#endif
}

void
kex_tests(void)
{
#ifdef WITH_ED301
	if (ssh_x301_available())
	{
		do_kex(KEX_X301_SHA512_NAME);
#ifdef USE_MLKEM1024
		do_kex(KEX_MLKEM1024_X301_SHA512_NAME);
#endif
	}
	else
		fprintf(stderr, "SKIP X301 KEX: provider unavailable\n");
#endif
	do_kex("curve25519-sha256");
#ifdef WITH_ED301
	x301_kex_negative_tests();
#ifdef USE_MLKEM1024
	mlkem1024x301_tests();
#endif
#endif
#ifdef WITH_OPENSSL
	do_kex("ecdh-sha2-nistp256");
	do_kex("ecdh-sha2-nistp384");
	do_kex("ecdh-sha2-nistp521");
	do_kex("diffie-hellman-group-exchange-sha256");
	do_kex("diffie-hellman-group-exchange-sha1");
	do_kex("diffie-hellman-group14-sha1");
	do_kex("diffie-hellman-group1-sha1");
	if (test_is_benchmark()) {
		do_kex("diffie-hellman-group14-sha256");
		do_kex("diffie-hellman-group16-sha512");
		do_kex("diffie-hellman-group18-sha512");
	}
# ifdef USE_MLKEM768X25519
	do_kex("mlkem768x25519-sha256");
# endif /* USE_MLKEM768X25519 */
# ifdef USE_SNTRUP761X25519
	do_kex("sntrup761x25519-sha512");
# endif /* USE_SNTRUP761X25519 */
#endif /* WITH_OPENSSL */
}

#ifdef WITH_ED301
#include "x301-vectors.h"

static void
x301_kex_negative_tests(void)
{
	struct kex kex;
	struct sshbuf *input, *reply = NULL, *secret = NULL;
	u_char wrong[39] = { 0 };
	size_t i;

	if (!ssh_x301_available()) {
		fprintf(stderr, "SKIP X301 KEX negatives: provider unavailable\n");
		return;
	}
	memset(&kex, 0, sizeof(kex));
	TEST_START("X301 KEX public lengths 37 and 39");
	for (i = 37; i <= 39; i += 2) {
		ASSERT_PTR_NE(input = sshbuf_from(wrong, i), NULL);
		ASSERT_INT_NE(kex_x301_enc(&kex, input, &reply, &secret), 0);
		ASSERT_PTR_EQ(reply, NULL);
		ASSERT_PTR_EQ(secret, NULL);
		ASSERT_INT_NE(kex_x301_dec(&kex, input, &secret), 0);
		ASSERT_PTR_EQ(secret, NULL);
		sshbuf_free(input);
	}
	TEST_DONE();
	TEST_START("X301 KEX canonical, weak-point and weak-secret vectors");
	for (i = 0; i < sizeof(x301_kex_errors) /
	    sizeof(*x301_kex_errors); i++) {
		test_subtest_info("%s", x301_kex_errors[i].id);
		memcpy(kex.x301_client_key, x301_kex_errors[i].secret, 38);
		ASSERT_PTR_NE(input = sshbuf_from(
		    x301_kex_errors[i].public, 38), NULL);
		ASSERT_INT_NE(kex_x301_dec(&kex, input, &secret), 0);
		ASSERT_PTR_EQ(secret, NULL);
		ASSERT_MEM_ZERO_EQ(kex.x301_client_key, 38);
		if (x301_kex_errors[i].invalid_public) {
			ASSERT_INT_NE(kex_x301_enc(&kex, input, &reply,
			    &secret), 0);
			ASSERT_PTR_EQ(reply, NULL);
			ASSERT_PTR_EQ(secret, NULL);
		}
		sshbuf_free(input);
	}
	TEST_DONE();
	fprintf(stderr, "X301 KEX negatives: 2 lengths, %zu vectors PASS\n",
	    sizeof(x301_kex_errors) / sizeof(*x301_kex_errors));
}
#endif /* WITH_ED301 */

#if defined(WITH_ED301) && defined(USE_MLKEM1024)
static void
mlkem1024x301_tests(void)
{
	struct kex kex;
	struct sshbuf *reply = NULL, *server_secret = NULL;
	struct sshbuf *client_secret = NULL, *input, *encoded;
	u_char combined[MLKEM1024_BYTES + crypto_x301_BYTES];
	u_char expected[64], bytes[1607];
	const u_char *hash;
	size_t len, i;

	if (!ssh_x301_available()) {
		fprintf(stderr, "SKIP ML-KEM1024/X301 KEX primitives: "
		    "provider unavailable\n");
		return;
	}
	memset(&kex, 0, sizeof(kex));
	kex.hash_alg = SSH_DIGEST_SHA512;
	TEST_START("ML-KEM1024/X301 independent SHA512 combiner");
	ASSERT_INT_EQ(kex_kem_mlkem1024x301_keypair(&kex), 0);
	ASSERT_SIZE_T_EQ(sshbuf_len(kex.client_pub), 1606);
	ASSERT_INT_EQ(kex_kem_mlkem1024x301_enc(&kex, kex.client_pub,
	    &reply, &server_secret), 0);
	ASSERT_SIZE_T_EQ(sshbuf_len(reply), 1606);
	/* Recover each component separately, then assemble K in the test. */
	ASSERT_INT_EQ(crypto_kem_mlkem1024_dec(combined, sshbuf_ptr(reply),
	    kex.mlkem1024_client_key), 0);
	ASSERT_INT_EQ(crypto_x301_shared(combined + MLKEM1024_BYTES,
	    kex.x301_client_key,
	    sshbuf_ptr(reply) + MLKEM1024_CIPHERTEXTBYTES), 0);
	ASSERT_INT_EQ(crypto_hash_sha512(expected, combined, sizeof(combined)), 0);
	ASSERT_PTR_NE(encoded = sshbuf_fromb(server_secret), NULL);
	ASSERT_INT_EQ(sshbuf_get_string_direct(encoded, &hash, &len), 0);
	ASSERT_SIZE_T_EQ(len, 64);
	ASSERT_MEM_EQ(hash, expected, 64);
	ASSERT_SIZE_T_EQ(sshbuf_len(encoded), 0);
	ASSERT_INT_EQ(kex_kem_mlkem1024x301_dec(&kex, reply,
	    &client_secret), 0);
	ASSERT_SIZE_T_EQ(sshbuf_len(client_secret), sshbuf_len(server_secret));
	ASSERT_MEM_EQ(sshbuf_ptr(client_secret), sshbuf_ptr(server_secret),
	    sshbuf_len(server_secret));
	ASSERT_MEM_ZERO_EQ(kex.x301_client_key, sizeof(kex.x301_client_key));
	ASSERT_MEM_ZERO_EQ(kex.mlkem1024_client_key,
	    sizeof(kex.mlkem1024_client_key));
	sshbuf_free(kex.client_pub);
	sshbuf_free(encoded);
	sshbuf_free(reply);
	sshbuf_free(server_secret);
	sshbuf_free(client_secret);
	reply = server_secret = client_secret = NULL;
	TEST_DONE();

	TEST_START("ML-KEM1024/X301 rejects lengths, modulus and weak X301");
	for (i = 0; i < 7; i++) {
		test_subtest_info("mutation %zu", i);
		memset(&kex, 0, sizeof(kex));
		kex.hash_alg = SSH_DIGEST_SHA512;
		ASSERT_INT_EQ(kex_kem_mlkem1024x301_keypair(&kex), 0);
		if (i < 4) {
			memcpy(bytes, sshbuf_ptr(kex.client_pub), 1606);
			bytes[1606] = 0;
			len = i == 0 ? 1605 : i == 1 ? 1607 : 1606;
			if (i == 2) { /* first encoded coefficient = 4095 */
				bytes[0] = 0xff;
				bytes[1] |= 0x0f;
			}
			if (i == 3)
				memset(bytes + MLKEM1024_PUBLICKEYBYTES, 0, 38);
			ASSERT_PTR_NE(input = sshbuf_from(bytes, len), NULL);
			ASSERT_INT_NE(kex_kem_mlkem1024x301_enc(&kex, input,
			    &reply, &server_secret), 0);
			ASSERT_PTR_EQ(reply, NULL);
			ASSERT_PTR_EQ(server_secret, NULL);
		} else {
			ASSERT_INT_EQ(kex_kem_mlkem1024x301_enc(&kex,
			    kex.client_pub, &reply, &server_secret), 0);
			memcpy(bytes, sshbuf_ptr(reply), 1606);
			bytes[1606] = 0;
			len = i == 4 ? 1605 : i == 5 ? 1607 : 1606;
			if (i == 6)
				memset(bytes + MLKEM1024_CIPHERTEXTBYTES, 0, 38);
			ASSERT_PTR_NE(input = sshbuf_from(bytes, len), NULL);
			ASSERT_INT_NE(kex_kem_mlkem1024x301_dec(&kex, input,
			    &client_secret), 0);
			ASSERT_PTR_EQ(client_secret, NULL);
			ASSERT_MEM_ZERO_EQ(kex.x301_client_key,
			    sizeof(kex.x301_client_key));
			ASSERT_MEM_ZERO_EQ(kex.mlkem1024_client_key,
			    sizeof(kex.mlkem1024_client_key));
		}
		sshbuf_free(input);
		sshbuf_free(kex.client_pub);
		sshbuf_free(reply);
		sshbuf_free(server_secret);
		reply = server_secret = NULL;
		explicit_bzero(&kex, sizeof(kex));
	}
	TEST_DONE();
	explicit_bzero(combined, sizeof(combined));
	explicit_bzero(expected, sizeof(expected));
}
#endif /* WITH_ED301 && USE_MLKEM1024 */
