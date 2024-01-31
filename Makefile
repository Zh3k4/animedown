include config.mk

SRC = animedown.c
OBJ = $(SRC:.c=.o)

animedown: $(OBJ)
	@printf 'LD\t%s\n' $@
	@$(CC) -o $@ $^ $(LIBS)

.c.o:
	@printf 'CC\t%s\n' $@
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ): config.h config.mk

clean:
	rm -f animedown $(OBJ) animedown-$(VERSION).tar.gz

dist: clean
	mkdir -p animedown-$(VERSION)
	cp\
		Makefile config.mk LICENSE README\
		config.def.h $(SRC)\
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
