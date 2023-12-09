#ifndef JLIB_H_
#define JLIB_H_

// Config
// Global Preprocessor JLIB_PRINT to be able to print jlib_val with jlib_print_val()
#define JLIB_STR(string) string
// End Config

#define JLIB_TYPE_UNDEFINED 0
#define JLIB_TYPE_NULL 1
#define JLIB_TYPE_TRUE 2
#define JLIB_TYPE_FALSE 3
#define JLIB_TYPE_INT 4
#define JLIB_TYPE_FLOAT 5
#define JLIB_TYPE_STRING 6
#define JLIB_TYPE_ARRAY 7
#define JLIB_TYPE_OBJECT 8
#define JLIB_TYPE_END 9

#define jlib_undefined ((jlib_val){.type = JLIB_TYPE_UNDEFINED, .need_free = 0})
#define jlib_null ((jlib_val){.type = JLIB_TYPE_NULL, .need_free = 0})
#define jlib_false ((jlib_val){.type = JLIB_TYPE_FALSE, .need_free = 0})
#define jlib_true ((jlib_val){.type = JLIB_TYPE_TRUE, .need_free = 0})
#define jlib_int(int) ((jlib_val){.type = JLIB_TYPE_INT, .i32 = int, .need_free = 0})
#define jlib_float(float) ((jlib_val){.type = JLIB_TYPE_FLOAT, .f32 = float, .need_free = 0})
#define jlib_str(string) ((jlib_val){.type = JLIB_TYPE_STRING, .str = JLIB_STR(string), .need_free = 0})
#define jlib_arr(...) ((jlib_val){.type = JLIB_TYPE_ARRAY, .arr = (jlib_val[]){__VA_ARGS__, {.type = JLIB_TYPE_END}}, .need_free = 0})
#define jlib_obj(...) ((jlib_val){.type = JLIB_TYPE_OBJECT, .obj = (jlib_obj[]){__VA_ARGS__, {.name = {.type = JLIB_TYPE_END}}}, .need_free = 0})

typedef struct jlib_val
{
	unsigned char type : 7; // JLIBN_TYPE_*
	unsigned char need_free : 1;
	union
	{
		int i32;
		float f32;
		char *str;
		struct jlib_val *arr;
		struct jlib_obj *obj;
	};
} jlib_val;

typedef struct jlib_obj
{
	jlib_val name;
	jlib_val val;
} jlib_obj;

void jlib_new_val(jlib_val *val);
void jlib_delete_val(jlib_val *val);

void jlib_set_undefined(jlib_val *val);
void jlib_set_null(jlib_val *val);
void jlib_set_true(jlib_val *val);
void jlib_set_false(jlib_val *val);
void jlib_set_int(jlib_val *val, int i32);
void jlib_set_float(jlib_val *val, float f32);
void jlib_set_str(jlib_val *val, char *str);
void jlib_set_arr(jlib_val *val, jlib_val arr);
void jlib_set_pbj(jlib_val *val, jlib_obj *obj);
void jlib_set_val(jlib_val *val, jlib_val val2);

void jlib_obj_set(jlib_val *obj, char *name, jlib_val val);

// not including null terminator
unsigned int jlib_encode_len(jlib_val *val);
// returns where buffer ends
char *jlib_encode(jlib_val *val, char *buf, unsigned int buf_len);

// return where buffer ends (0 if error)
char *jlib_decode(jlib_val *val, char *buf, unsigned int buf_len);

#ifdef JLIB_PRINT
void jlib_print_val(jlib_val *val);
#endif

#endif // JLIB_H_
