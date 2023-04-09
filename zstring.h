#ifndef ZSTRING_H_
#define ZSTRING_H_

#include <string.h>

typedef struct {
	char *data;
	size_t length;
} String;

int zstring_arr_add(String *arr, String str);
int eq_to_cstr(String a, char *b);

#endif /* ZSTRING_H_ */
#ifdef ZSTRING_IMPLEMENTATION

int
zstring_list_add(String *list, size_t *size, String str)
{
	String *newlist = realloc(list, (*size + 1) * sizeof(String));
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

#endif /* ZSTRING_IMPLEMENTATION  */

