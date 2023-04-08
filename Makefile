include config.mk

.PHONY: all options clean dist install uninstall

SRC = animedown.c
OBJ = $(SRC:.c=.o)

all: options animedown

run: options animedown
	./animedown

options:
	@echo animedown build options:
	@echo "CFLAGS   = $(CFLAGS)"
	@echo "LDFLAGS  = $(LDFLAGS)"
	@echo "CC       = $(CC)"

tags: $(SRC)
	$(CC) -M $(SRC) |\
		sed -e 's,\\,,' -e 's, ,\n,' |\
		sed -e '/^$$/d' -e '/\.o:[ \t]*$$/d' |\
		ctags --languages=c,c++ --kinds-c=+p -R -L -

.c.o:
	$(CC) -c $(CFLAGS) $<

$(OBJ): config.h config.mk

config.h:
	cp config.def.h config.h

animedown: $(OBJ)
	$(CC) -o $@ $^

clean:
	rm -f animedown $(OBJ) animedown-$(VERSION).tar.gz

dist: clean
	mkdir -p animedown-$(VERSION)
	cp\
		Makefile config.mk LICENSE README\
		config.def.h $(SRC)\
		animedown-$(VERSION)
	tar -czf animedown-$(VERSION).tar.gz animedown-$(VERSION)
	rm -rf animedown-$(VERSION)

install:
	mkdir -p $(PREFIX)/bin
	cp -f animedown $(PREFIX)/bin
	chmod 755 $(PREFIX)/bin/animedown

uninstall:
	rm -f $(PREFIX)/bin/animedown
