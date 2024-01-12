#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <openssl/sha.h>

#include "chest.h"

void Encrypt(char *src, char *dst) {
	printf("src: %s\n", src);
	printf("dst: %s\n", dst);

	// Open input file
	FILE *fr = fopen(src, "r");
	if (fr == NULL) {
		fprintf(stderr, "chest error: Cannot open %s: %s\n", src, strerror(errno));
		return;
	}

	// Open output file
	FILE *fw = fopen(dst, "w+");
	if (fw == NULL) {
		fprintf(stderr, "chest error: Cannot open %s: %s\n", dst, strerror(errno));
		fclose(fr);
		return;
	}

	// Ask for a password
	char *pw = malloc(4096);
	if (pw == NULL) {
		printf("chest error: malloc() returned NULL, exiting.\n");
		exit(1);
	}
	memset(pw, 0, 4096);
	printf("New password: ");
	fgets(pw, 4096, stdin);
	RemoveNewline(pw);

	// Example of deprecated code (openssl 3.x)
	//SHA512_CTX ctx;
	//SHA512_Init(&ctx);
	//SHA512_Update(&ctx, pw, strlen(pw));
	//SHA512_Final(sum, &ctx);

	// Compute the hash
	unsigned char *sum = malloc(SHA512_DIGEST_LENGTH);
	if (sum == NULL) {
		printf("chest error: malloc() returned NULL, exiting.\n");
		exit(1);
	}
	memset(sum, 0, SHA512_DIGEST_LENGTH);
	SHA512((unsigned char *)pw, strlen(pw), sum);
	
	memset(pw, 0, 4096);
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
	size_t sz;
	int i, cnt = 0;
	memset(buf, 0, 4096);
	while (1) { // Loop for the whole file content
		sz = fread(buf, 1, 4096, fr);
		if (sz == 0)
			break;

		for (i = 0; i < sz; i++) {
			// The actual encryption happens here
			buf2[i] = buf[i] - sum[cnt];

			++cnt;
			// Reset the hash index if the hash end has been reached
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
	free(sum);
}

