#include <stdio.h>
#include <malloc.h>

#include "jlib.h"

/*
This is Test 1 to check for memory leaks.
*/

int main(int argc, char **argv)
{
	jlib_val val = jlib_obj(
			{jlib_str("test123"), jlib_str("test12433")},
			{jlib_str("test"), jlib_int(23)},
			{jlib_str("test1"), jlib_float(1.523f)},
			{jlib_str("test2"), jlib_null},
			{jlib_str("test3"), jlib_false},
			{jlib_str("test4"), jlib_true},
			{jlib_str("test5"), jlib_undefined},
			{jlib_str("test6"), jlib_arr(jlib_int(25), jlib_int(26))},
			{jlib_str("test5"), jlib_obj({jlib_str("test"), jlib_int(23)})});

	jlib_print_val(&val);
	jlib_obj_set(&val, "test123", jlib_str("t33"));
	jlib_obj_set(&val, "test123324", jlib_str("t33"));
	jlib_obj_set(&val, "test1233241", jlib_str("t3354"));
	jlib_print_val(&val);

	printf("\nstrings:\n");
	for (unsigned int i = 0; val.obj[i].name.type != JLIB_TYPE_END; i++)
	{
		if (val.obj[i].name.type == JLIB_TYPE_STRING && val.obj[i].val.type == JLIB_TYPE_STRING)
			printf("%s: \"%s\"\n", val.obj[i].name.str, val.obj[i].val.str);
	}

	jlib_delete_val(&val);
}
