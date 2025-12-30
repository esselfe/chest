#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <openssl/sha.h>

#include "chest.h"

void ChestEncrypt(char *src, char *dst) {
	printf("src: %s\n", src);
	printf("dst: %s\n", dst);

	// Open input file
	FILE *fr = fopen(src, "rb");
	if (fr == NULL) {
		fprintf(stderr, "ChestEncrypt() error: Cannot open %s: %s\n", src, strerror(errno));
		return;
	}

	// Open output file
	FILE *fw = fopen(dst, "wb+");
	if (fw == NULL) {
		fprintf(stderr, "ChestEncrypt() error: Cannot open %s: %s\n", dst, strerror(errno));
		fclose(fr);
		return;
	}

	char *sum;
	if (chest_globals.use_password_file)
		if (chest_globals.use_shake256)
			sum = ChestHashShake256FromFile(chest_globals.password_filename);
		else
			sum = ChestHashSHA512FromFile(chest_globals.password_filename);
	else {
		// Ask for a password
		char *pw = ChestGetPassword();
		if (pw == NULL) {
			printf("ChestEncrypt() error: ChestGetPassword() returned NULL, exiting.\n");
			exit(1);
		}
		ChestRemoveNewline(pw);
		
		if (chest_globals.use_shake256)
			sum = ChestHashShake256FromString(pw);
		else
			sum = ChestHashSHA512FromString(pw);
	
		free(pw);
	}

	char *buf = malloc(4096);
	if (buf == NULL) {
		printf("ChestEncrypt() error: malloc() returned NULL, exiting.\n");
		exit(1);
	}
	char *buf2 = malloc(4096);
	if (buf2 == NULL) {
		printf("ChestEncrypt() error: malloc() returned NULL, exiting.\n");
		exit(1);
	}
	int cnt = 0;
	memset(buf, 0, 4096);
	while (1) { // Loop for the whole file content
		size_t sz = fread(buf, 1, 4096, fr);
		if (sz == 0)
			break;

		for (int i = 0; i < sz; i++) {
			// The actual encryption happens here
			buf2[i] = buf[i] - sum[cnt];

			++cnt;
			// Reset the hash index if the hash end has been reached
			if (cnt >= chest_globals.hash_length)
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

