#ifndef CHEST_H
#define CHEST_H 1

extern int use_password_file;
extern char *password_filename;
extern unsigned int hash_base_size;
extern unsigned int hash_factor;
extern unsigned int hash_length;

void RemoveNewline(char *text);
void Encrypt(char *src, char *dst);
void Decrypt(char *src, char *dst);
char *HashSHA512FromFile(char *filename);
char *HashSHA512FromString(const char *pw);

#endif /* CHEST_H */

