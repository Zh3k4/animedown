VERSION = 0.1

PREFIX=$(HOME)/.local

CFLAGS = -ggdb -std=c99 -pedantic -Wall -Wextra -Os
CFLAGS += -DVERSION=\"${VERSION}\" -D_POSIX_C_SOURCE=200809L
CFLAGS += -Wno-unused-function
LIBS = -lcurl

CC = cc

