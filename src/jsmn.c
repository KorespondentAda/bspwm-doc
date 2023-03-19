/** \file
 * JSON parsing
 * \todo Describe params
 * \todo Describe funcs
 */
#include "jsmn.h"

/** Allocates a fresh unused token from the token pool
 *
 * \param parser Used JSON parser
 * \param tokens Token pool
 * \param num_tokens Token pool size
 * \return Token, allocated from pool.
 *
 * Get token from pool specified by \p parser->toknext;
 * Init it start and end to `-1`, size to `0` and possible parent to `-1`.
 */
static jsmntok_t *jsmn_alloc_token(jsmn_parser *parser, jsmntok_t *tokens, size_t num_tokens)
{
	jsmntok_t *tok;
	if (parser->toknext >= num_tokens) {
		return NULL;
	}
	tok = &tokens[parser->toknext++];
	tok->start = tok->end = -1;
	tok->size = 0;
#ifdef JSMN_PARENT_LINKS
	tok->parent = -1;
#endif
	return tok;
}

/** Fills token type and boundaries
 *
 * \param token Token to init
 * \param type Token type
 * \param start Start position in data string
 * \param end End position in data string
 *
 * Init token by passed values.
 * \note Init size by 0
 */
static void jsmn_fill_token(jsmntok_t *token, jsmntype_t type, int start, int end)
{
	token->type = type;
	token->start = start;
	token->end = end;
	token->size = 0;
}

/** Fills next available token with JSON primitive.
 *
 * \param parser Used JSON parser
 * \param js JSON data string
 * \param len Data string length
 * \param tokens Token pool
 * \param num_tokens Token pool size
 * \return #jsmnerr error code, or 0 if no error occured
 *
 * \todo Describe
 * \see JSMN_STRICT JSMN_PARENT_LINKS
 */
static int jsmn_parse_primitive(jsmn_parser *parser, const char *js, size_t len, jsmntok_t *tokens, size_t num_tokens)
{
	/** Here is the function workflow: */
	jsmntok_t *token;
	int start;

	/** Searching primitive from current #parser position */
	start = parser->pos;

	for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
		switch (js[parser->pos]) {
#ifndef JSMN_STRICT
			/** \warning In strict mode primitive must be
			 * followed by "," or "}" or "]", not ":"
			 */
			case ':':
#endif
			case '\t' : case '\r' : case '\n' : case ' ' :
			case ','  : case ']'  : case '}' :
				goto found;
		}
		/** \error If JSON string contains non-printable (<32 || >=127) character:
		 * reset #parser position to start and
		 * return #JSMN_ERROR_INVAL error value
		 */
		if (js[parser->pos] < 32 || js[parser->pos] >= 127) {
			parser->pos = start;
			return JSMN_ERROR_INVAL;
		}
	}
#ifdef JSMN_STRICT
	/** \note In strict mode primitive must be followed by a comma/object/array:
	 * reset #parser position to start and
	 * return #JSMN_ERROR_INVAL error value
	 */
	parser->pos = start;
	return JSMN_ERROR_PART;
#endif

found:
	if (tokens == NULL) {
		parser->pos--;
		return 0;
	}
	token = jsmn_alloc_token(parser, tokens, num_tokens);
	if (token == NULL) {
		parser->pos = start;
		return JSMN_ERROR_NOMEM;
	}
	jsmn_fill_token(token, JSMN_PRIMITIVE, start, parser->pos);
#ifdef JSMN_PARENT_LINKS
	token->parent = parser->toksuper;
#endif
	parser->pos--;
	return 0;
}

/** Fills next token with JSON string.
 *
 * \todo Describe
 */
static int jsmn_parse_string(jsmn_parser *parser, const char *js, size_t len, jsmntok_t *tokens, size_t num_tokens)
{
	jsmntok_t *token;

	int start = parser->pos;

	parser->pos++;

	/* Skip starting quote */
	for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
		char c = js[parser->pos];

		/* Quote: end of string */
		if (c == '\"') {
			if (tokens == NULL) {
				return 0;
			}
			token = jsmn_alloc_token(parser, tokens, num_tokens);
			if (token == NULL) {
				parser->pos = start;
				return JSMN_ERROR_NOMEM;
			}
			jsmn_fill_token(token, JSMN_STRING, start+1, parser->pos);
#ifdef JSMN_PARENT_LINKS
			token->parent = parser->toksuper;
#endif
			return 0;
		}

		/* Backslash: Quoted symbol expected */
		if (c == '\\' && parser->pos + 1 < len) {
			int i;
			parser->pos++;
			switch (js[parser->pos]) {
				/* Allowed escaped symbols */
				case '\"': case '/' : case '\\' : case 'b' :
				case 'f' : case 'r' : case 'n'  : case 't' :
					break;
				/* Allows escaped symbol \uXXXX */
				case 'u':
					parser->pos++;
					for(i = 0; i < 4 && parser->pos < len && js[parser->pos] != '\0'; i++) {
						/* If it isn't a hex character we have an error */
						if(!((js[parser->pos] >= 48 && js[parser->pos] <= 57) || /* 0-9 */
									(js[parser->pos] >= 65 && js[parser->pos] <= 70) || /* A-F */
									(js[parser->pos] >= 97 && js[parser->pos] <= 102))) { /* a-f */
							parser->pos = start;
							return JSMN_ERROR_INVAL;
						}
						parser->pos++;
					}
					parser->pos--;
					break;
				/* Unexpected symbol */
				default:
					parser->pos = start;
					return JSMN_ERROR_INVAL;
			}
		}
	}
	parser->pos = start;
	return JSMN_ERROR_PART;
}

/** Run JSON parser.
 *
 * \param parser Used JSON parser
 * \param js JSON data string
 * \param len Data string length
 * \param tokens Token pool
 * \param num_tokens Token pool size
 * \return Count of found tokens, or error code, if any.
 *
 * Parse JSON string and fill tokens.
 * It parses a JSON data string into and array of tokens, each describing
 * a single JSON object.
 */
int jsmn_parse(jsmn_parser *parser, const char *js, size_t len, jsmntok_t *tokens, unsigned int num_tokens)
{
	/** Here is the function workflow: */
	int r;
	int i;
	jsmntok_t *token;
	int count = parser->toknext;

	/** Look JSON string symbol-by-symbol for: */
	for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
		char c;
		jsmntype_t type;

		c = js[parser->pos];
		/** \todo Too big `switch`? */
		switch (c) {
			/** - Open braces ('{', '['): */
			case '{': case '[':
				/** Start block, increase tokens count; */
				count++;
				if (tokens == NULL) {
					break;
				}
				/** Get new token; */
				token = jsmn_alloc_token(parser, tokens, num_tokens);
				/** \error if no token:
				 * return #JSMN_ERROR_NOMEM
				 */
				if (token == NULL)
					return JSMN_ERROR_NOMEM;
				/** Set token values:
				 * size to 1,
				 * parent to current supertoken,
				 * type to #JSMN_OBJECT or #JSMN_ARRAY if it is '{' or '[',
				 * start to current position,
				 * current toksuper to this token index;
				 */
				if (parser->toksuper != -1) {
					tokens[parser->toksuper].size++;
#ifdef JSMN_PARENT_LINKS
					token->parent = parser->toksuper;
#endif
				}
				token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
				token->start = parser->pos;
				parser->toksuper = parser->toknext - 1;
				break;
			/** - Close braces ('{', '['): */
			case '}': case ']':
				/** End block; */
				if (tokens == NULL)
					break;
				type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
#ifdef JSMN_PARENT_LINKS
				/** \error if this no tokens allocated (unmatched bracket):
				 * return #JSMN_ERROR_INVAL
				 */
				if (parser->toknext < 1) {
					return JSMN_ERROR_INVAL;
				}
				/** Use last allocated token as current; */
				token = &tokens[parser->toknext - 1];
				/** Search for first non-ended parent: */
				for (;;) {
					if (token->start != -1 && token->end == -1) {
						/** \error if bracket type not correspond token type:
						 * return #JSMN_ERROR_INVAL
						 */
						if (token->type != type) {
							return JSMN_ERROR_INVAL;
						}
						/** If found and corresponding type:
						 * Set position as it end and
						 * current toksuper to it parent
						 */
						token->end = parser->pos + 1;
						parser->toksuper = token->parent;
						break;
					}
					/** Stop if no parent; */
					if (token->parent == -1) {
						/** \error if wrong type or no supertoken present */
						if(token->type != type || parser->toksuper == -1) {
							return JSMN_ERROR_INVAL;
						}
						break;
					}
					/** Go to parent. */
					token = &tokens[token->parent];
				}
#else
				/** Search previous tokens for non-ended parent token */
				for (i = parser->toknext - 1; i >= 0; i--) {
					token = &tokens[i];
					if (token->start != -1 && token->end == -1) {
						/** \error if types is not match:
						 * return #JSMN_ERROR_INVAL
						 */
						if (token->type != type) {
							return JSMN_ERROR_INVAL;
						}
						/** If found, reset supertoken and set found token end */
						parser->toksuper = -1;
						token->end = parser->pos + 1;
						break;
					}
				}
				/** \error if unmatched closing bracket:
				 * return #JSMN_ERROR_INVAL
				 */
				if (i == -1) return JSMN_ERROR_INVAL;
				/** Then see for last non-ended token to set as supertoken */
				for (; i >= 0; i--) {
					token = &tokens[i];
					if (token->start != -1 && token->end == -1) {
						parser->toksuper = i;
						break;
					}
				}
#endif
				break;
			/** - Quotations ('"'): */
			case '\"':
				/** Parse as string */
				r = jsmn_parse_string(parser, js, len, tokens, num_tokens);
				/** \error if jsmn_parse_string() produce error:
				 * return given error code
				 */
				if (r < 0) return r;
				count++;
				/** If have supertoken — increase it size. */
				if (parser->toksuper != -1 && tokens != NULL)
					tokens[parser->toksuper].size++;
				break;
			/** - Space/line/tab delimeters ('\\t', '\\r', '\\n', ' '): */
			case '\t' : case '\r' : case '\n' : case ' ':
				/** Just ignore it. */
				break;
			/** - Colons (':'): */
			case ':':
				/** Supertoken sets to previous token. */
				parser->toksuper = parser->toknext - 1;
				break;
			/** - Period (','): */
			case ',':
				/** Ignore if there's no supertoken or if
				 * it's #JSMN_ARRAY or #JSMN_OBJECT;
				 */
				if (tokens != NULL && parser->toksuper != -1 &&
						tokens[parser->toksuper].type != JSMN_ARRAY &&
						tokens[parser->toksuper].type != JSMN_OBJECT) {
#ifdef JSMN_PARENT_LINKS
					/** New supertoken is its parent. */
					parser->toksuper = tokens[parser->toksuper].parent;
#else
					/** Search tokens backwards for #JSMN_ARRAY or #JSMN_OBJECT
					 * that's non-ended, set it as new supertoken.
					 */
					for (i = parser->toknext - 1; i >= 0; i--) {
						if (tokens[i].type == JSMN_ARRAY || tokens[i].type == JSMN_OBJECT) {
							if (tokens[i].start != -1 && tokens[i].end == -1) {
								parser->toksuper = i;
								break;
							}
						}
					}
#endif
				}
				break;
			/** - Primitives: */
#ifdef JSMN_STRICT
			/** \note In strict mode primitives are: numbers and booleans;
			 * And they must not be keys of the object
			 */
			case '-': case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
			case 't': case 'f': case 'n':
				/** \error if there is supertoken and it's
				 * #JSMN_OBJECT or non-empty #JSMN_STRING:
				 * return #JSMN_ERROR_INVAL
				 */
				if (tokens != NULL && parser->toksuper != -1) {
					jsmntok_t *t = &tokens[parser->toksuper];
					if (t->type == JSMN_OBJECT ||
							(t->type == JSMN_STRING && t->size != 0)) {
						return JSMN_ERROR_INVAL;
					}
				}
#else
			/** \note In non-strict mode every unquoted value is a primitive */
			default:
#endif
				/** Parse found primitive */
				r = jsmn_parse_primitive(parser, js, len, tokens, num_tokens);
				/** \error if jsmn_parse_primitive() returned error:
				 * return given error code
				 */
				if (r < 0) return r;
				/** Increase found token count and,
				 * if present, increase supertoken size
				 */
				count++;
				if (parser->toksuper != -1 && tokens != NULL)
					tokens[parser->toksuper].size++;
				break;

#ifdef JSMN_STRICT
			/** \error Unexpected char in strict mode:
			 * return #JSMN_ERROR_INVAL
			 */
			default:
				return JSMN_ERROR_INVAL;
#endif
		}
	}

	if (tokens != NULL) {
		for (i = parser->toknext - 1; i >= 0; i--) {
			/** \error if unmatched opened object or array found:
			 * return #JSMN_ERROR_PART
			 */
			if (tokens[i].start != -1 && tokens[i].end == -1) {
				return JSMN_ERROR_PART;
			}
		}
	}

	return count;
}

/** Create JSON parser over an array of tokens
 *
 * \param parser JSON parser to init
 *
 * Creates a new parser based over a given buffer with an array of tokens
 * available.
 * \note Init parser pos with `0`, toknext with `0`, toksuper with `-1`
 * \warning \p parser object must be created first
 */
void jsmn_init(jsmn_parser *parser)
{
	parser->pos = 0;
	parser->toknext = 0;
	parser->toksuper = -1;
}

