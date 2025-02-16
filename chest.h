#ifndef CHEST_H
#define CHEST_H 1

extern int use_password_file;
extern char *password_filename;
extern unsigned int hash_base;
extern unsigned int hash_factor;
extern unsigned int hash_length;
extern unsigned int hash_length_is_file_size;
extern unsigned int use_shake256;
extern char *hash;

void RemoveNewline(char *text);
void Encrypt(char *src, char *dst);
void Decrypt(char *src, char *dst);
char *HashSHA512FromFile(char *filename);
char *HashSHA512FromString(const char *pw);
char *HashShake256FromFile(char *filename);
char *HashShake256FromString(const char *pw);

#endif /* CHEST_H */

