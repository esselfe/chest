#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <sys/stat.h>
#include <openssl/sha.h>

#include "chest.h"

static const char *chest_version = "0.3.7";

struct ChestGlobals chest_globals;

const char *chest_default_extension = ".chest";
const char *chest_extension;

static struct option const long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'V'},
	{"base", required_argument, NULL, 'b'},
	{"extension", required_argument, NULL, 'e'},
	{"factor", required_argument, NULL, 'f'},
	{"password-file", required_argument, NULL, 'p'},
	{"shake256-full-size", no_argument, NULL, 'S'},
	{"shake256", no_argument, NULL, 's'},
	{NULL, 0, NULL, 0}
};
static char const *short_options = "hVb:e:f:p:Ss";

void ChestHelp(void) {
	printf("Usage: chest { -h/--help | -V/--version |\n"
	"    -s/--shake256 [ -b/--base BYTES | -f/--factor NUM ] |\n"
	"    -S/--shake256-full-size |\n"
	"    -e/--extension STRING | -p/--password-file FILENAME } FILENAME\n");
}

void ChestRemoveNewline(char *text) {
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
		int c = getopt_long(argc, argv, short_options, long_options, NULL);
		if (c == -1) break;

		switch (c) {
		case 'h':
			ChestHelp();
			exit(0);
		case 'V':
			printf("chest %s\n", chest_version);
			exit(0);
		case 'b':
			if (optarg != NULL && strlen(optarg)) {
				chest_globals.hash_base = atoi(optarg);
				if (chest_globals.hash_base == 0)
					chest_globals.hash_base = 1024;
			}
			
			break;
		case 'e':
			if (optarg != NULL && strlen(optarg))
				chest_extension = strdup(optarg);
			
			break;
		case 'f':
			if (optarg != NULL && strlen(optarg)) {
				chest_globals.hash_factor = atoi(optarg);
				if (chest_globals.hash_factor == 0)
					chest_globals.hash_factor = 1024;
			}
			
			break;
		case 'p':
			chest_globals.use_password_file = 1;
			if (optarg != NULL)
				chest_globals.password_filename = strdup(optarg);
			
			break;
		case 'S':
			chest_globals.use_shake256 = 1;
			chest_globals.hash_length_is_file_size = 1;
			break;
		case 's':
			chest_globals.use_shake256 = 1;
			break;
		default:
			printf("chest warning: unknown option %d/'%c'\n", c, (char)c);
			break;
		}
	}
	
	if (chest_globals.use_shake256) {
		if (chest_globals.hash_length_is_file_size) {
			struct stat st;
			if (stat(argv[argc-1], &st) < 0) {
				printf("chest:main() error: Cannot stat() '%s': %s\n",
					argv[argc-1], strerror(errno));
				exit(ECANCELED);
			}
			
			chest_globals.hash_length = st.st_size;
		}
		else
			chest_globals.hash_length = chest_globals.hash_base * chest_globals.hash_factor;
	}
	else
		chest_globals.hash_length = SHA512_DIGEST_LENGTH;

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
		printf("chest:main() error: filename length can't be zero! Cancelled.\n");
		return ECANCELED;
	}
	if (len >= strlen(chest_extension) && 
	  strcmp(argv[argc-1]+strlen(argv[argc-1]) - strlen(chest_extension),
	    chest_extension) == 0) {
		filename_s = (char *)malloc(len+1);
		if (filename_s == NULL) {
			printf("chest:main() error: malloc() returned NULL, exiting.\n");
			return 1;
		}
		sprintf(filename_s, "%s", argv[argc-1]);
		filename_d = (char *)malloc(len-strlen(chest_extension));
		if (filename_d == NULL) {
			free(filename_s);
			printf("chest:main() error: malloc() returned NULL, exiting.\n");
			return 1;
		}
		memset(filename_d, 0, len-strlen(chest_extension));
		for (int cnt = 0; cnt <= len-(strlen(chest_extension)+1); cnt++) {
			filename_d[cnt] = argv[argc-1][cnt];
		}
		ChestDecrypt(filename_s, filename_d);
	}
	else {
		filename_s = (char *)malloc(len+1);
		if (filename_s == NULL) {
			printf("chest:main() error: malloc() returned NULL, exiting.\n");
			return 1;
		}
		sprintf(filename_s, "%s", argv[argc-1]);
		filename_d = (char *)malloc(len + strlen(chest_extension) + 1);
		if (filename_d == NULL) {
			free(filename_s);
			printf("chest:main() error: malloc() returned NULL, exiting.\n");
			return 1;
		}
		sprintf(filename_d, "%s%s", argv[argc-1], chest_extension);
		ChestEncrypt(filename_s, filename_d);
	}

	return 0;
}

