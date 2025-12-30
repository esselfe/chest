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

// Use for main() program entrypoint, for library clients, needs to be declared.
extern struct ChestGlobals chest_globals;

void ChestRemoveNewline(char *text);
void ChestEncrypt(char *src, char *dst);
void ChestDecrypt(char *src, char *dst);
char *ChestGetPassword(void);
char *ChestHashSHA512FromFile(char *filename);
char *ChestHashSHA512FromString(const char *pw);
char *ChestHashShake256FromFile(char *filename);
char *ChestHashShake256FromString(const char *pw);

#endif /* CHEST_H */

