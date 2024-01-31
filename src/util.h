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
static char *get_cache_dir();
static int pmkdir(const char *const pathname);

#endif /* UTIL_H_ */
#ifdef UTIL_IMPLEMENTATION

size_t
write_callback(void *content, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	MemoryRegion *m = (MemoryRegion *)userp;

	char *p = (char *)realloc(m->memory, m->size + realsize + 1);
	if (p == NULL) {
		fprintf(stderr, "Error: no memory\n");
		return 1;
	}

	m->memory = p;
	memcpy(&(m->memory[m->size]), content, realsize);
	m->size += realsize;
	m->memory[m->size] = 0;

	return realsize;
}

static char *
get_cache_dir()
{
	if (getenv("XDG_CACHE_HOME")) {
		return getenv("XDG_CACHE_HOME");
	} else {
		const char *home = getenv("HOME");
		size_t cachesz = 128;
		char *cache = (char *)calloc(sizeof(char), cachesz);

		while (strlen(home) > cachesz - 7) {
			char *newcache = (char *)realloc(cache, cachesz * 2);
			if (newcache) {
				cache = newcache;
				cachesz *= 2;
			} else {
				free(cache);
				return NULL;
			}
		}

		strcat(cache, home);

		return strcat(cache, "/.cache");
	}
}

static int
pmkdir(const char *const pathname)
{
	if (mkdir(pathname, 0755) < 0 && errno != EEXIST) {
		return 0;
	}
	return 1;
}

#endif /* UTIL_IMPLEMENTATION */
