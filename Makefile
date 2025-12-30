
# Permits setting our own CC to clang or tcc, the default is cc
ifeq ($(CC), cc)
  CC = gcc
endif
CFLAGS = -std=c23 -Wall -O2 -D_DEFAULT_SOURCE
LDFLAGS = -lcrypto

OBJDIR = obj
OBJS = $(OBJDIR)/decrypt.o $(OBJDIR)/encrypt.o $(OBJDIR)/get-password.o \
$(OBJDIR)/hash-sha512.o $(OBJDIR)/hash-shake256.o \
$(OBJDIR)/chest.o

OBJDIR_SHARED = objlib
OBJS_LIB = $(OBJDIR_SHARED)/decrypt.o $(OBJDIR_SHARED)/encrypt.o \
$(OBJDIR_SHARED)/get-password.o $(OBJDIR_SHARED)/hash-sha512.o \
$(OBJDIR_SHARED)/hash-shake256.o $(OBJDIR_SHARED)/chest.o

PROGNAME = chest
PROGNAME_GO = gochest
PROGNAME_RUST = target/release/rschest
LIBNAME = libchest.so.1

PREFIX ?= /usr/local

.PHONY: default all shlib prepare go rust install-go install-rust install-python install-shlib install clean

default: all

all: prepare $(if $(USE_GO),go) $(if $(USE_RUST),rust) $(OBJS) $(PROGNAME) shlib
	@ls -l --color=auto $(PROGNAME) $(LIBNAME) 2>/dev/null || true

prepare:
	@[ -d $(OBJDIR) ] || mkdir -v $(OBJDIR)
	@[ -d $(OBJDIR_SHARED) ] || mkdir -v $(OBJDIR_SHARED)

go:
	go build -v -o $(PROGNAME_GO) gochest.go

rust:
	cargo build --release

shlib: $(if $(BUILD_SHARED),$(LIBNAME))

$(LIBNAME): $(OBJS_LIB)
	$(CC) -shared $(OBJS_LIB) -o $(LIBNAME) $(LDFLAGS)

$(OBJDIR_SHARED)/decrypt.o: decrypt.c
	$(CC) -c $(CFLAGS) -fPIC decrypt.c -o $(OBJDIR_SHARED)/decrypt.o

$(OBJDIR_SHARED)/encrypt.o: encrypt.c
	$(CC) -c $(CFLAGS) -fPIC encrypt.c -o $(OBJDIR_SHARED)/encrypt.o

$(OBJDIR_SHARED)/get-password.o: get-password.c
	$(CC) -c $(CFLAGS) -fPIC get-password.c -o $(OBJDIR_SHARED)/get-password.o

$(OBJDIR_SHARED)/hash-sha512.o: hash-sha512.c
	$(CC) -c $(CFLAGS) -fPIC hash-sha512.c -o $(OBJDIR_SHARED)/hash-sha512.o

$(OBJDIR_SHARED)/hash-shake256.o: hash-shake256.c
	$(CC) -c $(CFLAGS) -fPIC hash-shake256.c -o $(OBJDIR_SHARED)/hash-shake256.o

$(OBJDIR_SHARED)/chest.o: chest.c
	$(CC) -c $(CFLAGS) -DBUILD_SHARED=1 -fPIC chest.c -o $(OBJDIR_SHARED)/chest.o

$(OBJDIR)/decrypt.o: decrypt.c
	$(CC) -c $(CFLAGS) decrypt.c -o $(OBJDIR)/decrypt.o

$(OBJDIR)/encrypt.o: encrypt.c
	$(CC) -c $(CFLAGS) encrypt.c -o $(OBJDIR)/encrypt.o

$(OBJDIR)/get-password.o: get-password.c
	$(CC) -c $(CFLAGS) get-password.c -o $(OBJDIR)/get-password.o

$(OBJDIR)/hash-sha512.o: hash-sha512.c
	$(CC) -c $(CFLAGS) hash-sha512.c -o $(OBJDIR)/hash-sha512.o

$(OBJDIR)/hash-shake256.o: hash-shake256.c
	$(CC) -c $(CFLAGS) hash-shake256.c -o $(OBJDIR)/hash-shake256.o

$(OBJDIR)/chest.o: chest.c
	$(CC) -c $(CFLAGS) chest.c -o $(OBJDIR)/chest.o

$(PROGNAME): $(OBJS)
	$(CC) $(OBJS) -o $(PROGNAME) $(LDFLAGS)

install-go:
	@install -Dvm 0755 $(PROGNAME_GO) $(PREFIX)/bin/

install-rust:
	@install -Dvm 0755 $(PROGNAME_RUST) $(PREFIX)/bin/

install-python:
	@install -Dvm 0755 chest.py $(PREFIX)/bin/

install-shlib:
	@[ -f "$(LIBNAME)" ] && { install -Dvm 0755 $(LIBNAME) $(PREFIX)/lib/ && ldconfig; }

install: $(if $(USE_GO),install-go) $(if $(USE_RUST),install-rust) \
$(if $(USE_PYTHON),install-python) $(if $(BUILD_SHARED),install-shlib)
	install -Dvm 0755 $(PROGNAME) $(PREFIX)/bin/
	install -Dvm 0644 chest.1 $(PREFIX)/share/man/man1/
	gzip -9 $(PREFIX)/share/man/man1/chest.1

clean:
	@rm -rfv target $(OBJDIR_SHARED) $(LIBNAME) $(OBJDIR) $(PROGNAME) $(PROGNAME_GO) 2>/dev/null || true

