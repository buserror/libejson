/*
	This file is part of libejson.
	
	(C) 2010 Michel Pollet <buserror@gmail.com>
	
	libejson is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	libejson is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public License
	along with libejson.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ejson.h"

/*
	Demoes an "empty" driver that just prints the JSON as it
	comes in...
	
	run :
	./ejson_test -i test.json
 */
static void d_set_name(struct ejson_driver_t *d, 
		ejson_driver_value_t * v)
{
	int l = v->u.v_str.end - v->u.v_str.start;
	printf("\"%*.*s\": ", l, l, v->u.v_str.start);
}

static void d_open_array(struct ejson_driver_t *d)
{
	printf("[");fflush(stdout);
}

static void d_open_object(struct ejson_driver_t *d)
{
	printf("{");fflush(stdout);
}

static void d_set_value(struct ejson_driver_t *d, 
		int type, 
		ejson_driver_value_t * v)
{
	switch (type) {
		case ejson_driver_type_null:
			printf("null, ");
			break;
		case ejson_driver_type_bool:
			printf("%s, ", v->u.v_bool ? "true" : "false");
			break;
		case ejson_driver_type_integer:
			printf("%d, ", (int)v->u.v_int);
			break;
		case ejson_driver_type_hex:
			printf("0x%x, ", (int)v->u.v_int);
			break;
		case ejson_driver_type_float:
			printf("%f, ", (float)v->u.v_float);
			break;
		/*
		   uses the string parser to get the "real" string out
		 */
		case ejson_driver_type_string: {
			char buf[256];
			ejson_parse_string(v->u.v_str.start, v->u.v_str.end, buf);
			printf("\"%s\", ", buf);
		}	break;
	}
	fflush(stdout);
}

static void d_close_array(struct ejson_driver_t *d)
{
	printf("],");fflush(stdout);
}

static void d_close_object(struct ejson_driver_t *d)
{
	printf("},");fflush(stdout);
}

static void d_open_data(struct ejson_driver_t *d)
{
	printf("%% '");fflush(stdout);	
}
static void d_add_data(struct ejson_driver_t *d, uint8_t * data, size_t size)
{
	printf("(");
	for (int i = 0; i < size; i++)
		printf("%c", data[i]);
	printf(")");
	fflush(stdout);
}
static void d_close_data(struct ejson_driver_t *d)
{
	printf("' %%,");fflush(stdout);
}

ejson_driver_t driver = {
	.set_name = d_set_name,
	.open_array = d_open_array,
	.open_object = d_open_object,
	.set_value = d_set_value,
	.close_array = d_close_array,
	.close_object = d_close_object,
	
	.open_data = d_open_data,
	.add_data = d_add_data,
	.close_data = d_close_data,
};

int main(int argc, char * argv[])
{

	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-i")) {
			char * load = malloc(32*1024);
			FILE * f = fopen(argv[++i], "r");
			if (f) {
				size_t r = fread(load, 1, 32*1024, f);
				load[r] = 0;
				fclose(f);
				printf("### parsing %s, %d bytes\n", argv[i], (int)r);
				ejson_parse(&driver, load);
				printf("\n");
			}
		} else {
			printf("### parsing '%s'\n", argv[i]);
			ejson_parse(&driver, argv[i]);printf("\n");
		}
	}
	return 0;
}

