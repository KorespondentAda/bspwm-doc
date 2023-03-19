/** \file
 * JSMN library
 * \todo What about [licence](https://github.com/zserge/jsmn/blob/master/jsmn.h)?
 * \todo There is no need for `__cplusplus` check if written in C and has code
 * at all?
 */
#ifndef __JSMN_H_
#define __JSMN_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** JSON type identifier */
typedef enum {
	JSMN_UNDEFINED = 0,     /**< Undefined type */
	JSMN_OBJECT = 1,        /**< JSON object */
	JSMN_ARRAY = 2,         /**< Array */
	JSMN_STRING = 3,        /**< String */
	JSMN_PRIMITIVE = 4      /**< Primitives (number, boolean, null) */
} jsmntype_t;

/** JSMN Error values */
enum jsmnerr {
	JSMN_ERROR_NOMEM = -1,  /**< Not enough tokens were provided */
	JSMN_ERROR_INVAL = -2,  /**< Invalid character inside JSON string */
	JSMN_ERROR_PART = -3    /**< The string is not a full JSON packet,
	                         * more bytes expected
	                         */
};

/** JSON token description. */
typedef struct {
	jsmntype_t type;        /**< Type (object, array, string etc.) */
	int start;              /**< Start position in JSON data string */
	int end;                /**< End position in JSON data string */
	int size;               /**< \todo Describe */
#ifdef JSMN_PARENT_LINKS
	int parent;             /**< \todo Describe */
#endif
} jsmntok_t;

/** JSON parser.
 *
 * Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string
 */
typedef struct {
	unsigned int pos;       /**< Offset in the JSON string */
	unsigned int toknext;   /**< Next token index to allocate */
	int toksuper;           /**< Superior token node index,
	                         * e.g parent object or array
	                         */
} jsmn_parser;

void jsmn_init(jsmn_parser *parser);
int jsmn_parse(jsmn_parser *parser, const char *js, size_t len, jsmntok_t *tokens, unsigned int num_tokens);

#ifdef __cplusplus
}
#endif

#endif /* __JSMN_H_ */
