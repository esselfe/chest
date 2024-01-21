#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <openssl/sha.h>

#include "chest.h"

void Decrypt(char *src, char *dst) {
	printf("src: %s\n", src);
	printf("dst: %s\n", dst);

	// Open input file
	FILE *fr = fopen(src, "rb");
	if (fr == NULL) {
		fprintf(stderr, "chest error: Cannot open %s: %s\n", src, strerror(errno));
		return;
	}

	// Open output file
	FILE *fw = fopen(dst, "wb+");
	if (fw == NULL) {
		fprintf(stderr, "chest error: Cannot open %s: %s\n", dst, strerror(errno));
		fclose(fr);
		return;
	}

	// Ask for the password
	char *pw = malloc(4096);
	if (pw == NULL) {
		printf("chest error: malloc() returned NULL, exiting.\n");
		exit(1);
	}
	memset(pw, 0, 4096);
	printf("Password: ");
	fflush(stdout);
	fgets(pw, 4096, stdin);
	RemoveNewline(pw);

	unsigned char *sumpw = malloc(SHA512_DIGEST_LENGTH);
	if (sumpw == NULL) {
		printf("chest error: malloc() returned NULL, exiting.\n");
		exit(1);
	}
	// Example of deprecated code (openssl 3.x)
	//SHA512_CTX ctx;
	//SHA512_Init(&ctx);
	//SHA512_Update(&ctx, pw, strlen(pw));
	//SHA512_Final(sumpw, &ctx);

	// Compute the hash
	SHA512((unsigned char *)pw, strlen(pw), sumpw);
	free(pw);

	char *buf = malloc(4096);
	if (buf == NULL) {
		printf("chest error: malloc() returned NULL, exiting.\n");
		exit(1);
	}
	char *buf2 = malloc(4096);
	if (buf2 == NULL) {
		printf("chest error: malloc() returned NULL, exiting.\n");
		exit(1);
	}
	int cnt = 0;
	memset(buf, 0, 4096);
	while (1) { // Loop for the whole file content
		size_t sz = fread(buf, 1, 4096, fr);
		if (sz == 0)
			break;

		for (int i = 0; i < sz; i++) {
			// The actual decryption happens here
			buf2[i] = buf[i] + sumpw[cnt];

			++cnt;
			// Reset the hash index if the end has been reached
			if (cnt >= SHA512_DIGEST_LENGTH)
				cnt = 0;
		}

		// Write output and clear the buffers
		fwrite(buf2, 1, sz, fw);
		memset(buf, 0, 4096);
		memset(buf2, 0, 4096);
	}

	fclose(fr);
	fclose(fw);
	free(buf);
	free(buf2);
	free(sumpw);
}

