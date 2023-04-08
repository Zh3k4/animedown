#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"

static char *
get_cache_dir()
{
	if (getenv("XDG_CACHE_HOME")) {
		return getenv("XDG_CACHE_HOME");
	} else {
		const char *home = getenv("HOME");
		size_t cachesz = 128;
		char *cache = calloc(sizeof(char), cachesz);

		while (strlen(home) > cachesz - 7) {
			char * newcache = realloc(cache, cachesz * 2);
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

static bool
pmkdir(const char *const pathname)
{
	if (mkdir(pathname, 0755) < 0 && errno != EEXIST) {
		return false;
	}
	return true;
}

int
main(void)
{
	return EXIT_SUCCESS;
}

