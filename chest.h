#ifndef CHEST_H
#define CHEST_H 1

struct ChestGlobals {
	int use_password_file;
	char *password_filename;
	unsigned int hash_base;
	unsigned int hash_factor;
	unsigned int hash_length;
	unsigned int hash_length_is_file_size;
	unsigned int use_shake256;
};
extern struct ChestGlobals chest_globals;

#define CHEST_DEFAULT_EXTENSION ".chest"

void ChestHelp(void);
void ChestVersion(void);
void ChestRemoveNewline(char *text);
void ChestEncrypt(char *src, char *dst);
void ChestDecrypt(char *src, char *dst);
char *ChestGetPassword(void);
char *ChestHashSHA512FromFile(char *filename);
char *ChestHashSHA512FromString(const char *pw);
char *ChestHashShake256FromFile(char *filename);
char *ChestHashShake256FromString(const char *pw);

#endif /* CHEST_H */

