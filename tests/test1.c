#include <stdio.h>
#include <malloc.h>

#include "jlib.h"

/*
This is Test 1 to check for memory leaks.
*/

int main(int argc, char **argv)
{
	while(1)
	{
		jlib_val val = jlib_obj(
			{ jlib_str("test123"), jlib_str("test12433") },
			{ jlib_str("test"), jlib_int(23) },
			{ jlib_str("test1"), jlib_float(1.523f) },
			{ jlib_str("test2\b"), jlib_null },
			{ jlib_str("test3\t"), jlib_false },
			{ jlib_str("test4\""), jlib_true },
			{ jlib_str("test5"), jlib_undefined },
			{ jlib_str("test6"), jlib_arr(jlib_int(25), jlib_int(26)) },
			{ jlib_str("test5"), jlib_obj({jlib_str("test"), jlib_int(23)}) });

		printf("jlib_print_val(): ");
		jlib_print_val(&val);

		unsigned int encoded_len = jlib_encode_len(&val);
		printf("encoded_len(): %d\n", encoded_len);

		char *buf = (char *)malloc(encoded_len + 1);
		memset(buf, 0, encoded_len + 1);
		jlib_encode(&val, buf, encoded_len + 1);
		printf("jlib_encode(): %s\n", buf);

		jlib_val val2;
		char *b = jlib_decode(&val2, buf, encoded_len);
		printf("jlib_decode(): ");
		jlib_print_val(&val2);
		free(buf);

		jlib_delete_val(&val2);
		jlib_delete_val(&val);
	}
}
