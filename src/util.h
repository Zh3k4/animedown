#ifndef UTIL_H_
#define UTIL_H_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct {
	char *memory;
	size_t size;
} MemoryRegion;

size_t write_callback(void *content, size_t size, size_t nmemb, void *userp);
static char *get_cache_dir(void);
static int pmkdir(const char *const pathname);

#endif /* UTIL_H_ */
