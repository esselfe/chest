#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <openssl/sha.h>

void Decrypt(char *src, char *dst) {
	printf("src: %s\n", src);
	printf("dst: %s\n", dst);

	FILE *fr = fopen(src, "r");
	if (fr == NULL) {
		fprintf(stderr, "chest error: Cannot open %s: %s\n", src, strerror(errno));
		return;
	}

	FILE *fw = fopen(dst, "w+");
	if (fw == NULL) {
		fprintf(stderr, "chest error: Cannot open %s: %s\n", dst, strerror(errno));
		return;
	}

	char *pw = malloc(4096);
	memset(pw, 0, 4096);
	printf("Password: ");
	fflush(stdout);
	fgets(pw, 4096, stdin);
	pw[strlen(pw)-1] = '\0';

	unsigned char *sumpw = malloc(SHA512_DIGEST_LENGTH);
	//SHA512_CTX ctx;
	//SHA512_Init(&ctx);
	//SHA512_Update(&ctx, pw, strlen(pw));
	//SHA512_Final(sumpw, &ctx);

	SHA512((unsigned char *)pw, strlen(pw), sumpw);

	char *buf = malloc(4096);
	char *buf2 = malloc(4096);
	size_t sz;
	int i, cnt = 0;
	memset(buf, 0, 4096);
	while (1) {
		sz = fread(buf, 1, 4096, fr);
		if (sz == 0)
			break;

		for (i=0; i<sz; i++) {
			buf2[i] = buf[i] + sumpw[cnt];

			++cnt;
			if (cnt >= SHA512_DIGEST_LENGTH)
				cnt = 0;
		}

		fwrite(buf2, 1, sz, fw);
		memset(buf, 0, 4096);
		memset(buf2, 0, 4096);
	}

	fclose(fr);
	fclose(fw);
}

