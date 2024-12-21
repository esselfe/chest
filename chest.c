#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include "chest.h"

static const char *chest_version = "0.3.0";

int use_password_file;
char *password_filename;

const char *chest_default_extension = ".chest";
const char *chest_extension;

static struct option const long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'V'},
	{"extension", required_argument, NULL, 'e'},
	{"password-file", required_argument, NULL, 'p'},
	{NULL, 0, NULL, 0}
};
static char const *short_options = "hVe:p:";

void ChestHelp(void) {
	printf("Usage: chest { -h/--help | -V/--version |\n"
	"    -e/--extension STRING | -p/--password-file FILENAME } FILENAME\n");
}

void RemoveNewline(char *text) {
	char *cp = text;
	int len = strlen(text);
	if (len < 1) return;
	for (int cnt = 0; cnt < len; cnt++, cp++) {
		if (*cp == '\n')
			*cp = '\0';
	}
}

int main(int argc, char **argv) {
	// Parse some of the program options here
	while (1) {
		int c = getopt_long( argc, argv, short_options, long_options, NULL);
		if (c == -1) break;

		switch (c) {
		case 'h':
			ChestHelp();
			exit(0);
		case 'V':
			printf("chest %s\n", chest_version);
			exit(0);
		case 'e':
			if (optarg != NULL && strlen(optarg))
				chest_extension = strdup(optarg);
			
			break;
		case 'p':
			use_password_file = 1;
			if (optarg != NULL)
				password_filename = strdup(optarg);
			
			break;
		default:
			printf("chest warning: unknown option %d/'%c'\n", c, (char)c);
			break;
		}
	}

	// Must have at least 1 argument
	if (argc < 2) {
		ChestHelp();
		exit(EINVAL);
	}
	
	if (chest_extension == NULL)
		chest_extension = chest_default_extension;

	// Chech whether to encrypt or decrypt according to the file extension
	char *filename_s;
	char *filename_d;
	int len = strlen(argv[argc-1]);
	if (len < 1) {
		printf("chest error: filename length can't be zero! Cancelled.\n");
		return ECANCELED;
	}
	if (len >= 7 && 
	  strcmp(argv[argc-1]+strlen(argv[argc-1]) - strlen(chest_extension),
	    chest_extension) == 0) {
		filename_s = (char *)malloc(len+1);
		if (filename_s == NULL) {
			printf("chest error: malloc() returned NULL, exiting.\n");
			return 1;
		}
		sprintf(filename_s, "%s", argv[argc-1]);
		filename_d = (char *)malloc(len-strlen(chest_extension));
		if (filename_d == NULL) {
			free(filename_s);
			printf("chest error: malloc() returned NULL, exiting.\n");
			return 1;
		}
		memset(filename_d, 0, len-strlen(chest_extension));
		for (int cnt = 0; cnt <= len-(strlen(chest_extension)+1); cnt++) {
			filename_d[cnt] = argv[argc-1][cnt];
		}
		Decrypt(filename_s, filename_d);
	}
	else {
		filename_s = (char *)malloc(len+1);
		if (filename_s == NULL) {
			printf("chest error: malloc() returned NULL, exiting.\n");
			return 1;
		}
		sprintf(filename_s, "%s", argv[argc-1]);
		filename_d = (char *)malloc(len + strlen(chest_extension) + 1);
		if (filename_d == NULL) {
			free(filename_s);
			printf("chest error: malloc() returned NULL, exiting.\n");
			return 1;
		}
		sprintf(filename_d, "%s%s", argv[argc-1], chest_extension);
		Encrypt(filename_s, filename_d);
	}

	return 0;
}

