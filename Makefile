
CFLAGS = -std=c11 -Wall -O2 -D_DEFAULT_SOURCE
LDFLAGS = -lcrypto
OBJDIR = obj
OBJS = $(OBJDIR)/decrypt.o $(OBJDIR)/encrypt.o \
$(OBJDIR)/hash.o $(OBJDIR)/chest.o
PROGNAME = chest

.PHONY: all clean

default: prepare $(OBJS) $(PROGNAME)
	@ls -l --color=auto $(PROGNAME)

prepare:
	@[ -d $(OBJDIR) ] || mkdir -v $(OBJDIR)

$(OBJDIR)/decrypt.o: decrypt.c
	gcc -c $(CFLAGS) decrypt.c -o $(OBJDIR)/decrypt.o

$(OBJDIR)/encrypt.o: encrypt.c
	gcc -c $(CFLAGS) encrypt.c -o $(OBJDIR)/encrypt.o

$(OBJDIR)/hash.o: hash.c
	gcc -c $(CFLAGS) hash.c -o $(OBJDIR)/hash.o

$(OBJDIR)/chest.o: chest.c
	gcc -c $(CFLAGS) chest.c -o $(OBJDIR)/chest.o

$(PROGNAME): $(OBJS)
	gcc $(CFLAGS) $(OBJS) -o $(PROGNAME) $(LDFLAGS)

clean:
	@rm -rfv $(OBJDIR) $(PROGNAME) || true

