#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <openssl/sha.h>

#include "chest.h"

char *ChestHashSHA512FromFile(char *filename) {
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL) {
		printf("ChestHashSHA512FromFile() error: Cannot open %s: %s\n",
			filename, strerror(errno));
		exit(1);
	}
	
	fseek(fp, 0, SEEK_END);
	long filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char *pw = malloc(filesize);
	if (pw == NULL) {
		printf("ChestHashSHA512FromString() error: malloc() returned NULL, exiting.\n");
		fclose(fp);
		exit(ENOMEM);
	}
	fread(pw, 1, filesize, fp);
	fclose(fp);

	char *sum = malloc(SHA512_DIGEST_LENGTH);
	if (sum == NULL) {
		printf("ChestHashSHA512FromFile() error: malloc() returned NULL, exiting.\n");
		exit(1);
	}
	memset(sum, 0, SHA512_DIGEST_LENGTH);
	
	SHA512((const unsigned char *)pw, filesize, (unsigned char *)sum);
	
	free(pw);
	
	return sum;
}

char *ChestHashSHA512FromString(const char *pw) {
	char *sum = malloc(SHA512_DIGEST_LENGTH);
	if (sum == NULL) {
		printf("ChestHashSHA512FromString() error: malloc() returned NULL, exiting.\n");
		exit(ENOMEM);
	}
	memset(sum, 0, SHA512_DIGEST_LENGTH);
	
	SHA512((const unsigned char *)pw, strlen(pw), (unsigned char *)sum);
	
	return sum;
}

