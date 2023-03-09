PROGRAM = animedown
VERSION = 0.1

PREFIX=${HOME}/.local

CFLAGS = -std=c99 -pedantic -Wall -Wextra -Wno-deprecated-declarations -Os -DVERSION=\"${VERSION}\" -D_POSIX_C_SOURCE=200809L

CC = cc

