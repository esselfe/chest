#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <openssl/evp.h>

#include "chest.h"

char *HashShake256FromFile(char *filename) {
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL) {
		printf("chest::HashShake256FromFile() error: Cannot open %s: %s\n",
			filename, strerror(errno));
		exit(1);
	}
	
	fseek(fp, 0, SEEK_END);
	long filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char *pw = malloc(filesize);
	fread(pw, 1, filesize, fp);
	fclose(fp);

	hash = malloc(hash_length);
	if (hash == NULL) {
		printf("chest::HashShake256FromFile() error: malloc() returned NULL, exiting.\n");
		exit(ENOMEM);
	}
	
	EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
	
	if (EVP_DigestInit_ex(mdctx, EVP_shake256(), NULL) != 1) {
		printf("chest::HashShake256FromFile() error: Failed to initialize EVP_MD_CTX\n");
		EVP_MD_CTX_free(mdctx);
		exit(ECANCELED);
	}

	if (EVP_DigestUpdate(mdctx, pw, filesize) != 1) {
		printf("chest::HashShake256FromFile() error: Failed to update EVP_MD_CTX\n");
		EVP_MD_CTX_free(mdctx);
		exit(ECANCELED);
	}
	
	if (EVP_DigestFinalXOF(mdctx, (unsigned char *)hash, hash_length) != 1) {
		printf("chest::HashShake256FromFile() error: Failed to finalize EVP_MD_CTX\n");
		EVP_MD_CTX_free(mdctx);
		exit(ECANCELED);
	}
	
	EVP_MD_CTX_free(mdctx);
	free(pw);
	
	return hash;
}

char *HashShake256FromString(const char *pw) {
	hash = malloc(hash_length);
	if (hash == NULL) {
		printf("chest::HashShake256FromString() error: malloc() returned NULL, exiting.\n");
		exit(ENOMEM);
	}
	
	EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
	
	if (EVP_DigestInit_ex(mdctx, EVP_shake256(), NULL) != 1) {
		printf("chest::HashShake256FromString() error: Failed to initialize EVP_MD_CTX\n");
		EVP_MD_CTX_free(mdctx);
		exit(ECANCELED);
	}

	if (EVP_DigestUpdate(mdctx, pw, strlen(pw)) != 1) {
		printf("chest::HashShake256FromString() error: Failed to update EVP_MD_CTX\n");
		EVP_MD_CTX_free(mdctx);
		exit(ECANCELED);
	}
	
	if (EVP_DigestFinalXOF(mdctx, (unsigned char *)hash, hash_length) != 1) {
		printf("chest::HashShake256FromString() error: Failed to finalize EVP_MD_CTX\n");
		EVP_MD_CTX_free(mdctx);
		exit(ECANCELED);
	}

	EVP_MD_CTX_free(mdctx);
		
	return hash;
}

