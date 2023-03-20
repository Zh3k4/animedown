include config.mk

SRC = ${PROGRAM}.c
OBJ = ${SRC:%.c=obj/%.o}

all: options ${PROGRAM}

run: options ${PROGRAM}
	./${PROGRAM}

options:
	@echo ${PROGRAM} build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

tags: ${SRC}
	${CC} -M ${SRC} |\
		sed -e 's,\\,,' -e 's, ,\n,' |\
		sed -e '/^$$/d' -e '/\.o:[ \t]*$$/d' |\
		ctags --languages=c,c++ --kinds-c=+p -R -L -

obj/%.o: %.c
	${CC} -c $< -o $@ ${CFLAGS}

${OBJ}: config.h config.mk

config.h:
	cp config.def.h config.h

${PROGRAM}: ${OBJ}
	${CC} -o $@ $^ ${LDFLAGS}

clean:
	rm -f ${OBJ} ${PROGRAM} ${PROGRAM}-*.tar.gz

dist: clean
	mkdir -p ${PROGRAM}-${VERSION}
	cp -R Makefile README.md CHANGELOG.md config.def.h config.mk\
		${SRC} ${PROGRAM}-${VERSION} || true
	tar -cf ${PROGRAM}-${VERSION}.tar ${PROGRAM}-${VERSION}
	gzip ${PROGRAM}-${VERSION}.tar
	rm -rf ${PROGRAM}-${VERSION}

install:
	mkdir -p ${PREFIX}/bin
	cp -f $(PROGRAM) ${PREFIX}/bin
	chmod 755 ${PREFIX}/bin/${PROGRAM}

uninstall:
	rm -f ${PREFIX}/bin/${PROGRAM}

.PHONY: all options clean dist install uninstall
