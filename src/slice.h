#ifndef SLICE_H_
#define SLICE_H_

#include <stdlib.h>

typedef struct {
	char *data;
	size_t length;
} String;

int slice_arr_add(String *arr, String str);
int eq_to_cstr(String a, char *b);

#endif /* SLICE_H_ */
