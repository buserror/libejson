/*
	This file is part of libejson.
	
	(C) 2010 Michel Pollet <buserror@gmail.com>
 */

#ifndef __EJSON_H__
#define __EJSON_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
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
 * Returned by the parser if the stack is a fixed stack, and we
 * run out of space (the JSON depth is too deep
 */
enum {
	EJSON_ERR_NOSTACK = -1969,
	EJSON_ERR_BASE64,
	EJSON_ERR_VALUE,
	EJSON_ERR_VALUE_LIST,
	EJSON_ERR_VALUE_OBJECT,
	EJSON_ERR,
};

/*!
 * JSON value. works in pair with the ejson_driver_type_XX enum,
 * passed as argument to set_name() and set_value().
 *
 * Note that since the library tries really hard not to allocate
 * memory, the v_str for the string value if defined by start and
 * end pointer *straight* into the source JSON, no duplication is made.
 */
typedef struct ejson_driver_value_t {
	union {
		long long v_int;
		double v_float;
		int	v_bool;
		struct { const char * start, * end; } v_str;
	} u;
} ejson_driver_value_t;

/*!
 * JSON decoder driver structure. You need to declare one of these
 * statically and pass it to the decoder/parser. these callbacks
 * allows the user of the library to allocate and populate its own
 * data format to map it to the JSON structure
 */
typedef struct ejson_driver_t {
	void * refcon; // yours to use, not used by the parser
	
	/*
	 * called optionally before the value is set. This is called
	 * for objects that have a name, ie { "a" : 4 }.
	 * this is /not/ called for objects in an array
	 */
	void (*set_name)(struct ejson_driver_t *d, 
		ejson_driver_value_t * v);

	/*
	 * Called when a JSON array is opened
	 */
	void (*open_array)(struct ejson_driver_t *d);
	/*
	 * Called when a JSON object is opened
	 */
	void (*open_object)(struct ejson_driver_t *d);

	/*
	 * Allocate and set the value for a node.
	 * set_name() will already have been called for a named node
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

	/*
	 * Some platforms do not want to use malloc/realloc/free, give them
	 * a way to allocate give the parser a stack without hard coding the
	 * limits in the library.
	 * 'size' is in sizeof(int), not in bytes.
	 * Note: realloc_stack will be called *at least* once. dealloc_stack
	 *       will be called only once at the end of parsing.
	 */
	int * (*realloc_stack)(struct ejson_driver_t *d, int * stack, int size);
	int * (*dealloc_stack)(struct ejson_driver_t *d, int * stack);

	void (*error)(void *refcon, int err, const char * where);
} ejson_driver_t;

/*!
 * parses 'str', calls the callbacks from 'd' driver instance
 *
 * The parsing stack is dynamically allocated, so will allow parsing
 * any depth of JSON objects.
 */
int
ejson_parse(
		ejson_driver_t *d,
		const char *str);

/*!
 * This will parse 'str', calls the driver 'd'.
 *
 * if 'stack' is NULL, the stack will be allocated on the fly
 */
int
ejson_parse_full(
		ejson_driver_t *d,
		const char *str,
		int * stack,
		int stack_size);

/*!
 * parses a string containing \b\t\n'r etc, also converts \uXXXX to UTF8
 * returns the length of the destination string.
 *
 * Note that the destination string /can/ be the source string, as it will
 * always be shorter than the source string.
 *
 * if src == dst on entry, the source string will be clobbered, this
 * Implicitly means that the 'const' on the 'str' parameter is for
 * convenience only.
 */
int
ejson_parse_string(
		const char *str,
		const char *end,
		char * out);

#ifdef __cplusplus
};
#endif

#endif /* __EJSON_H__ */
