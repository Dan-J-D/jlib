#include "jlib.h"

#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef JLIB_PRINT
#include <stdio.h>
#endif

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

#define jlib_encoded_str_(s) #s
#define jlib_encoded_str(s) jlib_encoded_str_(s)

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
		char buf[sizeof(jlib_encoded_str(INT32_MAX)) + 1];
		if (jlib_itoa(val->i32, buf, sizeof(jlib_encoded_str(INT32_MAX)), 10) == 0)
			len += strlen(buf);
		else
			len += sizeof(jlib_encoded_str(INT32_MAX));
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
		len += strlen(val->str) + 2;
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
		strcpy(buf, "undefined");
		buf += 9;
		buf_len -= 9;
		break;
	case JLIB_TYPE_NULL:
		if (buf_len < 4)
			return NULL;
		strcpy(buf, "null");
		buf += 4;
		buf_len -= 4;
		break;
	case JLIB_TYPE_TRUE:
		if (buf_len < 4)
			return NULL;
		strcpy(buf, "true");
		buf += 4;
		buf_len -= 4;
		break;
	case JLIB_TYPE_FALSE:
		if (buf_len < 5)
			return NULL;
		strcpy(buf, "false");
		buf += 5;
		buf_len -= 5;
		break;
	case JLIB_TYPE_INT:
	{
		char buf2[sizeof(jlib_encoded_str(INT32_MAX)) + 1];
		if (jlib_itoa(val->i32, buf2, sizeof(jlib_encoded_str(INT32_MAX)), 10) == 0)
		{
			if (buf_len < strlen(buf2))
				return NULL;
			strcpy(buf, buf2);
			buf += strlen(buf2);
			buf_len -= strlen(buf2);
		}
		else
		{
			if (buf_len < sizeof(jlib_encoded_str(0)))
				return NULL;
			strcpy(buf, jlib_encoded_str(0));
			buf += sizeof(jlib_encoded_str(0));
			buf_len -= sizeof(jlib_encoded_str(0));
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
			strcpy(buf, "0.00000000");
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
		strcpy(buf, val->str);
		buf += strlen(val->str);
		*buf++ = '"';
		buf_len -= strlen(val->str) + 2;
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
		if (buf_len < 9 || strncmp(buf, "undefined", 9) != 0)
			return NULL;
		val->type = JLIB_TYPE_UNDEFINED;
		buf += 9;
		buf_len -= 9;
		break;
	case 'n':
		if (buf_len < 4 || strncmp(buf, "null", 4) != 0)
			return NULL;
		val->type = JLIB_TYPE_NULL;
		buf += 4;
		buf_len -= 4;
		break;
	case 't':
		if (buf_len < 4 || strncmp(buf, "true", 4) != 0)
			return NULL;
		val->type = JLIB_TYPE_TRUE;
		buf += 4;
		buf_len -= 4;
		break;
	case 'f':
		if (buf_len < 5 || strncmp(buf, "false", 5) != 0)
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
		val->arr = (jlib_val *)malloc(sizeof(jlib_val) * (len + 2));
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
		val->obj = (jlib_obj *)malloc(sizeof(jlib_obj) * (len + 2));
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
		char *buf3 = (char *)malloc(buf2 - buf + 1);
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
		printf("undefined");
		break;
	case JLIB_TYPE_NULL:
		printf("null");
		break;
	case JLIB_TYPE_TRUE:
		printf("true");
		break;
	case JLIB_TYPE_FALSE:
		printf("false");
		break;
	case JLIB_TYPE_INT:
		printf("%d", val->i32);
		break;
	case JLIB_TYPE_FLOAT:
		printf("%f", val->f32);
		break;
	case JLIB_TYPE_STRING:
		printf("\"%s\"", val->str);
		break;
	case JLIB_TYPE_ARRAY:
	{
		printf("[");
		for (unsigned int i = 0; val->arr[i].type != JLIB_TYPE_END; i++)
		{
			if (i != 0)
				printf(",");
			jlib_print_val_(&val->arr[i]);
		}
		printf("]");
		break;
	}
	case JLIB_TYPE_OBJECT:
	{
		printf("{");
		for (unsigned int i = 0; val->obj[i].name.type != JLIB_TYPE_END; i++)
		{
			if (i != 0)
				printf(",");
			jlib_print_val_(&val->obj[i].name);
			printf(":");
			jlib_print_val_(&val->obj[i].val);
		}
		printf("}");
		break;
	}
	}
}

void jlib_print_val(jlib_val *val)
{
	jlib_print_val_(val);
	printf("\n");
}
#endif