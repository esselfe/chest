#ifndef CHEST_H
#define CHEST_H 1

extern int use_password_file;
extern char *password_filename;

void RemoveNewline(char *text);
void Encrypt(char *src, char *dst);
void Decrypt(char *src, char *dst);
char *HashFromFile(char *filename);
char *HashFromString(const char *pw);

#endif /* CHEST_H */

