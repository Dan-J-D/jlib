#include "jlib.h"

#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef JLIB_PRINT
#include <stdio.h>
#endif

#define JLIB_TO_STR_(s) #s
#define JLIB_TO_STR(s) JLIB_TO_STR_(s)

void jlib_strrev(unsigned char *str)
{
	int i;
	int j;
	unsigned char a;
	unsigned len = strlen((const char *)str);
	for (i = 0, j = len - 1; i < j; i++, j--)
	{
		a = str[i];
		str[i] = str[j];
		str[j] = a;
	}
}

int jlib_itoa(int num, unsigned char *str, int len, int base)
{
	int sum = num;
	int i = 0;
	int digit;
	if (len == 0)
		return -1;
	do
	{
		digit = sum % base;
		if (digit < 0xA)
			str[i++] = '0' + digit;
		else
			str[i++] = 'A' + digit - 0xA;
		sum /= base;
	} while (sum && (i < (len - 1)));
	if (i == (len - 1) && sum)
		return -1;
	str[i] = '\0';
	jlib_strrev(str);
	return 0;
}

void jlib_new_val(jlib_val *val)
{
	val->need_free = 0;
	val->type = JLIB_TYPE_UNDEFINED;
}

void jlib_delete_val(jlib_val *val)
{
	switch (val->type)
	{
	case JLIB_TYPE_STRING:
		if (val->need_free)
			free(val->str);
		break;
	case JLIB_TYPE_ARRAY:
		for (unsigned int i = 0; val->arr[i].type != JLIB_TYPE_END; i++)
			jlib_delete_val(&val->arr[i]);
		if (val->need_free)
			free(val->arr);
		break;
	case JLIB_TYPE_OBJECT:
		for (unsigned int i = 0; val->obj[i].name.type != JLIB_TYPE_END; i++)
		{
			jlib_delete_val(&val->obj[i].name);
			jlib_delete_val(&val->obj[i].val);
		}
		if (val->need_free)
			free(val->obj);
		break;
	}

	val->type = JLIB_TYPE_UNDEFINED;
}

void jlib_set_undefined(jlib_val *val)
{
	jlib_delete_val(val);
	val->type = JLIB_TYPE_UNDEFINED;
	val->need_free = 0;
}

void jlib_set_null(jlib_val *val)
{
	jlib_delete_val(val);
	val->type = JLIB_TYPE_NULL;
	val->need_free = 0;
}

void jlib_set_true(jlib_val *val)
{
	jlib_delete_val(val);
	val->type = JLIB_TYPE_TRUE;
	val->need_free = 0;
}

void jlib_set_false(jlib_val *val)
{
	jlib_delete_val(val);
	val->type = JLIB_TYPE_FALSE;
	val->need_free = 0;
}

void jlib_set_int(jlib_val *val, int i32)
{
	jlib_delete_val(val);
	val->type = JLIB_TYPE_INT;
	val->i32 = i32;
	val->need_free = 0;
}

void jlib_set_float(jlib_val *val, float f32)
{
	jlib_delete_val(val);
	val->type = JLIB_TYPE_FLOAT;
	val->f32 = f32;
	val->need_free = 0;
}

void jlib_set_str(jlib_val *val, char *str)
{
	jlib_delete_val(val);
	val->type = JLIB_TYPE_STRING;
	val->str = (char *)malloc(strlen(str) + 1);
	strcpy(val->str, str);
	val->need_free = 1;
}

void jlib_set_arr(jlib_val *val, jlib_val arr)
{
	jlib_delete_val(val);
	val->type = JLIB_TYPE_ARRAY;

	unsigned int len = 0;
	for (unsigned int i = 0; arr.arr[i].type != JLIB_TYPE_END; i++)
		len++;

	val->arr = (jlib_val *)malloc(sizeof(jlib_val) * (len + 1));
	for (unsigned int i = 0; i < len + 1; i++)
	{
		switch (arr.arr[i].type)
		{
		case JLIB_TYPE_UNDEFINED:
			jlib_set_undefined(&val->arr[i]);
			break;
		case JLIB_TYPE_NULL:
			jlib_set_null(&val->arr[i]);
			break;
		case JLIB_TYPE_TRUE:
			jlib_set_true(&val->arr[i]);
			break;
		case JLIB_TYPE_FALSE:
			jlib_set_false(&val->arr[i]);
			break;
		case JLIB_TYPE_INT:
			jlib_set_int(&val->arr[i], arr.arr[i].i32);
			break;
		case JLIB_TYPE_FLOAT:
			jlib_set_float(&val->arr[i], arr.arr[i].f32);
			break;
		case JLIB_TYPE_STRING:
			jlib_set_str(&val->arr[i], arr.arr[i].str);
			break;
		case JLIB_TYPE_ARRAY:
			jlib_set_arr(&val->arr[i], arr.arr[i]);
			break;
		case JLIB_TYPE_OBJECT:
			jlib_set_pbj(&val->arr[i], arr.arr[i].obj);
			break;
		case JLIB_TYPE_END:
			val->arr[i].type = JLIB_TYPE_END;
			break;
		}
	}
	val->need_free = 1;
}

void jlib_set_pbj(jlib_val *val, jlib_obj *obj)
{
	jlib_delete_val(val);
	val->type = JLIB_TYPE_OBJECT;
	val->obj = obj;
	val->need_free = 0;
}

void jlib_set_val(jlib_val *val, jlib_val val2)
{
	switch (val2.type)
	{
	case JLIB_TYPE_UNDEFINED:
		jlib_set_undefined(val);
		break;
	case JLIB_TYPE_NULL:
		jlib_set_null(val);
		break;
	case JLIB_TYPE_TRUE:
		jlib_set_true(val);
		break;
	case JLIB_TYPE_FALSE:
		jlib_set_false(val);
		break;
	case JLIB_TYPE_INT:
		jlib_set_int(val, val2.i32);
		break;
	case JLIB_TYPE_FLOAT:
		jlib_set_float(val, val2.f32);
		break;
	case JLIB_TYPE_STRING:
		jlib_set_str(val, val2.str);
		break;
	case JLIB_TYPE_ARRAY:
		jlib_set_arr(val, val2);
		break;
	case JLIB_TYPE_OBJECT:
		jlib_set_pbj(val, val2.obj);
		break;
	}
}

void jlib_obj_set(jlib_val *obj, char *name, jlib_val val)
{
	unsigned int found = UINT32_MAX;
	unsigned int len = 0;
	for (unsigned int i = 0; obj->obj[i].name.type != JLIB_TYPE_END; i++)
	{
		if (obj->obj[i].name.type == JLIB_TYPE_STRING)
		{
			if (strcmp(obj->obj[i].name.str, name) == 0)
			{
				found = i;
				break;
			}
		}
		len++;
	}

	if (found != UINT32_MAX)
	{
		jlib_delete_val(&obj->obj[found].val);
		jlib_set_val(&obj->obj[found].val, val);
		return;
	}

	jlib_obj *obj2 = (jlib_obj *)malloc(sizeof(jlib_obj) * (len + 2));

	for (unsigned int i = 0; i < len + 1; i++)
		obj2[i] = obj->obj[i];

	jlib_set_str(&obj2[len].name, name);
	jlib_set_val(&obj2[len].val, val);

	obj2[len + 1].name.type = JLIB_TYPE_END;
	obj2[len + 1].val.type = JLIB_TYPE_END;

	if (obj->need_free)
		free(obj->obj);

	obj->obj = obj2;
	obj->need_free = 1;
}

unsigned int jlib_escape_len(char ch)
{
	switch (ch)
	{
	case '\b':
	case '\f':
	case '\n':
	case '\r':
	case '\t':
	case '\\':
	case '"':
		return 2;
	default:
		return 1;
	}
}

unsigned int jlib_escape_str_len(char *str)
{
	unsigned int len = 0;
	for (unsigned int i = 0; str[i] != '\0'; i++)
		len += jlib_escape_len(str[i]);
	return len;
}

char *jlib_escape_str(char *str, char *buf, unsigned int buf_len)
{
	for (unsigned int i = 0; str[i] != '\0' && buf_len >= 0; i++)
	{
		switch (str[i])
		{
		case '\b':
			if (buf_len < 2)
				return NULL;
			*buf++ = '\\';
			*buf++ = 'b';
			buf_len -= 2;
			break;
		case '\f':
			if (buf_len < 2)
				return NULL;
			*buf++ = '\\';
			*buf++ = 'f';
			buf_len -= 2;
			break;
		case '\n':
			if (buf_len < 2)
				return NULL;
			*buf++ = '\\';
			*buf++ = 'n';
			buf_len -= 2;
			break;
		case '\r':
			if (buf_len < 2)
				return NULL;
			*buf++ = '\\';
			*buf++ = 'r';
			buf_len -= 2;
			break;
		case '\t':
			if (buf_len < 2)
				return NULL;
			*buf++ = '\\';
			*buf++ = 't';
			buf_len -= 2;
			break;
		case '\\':
			if (buf_len < 2)
				return NULL;
			*buf++ = '\\';
			*buf++ = '\\';
			buf_len -= 2;
			break;
		case '"':
			if (buf_len < 2)
				return NULL;
			*buf++ = '\\';
			*buf++ = '"';
			buf_len -= 2;
			break;
		default:
			if (buf_len < 1)
				return NULL;
			*buf++ = str[i];
			buf_len--;
			break;
		}
	}
	return buf;
}

unsigned int jlib_encode_len(jlib_val *val)
{
	unsigned int len = 0;
	switch (val->type)
	{
	case JLIB_TYPE_UNDEFINED:
		len += 9;
		break;
	case JLIB_TYPE_NULL:
		len += 4;
		break;
	case JLIB_TYPE_TRUE:
		len += 4;
		break;
	case JLIB_TYPE_FALSE:
		len += 5;
		break;
	case JLIB_TYPE_INT:
	{
		char buf[sizeof(JLIB_TO_STR(INT32_MAX)) + 1];
		if (jlib_itoa(val->i32, buf, sizeof(JLIB_TO_STR(INT32_MAX)), 10) == 0)
			len += strlen(buf);
		else
			len += sizeof(JLIB_TO_STR(INT32_MAX));
		break;
	}
	case JLIB_TYPE_FLOAT:
	{
		char buf[10 + 1];
		if (gcvt((double)val->f32, 10, buf) == buf)
			len += strlen(buf);
		else
			len += 10;
		break;
	}
	case JLIB_TYPE_STRING:
		len += jlib_escape_str_len(val->str) + 2; // '"' + '"'
		break;
	case JLIB_TYPE_ARRAY:
		len++; // '['
		for (unsigned int i = 0; val->arr[i].type != JLIB_TYPE_END; i++)
			len += jlib_encode_len(&val->arr[i]) + 1; // ',' or ']'
		break;
	case JLIB_TYPE_OBJECT:
		len++; // '{'
		for (unsigned int i = 0; val->obj[i].name.type != JLIB_TYPE_END; i++)
		{
			len += jlib_encode_len(&val->obj[i].name) + 1; // ':'
			len += jlib_encode_len(&val->obj[i].val) + 1;	 // ',' or '}'
		}
		break;
	}

	return len;
}

char *jlib_encode(jlib_val *val, char *buf, unsigned int buf_len)
{
	switch (val->type)
	{
	case JLIB_TYPE_UNDEFINED:
		if (buf_len < 9)
			return NULL;
		strcpy(buf, JLIB_STR("undefined"));
		buf += 9;
		buf_len -= 9;
		break;
	case JLIB_TYPE_NULL:
		if (buf_len < 4)
			return NULL;
		strcpy(buf, JLIB_STR("null"));
		buf += 4;
		buf_len -= 4;
		break;
	case JLIB_TYPE_TRUE:
		if (buf_len < 4)
			return NULL;
		strcpy(buf, JLIB_STR("true"));
		buf += 4;
		buf_len -= 4;
		break;
	case JLIB_TYPE_FALSE:
		if (buf_len < 5)
			return NULL;
		strcpy(buf, JLIB_STR("false"));
		buf += 5;
		buf_len -= 5;
		break;
	case JLIB_TYPE_INT:
	{
		char buf2[sizeof(JLIB_TO_STR(INT32_MAX)) + 1];
		if (jlib_itoa(val->i32, buf2, sizeof(JLIB_TO_STR(INT32_MAX)), 10) == 0)
		{
			if (buf_len < strlen(buf2))
				return NULL;
			strcpy(buf, buf2);
			buf += strlen(buf2);
			buf_len -= strlen(buf2);
		}
		else
		{
			if (buf_len < sizeof(JLIB_TO_STR(0)))
				return NULL;
			strcpy(buf, JLIB_TO_STR(0));
			buf += sizeof(JLIB_TO_STR(0));
			buf_len -= sizeof(JLIB_TO_STR(0));
		}
		break;
	}
	case JLIB_TYPE_FLOAT:
	{
		char buf2[10 + 1];
		if (gcvt((double)val->f32, 10, buf2))
		{
			if (buf_len < strlen(buf2))
				return NULL;
			strcpy(buf, buf2);
			buf += strlen(buf2);
			buf_len -= strlen(buf2);
		}
		else
		{
			if (buf_len < 10)
				return NULL;
			strcpy(buf, JLIB_STR("0.00000000"));
			buf += 10;
			buf_len -= 10;
		}
		break;
	}
	case JLIB_TYPE_STRING:
	{
		if (buf_len < strlen(val->str) + 2)
			return NULL;
		*buf++ = '"';
		buf = jlib_escape_str(val->str, buf, buf_len);
		*buf++ = '"';
		buf_len -= jlib_escape_str_len(val->str) + 2;
		break;
	}
	case JLIB_TYPE_ARRAY:
	{
		if (buf_len < 1)
			return NULL;
		*buf++ = '[';
		buf_len--;
		for (unsigned int i = 0; val->arr[i].type != JLIB_TYPE_END; i++)
		{
			if (i != 0)
			{
				if (buf_len < 1)
					return NULL;
				*buf++ = ',';
				buf_len--;
			}
			if (buf_len < jlib_encode_len(&val->arr[i]))
				return NULL;
			buf = jlib_encode(&val->arr[i], buf, buf_len);
			buf_len -= jlib_encode_len(&val->arr[i]);
		}
		if (buf_len < 1)
			return NULL;
		*buf++ = ']';
		buf_len--;
		break;
	}
	case JLIB_TYPE_OBJECT:
	{
		if (buf_len < 1)
			return NULL;
		*buf++ = '{';
		buf_len--;
		for (unsigned int i = 0; val->obj[i].name.type != JLIB_TYPE_END; i++)
		{
			if (i != 0)
			{
				if (buf_len < 1)
					return NULL;
				*buf++ = ',';
				buf_len--;
			}
			if (val->obj[i].name.type == JLIB_TYPE_STRING)
			{
				if (buf_len < strlen(val->obj[i].name.str) + 2)
					return NULL;
				*buf++ = '"';
				strcpy(buf, val->obj[i].name.str);
				buf += strlen(val->obj[i].name.str);
				*buf++ = '"';
				buf_len -= strlen(val->obj[i].name.str) + 2;
			}
			else
			{
				if (buf_len < jlib_encode_len(&val->obj[i].name))
					return NULL;
				buf = jlib_encode(&val->obj[i].name, buf, buf_len);
				buf_len -= jlib_encode_len(&val->obj[i].name);
			}
			if (buf_len < 1)
				return NULL;
			*buf++ = ':';
			buf_len--;
			if (buf_len < jlib_encode_len(&val->obj[i].val))
				return NULL;
			buf = jlib_encode(&val->obj[i].val, buf, buf_len);
			buf_len -= jlib_encode_len(&val->obj[i].val);
		}
		if (buf_len < 1)
			return NULL;
		*buf++ = '}';
		buf_len--;
		break;
	}
	}

	return buf;
}

char *jlib_decode(jlib_val *val, char *buf, unsigned int buf_len)
{
	if (!val || !buf || buf_len == 0)
		return NULL;

	jlib_delete_val(val);

	switch (*buf)
	{
	case 'u':
		if (buf_len < 9 || strncmp(buf, JLIB_STR("undefined"), 9) != 0)
			return NULL;
		val->type = JLIB_TYPE_UNDEFINED;
		buf += 9;
		buf_len -= 9;
		break;
	case 'n':
		if (buf_len < 4 || strncmp(buf, JLIB_STR("null"), 4) != 0)
			return NULL;
		val->type = JLIB_TYPE_NULL;
		buf += 4;
		buf_len -= 4;
		break;
	case 't':
		if (buf_len < 4 || strncmp(buf, JLIB_STR("true"), 4) != 0)
			return NULL;
		val->type = JLIB_TYPE_TRUE;
		buf += 4;
		buf_len -= 4;
		break;
	case 'f':
		if (buf_len < 5 || strncmp(buf, JLIB_STR("false"), 5) != 0)
			return NULL;
		val->type = JLIB_TYPE_FALSE;
		buf += 5;
		buf_len -= 5;
		break;
	case '"':
	{
		char *buf2 = buf + 1;
		while (*buf2 != '"')
		{
			if (*buf2 == '\0')
				return NULL;
			buf2++;
		}
		val->type = JLIB_TYPE_STRING;
		val->str = (char *)malloc(buf2 - buf);
		strncpy(val->str, buf + 1, buf2 - buf - 1);
		val->str[buf2 - buf - 1] = '\0';
		val->need_free = 1;
		buf = buf2 + 1;
		buf_len -= buf2 - buf + 1;
		break;
	}
	case '[':
	{
		val->type = JLIB_TYPE_ARRAY;
		unsigned int len = 0;
		char *buf2 = buf + 1;
		while (*buf2 != ']')
		{
			if (*buf2 == '\0')
				return NULL;
			if (*buf2 == ',')
				len++;
			buf2++;
		}
		val->arr = (jlib_val *)malloc(sizeof(jlib_val) * (len + 1));
		val->need_free = 1;
		len = 0;
		buf2 = buf + 1;
		while (*buf2 != ']')
		{
			if (*buf2 == '\0')
				return NULL;
			if (*buf2 == ',')
			{
				buf2++;
				continue;
			}
			buf2 = jlib_decode(&val->arr[len], buf2, buf_len - (buf2 - buf));
			if (!buf2)
				return NULL;
			len++;
		}
		val->arr[len].type = JLIB_TYPE_END;
		buf = buf2 + 1;
		buf_len -= buf2 - buf + 1;
		break;
	}
	case '{':
	{
		val->type = JLIB_TYPE_OBJECT;
		unsigned int len = 0;
		char *buf2 = buf + 1;
		while (*buf2 != '}')
		{
			if (*buf2 == '\0')
				return NULL;
			if (*buf2 == ',')
				len++;
			buf2++;
		}
		val->obj = (jlib_obj *)malloc(sizeof(jlib_obj) * (len + 1));
		val->need_free = 1;
		len = 0;
		buf2 = buf + 1;
		while (*buf2 != '}')
		{
			if (*buf2 == '\0')
				return NULL;
			if (*buf2 == ',')
			{
				buf2++;
				continue;
			}
			buf2 = jlib_decode(&val->obj[len].name, buf2, buf_len - (buf2 - buf));
			if (!buf2)
				return NULL;
			if (*buf2 != ':')
				return NULL;
			buf2++;
			buf2 = jlib_decode(&val->obj[len].val, buf2, buf_len - (buf2 - buf));
			if (!buf2)
				return NULL;
			len++;
		}
		val->obj[len].name.type = JLIB_TYPE_END;
		buf = buf2 + 1;
		buf_len -= buf2 - buf + 1;
		break;
	}
	default:
	{
		char *buf2 = buf;
		while (*buf2 != '\0' && *buf2 != ',' && *buf2 != '}' && *buf2 != ']')
			buf2++;

		char *buf3 = buf;
		while (*buf3 == '.' || (*buf3 >= '0' && *buf3 <= '9'))
			buf3++;

		if (buf3 != buf2)
			return NULL;

		buf3 = (char *)malloc(buf2 - buf + 1);
		strncpy(buf3, buf, buf2 - buf);
		buf3[buf2 - buf] = '\0';
		if (strchr(buf3, '.') != NULL)
		{
			val->type = JLIB_TYPE_FLOAT;
			val->f32 = atof(buf3);
		}
		else if (atoi(buf3) != 0)
		{
			val->type = JLIB_TYPE_INT;
			val->i32 = atoi(buf3);
		}
		free(buf3);
		buf = buf2;
		buf_len -= buf2 - buf;
		break;
	}
	}

	return buf;
}

#ifdef JLIB_PRINT
void jlib_print_val_(jlib_val *val)
{
	switch (val->type)
	{
	case JLIB_TYPE_UNDEFINED:
		printf(JLIB_STR("undefined"));
		break;
	case JLIB_TYPE_NULL:
		printf(JLIB_STR("null"));
		break;
	case JLIB_TYPE_TRUE:
		printf(JLIB_STR("true"));
		break;
	case JLIB_TYPE_FALSE:
		printf(JLIB_STR("false"));
		break;
	case JLIB_TYPE_INT:
		printf(JLIB_STR("%d"), val->i32);
		break;
	case JLIB_TYPE_FLOAT:
		printf(JLIB_STR("%f"), val->f32);
		break;
	case JLIB_TYPE_STRING:
		unsigned int len = jlib_escape_str_len(val->str);
		if (len != strlen(val->str))
		{
			char *buf = (char *)malloc(len + 1);
			buf[len] = '\0';
			if (jlib_escape_str(val->str, buf, len))
				printf(JLIB_STR("\"%s\""), buf);
			free(buf);
		}
		else
			printf(JLIB_STR("\"%s\""), val->str);
		break;
	case JLIB_TYPE_ARRAY:
	{
		printf(JLIB_STR("["));
		for (unsigned int i = 0; val->arr[i].type != JLIB_TYPE_END; i++)
		{
			if (i != 0)
				printf(JLIB_STR(","));
			jlib_print_val_(&val->arr[i]);
		}
		printf(JLIB_STR("]"));
		break;
	}
	case JLIB_TYPE_OBJECT:
	{
		printf(JLIB_STR("{"));
		for (unsigned int i = 0; val->obj[i].name.type != JLIB_TYPE_END; i++)
		{
			if (i != 0)
				printf(JLIB_STR(","));
			jlib_print_val_(&val->obj[i].name);
			printf(JLIB_STR(":"));
			jlib_print_val_(&val->obj[i].val);
		}
		printf(JLIB_STR("}"));
		break;
	}
	}
}

void jlib_print_val(jlib_val *val)
{
	jlib_print_val_(val);
	printf(JLIB_STR("\n"));
}
#endif
