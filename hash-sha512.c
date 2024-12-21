#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <openssl/sha.h>

#include "chest.h"

char *HashSHA512FromFile(char *filename) {
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL) {
		printf("chest::HashFromFile() error: Cannot open %s: %s\n",
			filename, strerror(errno));
		exit(1);
	}
	
	fseek(fp, 0, SEEK_END);
	long filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char *pw = malloc(filesize);
	fread(pw, 1, filesize, fp);
	fclose(fp);

	char *sum = malloc(SHA512_DIGEST_LENGTH);
	if (sum == NULL) {
		printf("chest::HashFromFile() error: malloc() returned NULL, exiting.\n");
		exit(1);
	}
	memset(sum, 0, SHA512_DIGEST_LENGTH);
	
	SHA512((const unsigned char *)pw, filesize, (unsigned char *)sum);
	
	free(pw);
	
	return sum;
}

char *HashSHA512FromString(const char *pw) {
	char *sum = malloc(SHA512_DIGEST_LENGTH);
	if (sum == NULL) {
		printf("chest::HashFromString() error: malloc() returned NULL, exiting.\n");
		exit(1);
	}
	memset(sum, 0, SHA512_DIGEST_LENGTH);
	
	SHA512((const unsigned char *)pw, strlen(pw), (unsigned char *)sum);
	
	return sum;
}

