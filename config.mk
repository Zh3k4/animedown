PROGRAM = animedown
VERSION = 0.1

PREFIX=${HOME}/.local

CPPFLAGS = -DVERSION=\"${VERSION}\" -D_POSIX_C_SOURCE=200809L
CFLAGS = -std=c99 -pedantic -Wall -Wextra -Wno-deprecated-declarations -Os $(CPPFLAGS)

CC = cc

