#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <ctype.h>

static struct termios saved_attributes;

static void RestoreInput(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &saved_attributes);
}

static void SaveInput(void) {
	tcgetattr(STDIN_FILENO, &saved_attributes);
}

char *GetPassword(void) {
	if (!isatty(STDIN_FILENO)) {
		printf("chest:GetPassword() error: Not a TTY terminal.\n");
		exit(ECANCELED);
	}

	char *pass = malloc(1000000);
	if (pass == NULL) {
		printf("chest:GetPassword() error: malloc() returned NULL!\n");
		exit(ENOMEM);
	}
	memset(pass, 0, 1000000);

	SaveInput();

	// Turn off displaying the input characters
	struct termios tattr;
	tcgetattr (STDIN_FILENO, &tattr);
	tattr.c_lflag &= ~(ICANON | ECHO);	/* Clear ICANON and ECHO. */
	tattr.c_cc[VMIN] = 1;
	tattr.c_cc[VTIME] = 0;
	tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);

	printf("Password: ");
	fflush(stdout);

	int cnt = 0;
	char c = 0;
	while (read(STDIN_FILENO, &c, 1) && cnt < sizeof(pass) - 2) {
		if (c == '\n') break;

		pass[cnt++] = c;
		fputc('*', stdout);
		fflush(stdout);
	}
	printf("\n");

	RestoreInput();

	return pass;
}
