#include <stdio.h>
#include <malloc.h>

#include "jlib.h"

/*
This is Test 3 to check for memory leaks.
*/

int main(int argc, char **argv)
{
	jlib_val val = jlib_obj({jlib_str("\""), jlib_str("tes\"t1243\'3\t")});

	jlib_print_val(&val);
	jlib_obj_set(&val, "test123", jlib_str("t33"));
	jlib_obj_set(&val, "test123324", jlib_str("t33"));
	jlib_obj_set(&val, "test1233241", jlib_str("t3354"));
	jlib_print_val(&val);

	jlib_delete_val(&val);
}
