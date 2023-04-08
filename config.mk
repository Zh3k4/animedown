VERSION = 0.1

PREFIX=$(HOME)/.local

CPPFLAGS = -DVERSION=\"${VERSION}\" -D_POSIX_C_SOURCE=200809L
CFLAGS = -std=c99 -pedantic -Wall -Wextra -Os $(CPPFLAGS)

CC = cc

