VERSION = 0.1

PREFIX=$(HOME)/.local

CPPFLAGS = -DVERSION=\"${VERSION}\" -D_POSIX_C_SOURCE=200809L
CFLAGS = -ggdb -std=c99 -pedantic -Wall -Wextra -Os $(CPPFLAGS)
CFLAGS += -Wno-unused-function
LIBS = -lcurl

CC = cc

