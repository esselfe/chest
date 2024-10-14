
CFLAGS = -std=c11 -Wall -O2 -D_DEFAULT_SOURCE
LDFLAGS = -lcrypto
OBJDIR = obj
OBJS = $(OBJDIR)/decrypt.o $(OBJDIR)/encrypt.o \
$(OBJDIR)/hash.o $(OBJDIR)/chest.o
PROGNAME = chest
PROGNAME_GO = gochest
PROGNAME_RUST = target/release/rschest
PREFIX ?= /usr/local

.PHONY: default all prepare go rust clean install

default: all

all: prepare go rust $(OBJS) $(PROGNAME)
	@ls -l --color=auto $(PROGNAME)

prepare:
	@[ -d $(OBJDIR) ] || mkdir -v $(OBJDIR)

go:
	@{ which go &>/dev/null && go build -v -o $(PROGNAME_GO) gochest.go; } || true

rust:
	@{ which cargo &>/dev/null && cargo build --release; } || true

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
	@rm -rfv target $(OBJDIR) $(PROGNAME) $(PROGNAME_GO) || true

install:
	[ -d $(PREFIX)/bin ] || mkdir -pv $(PREFIX)/bin
	[ -d $(PREFIX)/share/man/man1 ] || mkdir -pv $(PREFIX)/share/man/man1
	@which go &>/dev/null && install -vm 0755 $(PROGNAME_GO) $(PREFIX)/bin/
	@which cargo &>/dev/null && install -vm 0755 $(PROGNAME_RUST) $(PREFIX)/bin/
	@which python3 &>/dev/null && install -vm 0755 chest.py $(PREFIX)/bin/
	install -vm 0755 $(PROGNAME) $(PREFIX)/bin/
	install -vm 0644 chest.1 $(PREFIX)/share/man/man1/
	gzip -9 $(PREFIX)/share/man/man1/chest.1

