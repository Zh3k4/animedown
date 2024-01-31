VERSION = 0.1

CFLAGS = -std=c99 -pedantic -Wall -Wextra -Os
CFLAGS += -DVERSION=\"${VERSION}\" -D_POSIX_C_SOURCE=200809L
CFLAGS += -Wno-unused-function
LIBS = -lcurl

PREFIX ?= $(HOME)/.local

OBJ = \
	src/animedown.o \
	src/slice.o \
	src/util.o \
	src/xml.o
SRC = $(OBJ:.o=.c)

animedown: $(OBJ)
	@printf 'LD\t%s\n' $@
	@$(CC) -o $@ $^ $(LIBS)

.c.o:
	@printf 'CC\t%s\n' $@
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ): src/config.h Makefile

clean:
	rm -f animedown $(OBJ) animedown-$(VERSION).tar.gz

dist: clean
	mkdir -p animedown-$(VERSION)
	cp -r\
		Makefile LICENSE README\
		src\
		animedown-$(VERSION)
	tar czf animedown-$(VERSION).tar.gz animedown-$(VERSION)
	rm -rf animedown-$(VERSION)

install:
	mkdir -p $(PREFIX)/bin
	install -m755 animedown $(PREFIX)/bin/animedown

uninstall:
	rm -f $(PREFIX)/bin/animedown

tags: $(SRC)
	$(CC) -M $(SRC) |\
		sed -e 's,\\,,' -e 's, ,\n,' |\
		sed -e '/^$$/d' -e '/\.o:[ \t]*$$/d' |\
		ctags --languages=c,c++ --kinds-c=+p -R -L -

.PHONY: all clean dist install uninstall
