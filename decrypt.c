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

	char *sum;
	if (use_password_file) {
		if (use_shake256)
			sum = HashShake256FromFile(password_filename);
		else
			sum = HashSHA512FromFile(password_filename);
	}
	else {
		// Ask for a password
		char *pw = malloc(4096);
		if (pw == NULL) {
			printf("chest error: malloc() returned NULL, exiting.\n");
			exit(1);
		}
		memset(pw, 0, 4096);
		printf("Password: ");
		fgets(pw, 4096, stdin);
		RemoveNewline(pw);
		
		if (use_shake256)
			sum = HashShake256FromString(pw);
		else
			sum = HashSHA512FromString(pw);
	
		free(pw);
	}

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
			buf2[i] = buf[i] + sum[cnt];

			++cnt;
			// Reset the hash index if the end has been reached
			if (cnt >= hash_length)
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

