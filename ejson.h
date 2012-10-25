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

#ifndef __EJSON_H__
#define __EJSON_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * These are passed as argument to ejson_driver_t set_value()
 */
enum {
	ejson_driver_type_null = 0,
	ejson_driver_type_bool			= (1 << 0),
	ejson_driver_type_integer		= (1 << 1),
	ejson_driver_type_hex			= (1 << 2),
	ejson_driver_type_float			= (1 << 3),
	ejson_driver_type_string		= (1 << 4),
};

/*
 * JSON value, works in pair with the enum, passed as argument
 */
typedef struct ejson_driver_value_t {
	union {
		long long v_int;
		double v_float;
		int	v_bool;
		struct { const char * start, * end; } v_str;
	} u;
} ejson_driver_value_t;

/*
 * JSON decoder driver structure. You need to declare one of these
 * staticaly and pass it to the decoder/parser. these callbacks
 * allows the user to allocate and populate its own data format
 * to map it to the JSON structure
 */
typedef struct ejson_driver_t {
	void * refcon; // yours to use, not used by the parser
	
	/*
	 * called optionaly before the value is set. This is called
	 * for objects that have a name, ie { "a" : 4 }.
	 * this is /not/ called for objects in an array
	 */
	void (*set_name)(struct ejson_driver_t *d, 
		ejson_driver_value_t * v);

	/*
	 * Called when an array is opened
	 */
	void (*open_array)(struct ejson_driver_t *d);
	/*
	 * Called when an object is opened
	 */
	void (*open_object)(struct ejson_driver_t *d);

	/*
	 * Allocate and set the value for a node.
	 * set_name() already had been called for a named node
	 */
	void (*set_value)(struct ejson_driver_t *d, 
		int type, 
		ejson_driver_value_t * v);
	
	/*
	 * Close and array after the last node was added,
	 * continues parsing the parent structure.
	 */
	void (*close_array)(struct ejson_driver_t *d);
	/*
	 * Closes an object and continues parsing
	 */
	void (*close_object)(struct ejson_driver_t *d);

	/* 
	 * JSON extension ; allow specifying node 'flags' as
	 *
	 * { "lala"(dirty,visible) : "My String" }
	 * Any word can be passed between the parenthesis, this is made
	 * to allow "node attributes".
	 * the string is delimited by v->u.v_str.start <-> v->u.v_str.end
	 */
	void (*add_flag)(struct ejson_driver_t *d, 
		ejson_driver_value_t * v);

	/* 
	 * JSON extension ; allow specifying random binary data as value
	 *
	 * The parser understands base64 encoded values:
	 * { "node" : % ....base64 data .... % }
	 * The data is passed to add_data() as it is decoded
	 */
	void (*open_data)(struct ejson_driver_t *d);
	void (*add_data)(struct ejson_driver_t *d, uint8_t * data, size_t size);
	void (*close_data)(struct ejson_driver_t *d);	
} ejson_driver_t;

/*
 * parses 'str', calls the callbacks from 'd' driver instance
 */
int ejson_parse( ejson_driver_t *d, const char * str );

/*
 * parses a string containing \b\t\n'r etc, also converts \uXXXX to UTF8
 * returns the length of the destination string, which /might/.
 * Note that the destination string /can/ be the source string, as it will
 * always be shorter than the source string.
 * if src == dst on entry, the source string will be clobbered.
 */
int ejson_parse_string(const char * str, const char *end, char * out);

#ifdef __cplusplus
};
#endif

#endif /* __EJSON_H__ */
