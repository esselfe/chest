#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include "chest.h"

static const char *chest_version = "0.1.1";

static struct option const long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'V'},
	{NULL, 0, NULL, 0}
};
static char const *short_options = "hV";

void ChestHelp(void) {
	printf("Usage: chest { -h/--help | -V/--version } FILENAME\n");
}

int main(int argc, char **argv) {
	int c;
	while (1) {
		c = getopt_long( argc, argv, short_options, long_options, NULL);
		if (c == -1) break;

		switch (c) {
		case 'h':
			ChestHelp();
			exit(0);
			break;
		case 'V':
			printf("chest %s\n", chest_version);
			exit(0);
			break;
		}
	}

	if (argc < 2) {
		ChestHelp();
		exit(EINVAL);
	}

	char *filename_s, *filename_d;
	int len = strlen(argv[1]);
	if (strcmp(argv[1]+strlen(argv[1])-6, ".chest") == 0) {
		filename_s = (char *)malloc(len);
		sprintf(filename_s, "%s", argv[1]);
		filename_d = (char *)malloc(len-7);
		int cnt;
		for (cnt = 0; cnt <= len-7; cnt++) {
			filename_d[cnt] = argv[1][cnt];
		}
		Decrypt(filename_s, filename_d);
	}
	else {
		filename_s = (char *)malloc(len);
		sprintf(filename_s, "%s", argv[1]);
		filename_d = (char *)malloc(len+6);
		sprintf(filename_d, "%s.chest", argv[1]);
		Encrypt(filename_s, filename_d);
	}

	return 0;
}

