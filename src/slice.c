#include "slice.h"

#include <string.h>

int
slice_list_add(String *list, size_t *size, String str)
{
	String *newlist = (String *)realloc(list, (*size + 1) * sizeof(String));
	if (newlist == NULL) {
		return 0;
	}
	list = newlist;
	*size += 1;

	list[*size - 1] = str;

	return 1;
}

int
eq_to_cstr(String a, char *b)
{
	size_t bl = strlen(b);

	if (a.length != bl) {
		return 0;
	}

	for (size_t i = 0; i < bl; ++i) {
		if (a.data[i] != b[i]) {
			return 0;
		}
	}

	return 1;
}

