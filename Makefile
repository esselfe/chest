
# Permits setting our own CC to clang or tcc, the default is cc
ifeq ($(CC), cc)
  CC = gcc
endif
CFLAGS = -std=c23 -Wall -O2 -D_DEFAULT_SOURCE
LDFLAGS = -lcrypto

OBJDIR = obj
OBJS = $(OBJDIR)/decrypt.o $(OBJDIR)/encrypt.o \
$(OBJDIR)/hash-sha512.o $(OBJDIR)/hash-shake256.o \
$(OBJDIR)/chest.o

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
	@{ which go &>/dev/null && \
	go build -v -o $(PROGNAME_GO) gochest.go; } || true

rust:
	@{ which cargo &>/dev/null && \
	cargo build --release; } || true

$(OBJDIR)/decrypt.o: decrypt.c
	$(CC) -c $(CFLAGS) decrypt.c -o $(OBJDIR)/decrypt.o

$(OBJDIR)/encrypt.o: encrypt.c
	$(CC) -c $(CFLAGS) encrypt.c -o $(OBJDIR)/encrypt.o

$(OBJDIR)/hash-sha512.o: hash-sha512.c
	$(CC) -c $(CFLAGS) hash-sha512.c -o $(OBJDIR)/hash-sha512.o

$(OBJDIR)/hash-shake256.o: hash-shake256.c
	$(CC) -c $(CFLAGS) hash-shake256.c -o $(OBJDIR)/hash-shake256.o

$(OBJDIR)/chest.o: chest.c
	$(CC) -c $(CFLAGS) chest.c -o $(OBJDIR)/chest.o

$(PROGNAME): $(OBJS)
	$(CC) $(OBJS) -o $(PROGNAME) $(LDFLAGS)

clean:
	@rm -rfv target $(OBJDIR) $(PROGNAME) $(PROGNAME_GO) || true

install:
	@{ which go &>/dev/null && \
	  install -Dvm 0755 $(PROGNAME_GO) $(PREFIX)/bin/; } || true
	@{ which cargo &>/dev/null && \
	  install -Dvm 0755 $(PROGNAME_RUST) $(PREFIX)/bin/; } || true
	@{ which python3 &>/dev/null && \
	  install -Dvm 0755 chest.py $(PREFIX)/bin/; } || true
	install -Dvm 0755 $(PROGNAME) $(PREFIX)/bin/
	install -Dvm 0644 chest.1 $(PREFIX)/share/man/man1/
	gzip -9 $(PREFIX)/share/man/man1/chest.1

