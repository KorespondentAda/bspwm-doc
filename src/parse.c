#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include "parse.h"

/** \file
 * String parsing tools
 */

/** Parse boolean value
 *
 * \param [in ] value String to parse
 * \param [out] b Parsed value
 * \return True if parsed sucessfully, false otherwise
 *
 * Parse string to boolean values:
 * - `true` if get "true" or "on";
 * - `false` if get "false" or "off".
 */
bool parse_bool(char *value, bool *b)
{
	if (streq("true", value) || streq("on", value)) {
		*b = true;
		return true;
	} else if (streq("false", value) || streq("off", value)) {
		*b = false;
		return true;
	}
	return false;
}

/** Parse split type
 *
 * \param [in ] s String to parse
 * \param [out] t Parsed value
 * \return True if parsed sucessfully, false otherwise
 *
 * Parse string to #split_type_t values:
 * - #TYPE_HORIZONTAL if get "horizontal";
 * - #TYPE_VERTICAL if get "vertical";
 */
bool parse_split_type(char *s, split_type_t *t)
{
	if (streq("horizontal", s)) {
		*t = TYPE_HORIZONTAL;
		return true;
	} else if (streq("vertical", s)) {
		*t = TYPE_VERTICAL;
		return true;
	}
	return false;
}

/** Parse split mode
 *
 * \param [in ] s String to parse
 * \param [out] m Parsed value
 * \return True if parsed sucessfully, false otherwise
 *
 * Parse string to #split_mode_t values:
 * - #MODE_AUTOMATIC if get "automatic";
 * - #MODE_MANUAL if get "vertical";
 * \todo Why "vertical"?
 */
bool parse_split_mode(char *s, split_mode_t *m)
{
	if (streq("automatic", s)) {
		*m = MODE_AUTOMATIC;
		return true;
	} else if (streq("vertical", s)) {
		*m = MODE_MANUAL;
		return true;
	}
	return false;
}

/** Parse node layout mode
 * \todo What is \ref layout_t precisely?
 *
 * \param [in ] s String to parse
 * \param [out] l Parsed value
 * \return True if parsed sucessfully, false otherwise
 *
 * Parse string to layout_t values:
 * - #LAYOUT_MONOCLE if get "monocle";
 * - #LAYOUT_TILED if get "tiled";
 */
bool parse_layout(char *s, layout_t *l)
{
	if (streq("monocle", s)) {
		*l = LAYOUT_MONOCLE;
		return true;
	} else if (streq("tiled", s)) {
		*l = LAYOUT_TILED;
		return true;
	}
	return false;
}

/** Parse client state
 *
 * \param [in ] s String to parse
 * \param [out] t Parsed value
 * \return True if parsed sucessfully, false otherwise
 *
 * Parse string to client_state_t values:
 * - #STATE_TILED if get "tiled";
 * - #STATE_PSEUDO_TILED if get "pseudo_tiled";
 * - #STATE_FLOATING if get "floating";
 * - #STATE_FULLSCREEN if get "fullscreen";
 */
bool parse_client_state(char *s, client_state_t *t)
{
	if (streq("tiled", s)) {
		*t = STATE_TILED;
		return true;
	} else if (streq("pseudo_tiled", s)) {
		*t = STATE_PSEUDO_TILED;
		return true;
	} else if (streq("floating", s)) {
		*t = STATE_FLOATING;
		return true;
	} else if (streq("fullscreen", s)) {
		*t = STATE_FULLSCREEN;
		return true;
	}
	return false;
}

/** Parse stack layer
 *
 * \param [in ] s String to parse
 * \param [out] l Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to stack_layer_t values:
 * - #LAYER_BELOW if get "below";
 * - #LAYER_NORMAL if get "normal";
 * - #LAYER_ABOVE if get "above";
 */
bool parse_stack_layer(char *s, stack_layer_t *l)
{
	if (streq("below", s)) {
		*l = LAYER_BELOW;
		return true;
	} else if (streq("normal", s)) {
		*l = LAYER_NORMAL;
		return true;
	} else if (streq("above", s)) {
		*l = LAYER_ABOVE;
		return true;
	}
	return false;
}

/** Parse direction
 * \param [in ] s String to parse
 * \param [out] d Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \note bspc(1) COMMON DEFINITIONS->DIR
 *
 * Parse string to stack_layer_t values:
 * - #DIR_NORTH if get "north";
 * - #DIR_WEST if get "west";
 * - #DIR_SOUTH if get "south";
 * - #DIR_EAST if get "east";
 */
bool parse_direction(char *s, direction_t *d)
{
	if (streq("north", s)) {
		*d = DIR_NORTH;
		return true;
	} else if (streq("west", s)) {
		*d = DIR_WEST;
		return true;
	} else if (streq("south", s)) {
		*d = DIR_SOUTH;
		return true;
	} else if (streq("east", s)) {
		*d = DIR_EAST;
		return true;
	}
	return false;
}

/** Parse cycle direction
 *
 * \param [in ] s String to parse
 * \param [out] d Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to cycle_dir_t values:
 * - #CYCLE_NEXT if get "next";
 * - #CYCLE_PREV if get "prev";
 */
bool parse_cycle_direction(char *s, cycle_dir_t *d)
{
	if (streq("next", s)) {
		*d = CYCLE_NEXT;
		return true;
	} else if (streq("prev", s)) {
		*d = CYCLE_PREV;
		return true;
	}
	return false;
}

/** Parse circulate direction
 *
 * \param [in ] s String to parse
 * \param [out] d Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to circulate_dir_t values:
 * - #CIRCULATE_FORWARD if get "forward";
 * - #CIRCULATE_BACKWARD if get "backward";
 */
bool parse_circulate_direction(char *s, circulate_dir_t *d)
{
	if (streq("forward", s)) {
		*d = CIRCULATE_FORWARD;
		return true;
	} else if (streq("backward", s)) {
		*d = CIRCULATE_BACKWARD;
		return true;
	}
	return false;
}

/** Parse history direction
 *
 * \param [in ] s String to parse
 * \param [out] d Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to history_dir_t values:
 * - #HISTORY_OLDER if get "older";
 * - #HISTORY_NEWER if get "newer";
 */
bool parse_history_direction(char *s, history_dir_t *d)
{
	if (streq("older", s)) {
		*d = HISTORY_OLDER;
		return true;
	} else if (streq("newer", s)) {
		*d = HISTORY_NEWER;
		return true;
	}
	return false;
}


/** Parse flip mode
 *
 * \param [in ] s String to parse
 * \param [out] f Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to flip_t values:
 * - #FLIP_HORIZONTAL if get "horizontal";
 * - #FLIP_VERTICAL if get "vertical";
 */
bool parse_flip(char *s, flip_t *f)
{
	if (streq("horizontal", s)) {
		*f = FLIP_HORIZONTAL;
		return true;
	} else if (streq("vertical", s)) {
		*f = FLIP_VERTICAL;
		return true;
	}
	return false;
}

/** Parse resize handle
 *
 * \param [in ] s String to parse
 * \param [out] h Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to resize_handle_t values:
 * - #HANDLE_LEFT if get "left";
 * - #HANDLE_TOP if get "top";
 * - #HANDLE_RIGHT if get "right";
 * - #HANDLE_BOTTOM if get "bottom";
 * - #HANDLE_TOP_LEFT if get "top_left";
 * - #HANDLE_TOP_RIGHT if get "top_right";
 * - #HANDLE_BOTTOM_RIGHT if get "bottom_right";
 * - #HANDLE_BOTTOM_LEFT if get "bottom_left";
 */
bool parse_resize_handle(char *s, resize_handle_t *h)
{
	if (streq("left", s)) {
		*h = HANDLE_LEFT;
		return true;
	} else if (streq("top", s)) {
		*h = HANDLE_TOP;
		return true;
	} else if (streq("right", s)) {
		*h = HANDLE_RIGHT;
		return true;
	} else if (streq("bottom", s)) {
		*h = HANDLE_BOTTOM;
		return true;
	} else if (streq("top_left", s)) {
		*h = HANDLE_TOP_LEFT;
		return true;
	} else if (streq("top_right", s)) {
		*h = HANDLE_TOP_RIGHT;
		return true;
	} else if (streq("bottom_right", s)) {
		*h = HANDLE_BOTTOM_RIGHT;
		return true;
	} else if (streq("bottom_left", s)) {
		*h = HANDLE_BOTTOM_LEFT;
		return true;
	}
	return false;
}

/** Parse XCB key modifier mask
 *
 * \param [in ] s String to parse
 * \param [out] m Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to xcb_mod_mask_t values:
 * - #XCB_MOD_MASK_SHIFT if get "shift";
 * - #XCB_MOD_MASK_CONTROL if get "control";
 * - #XCB_MOD_MASK_LOCK if get "lock";
 * - #XCB_MOD_MASK_1 if get "mod1";
 * - #XCB_MOD_MASK_2 if get "mod2";
 * - #XCB_MOD_MASK_3 if get "mod3";
 * - #XCB_MOD_MASK_4 if get "mod4";
 * - #XCB_MOD_MASK_5 if get "mod5";
 */
bool parse_modifier_mask(char *s, uint16_t *m)
{
	if (strcmp(s, "shift") == 0) {
		*m = XCB_MOD_MASK_SHIFT;
		return true;
	} else if (strcmp(s, "control") == 0) {
		*m = XCB_MOD_MASK_CONTROL;
		return true;
	} else if (strcmp(s, "lock") == 0) {
		*m = XCB_MOD_MASK_LOCK;
		return true;
	} else if (strcmp(s, "mod1") == 0) {
		*m = XCB_MOD_MASK_1;
		return true;
	} else if (strcmp(s, "mod2") == 0) {
		*m = XCB_MOD_MASK_2;
		return true;
	} else if (strcmp(s, "mod3") == 0) {
		*m = XCB_MOD_MASK_3;
		return true;
	} else if (strcmp(s, "mod4") == 0) {
		*m = XCB_MOD_MASK_4;
		return true;
	} else if (strcmp(s, "mod5") == 0) {
		*m = XCB_MOD_MASK_5;
		return true;
	}
	return false;
}

/** Parse XCB \todo Name?
 *
 * \param [in ] s String to parse
 * \param [out] b Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to xcb_button_index_t values:
 * - #XCB_BUTTON_INDEX_ANY if get "any";
 * - #XCB_BUTTON_INDEX_1 if get "button1";
 * - #XCB_BUTTON_INDEX_2 if get "button2";
 * - #XCB_BUTTON_INDEX_3 if get "button3";
 * - -1 if get "none";
 */
bool parse_button_index(char *s, int8_t *b)
{
	if (strcmp(s, "any") == 0) {
		*b = XCB_BUTTON_INDEX_ANY;
		return true;
	} else if (strcmp(s, "button1") == 0) {
		*b = XCB_BUTTON_INDEX_1;
		return true;
	} else if (strcmp(s, "button2") == 0) {
		*b = XCB_BUTTON_INDEX_2;
		return true;
	} else if (strcmp(s, "button3") == 0) {
		*b = XCB_BUTTON_INDEX_3;
		return true;
	} else if (strcmp(s, "none") == 0) {
		*b = -1;
		return true;
	}
	return false;
}

/** Parse pointer action
 *
 * \param [in ] s String to parse
 * \param [out] a Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to pointer_action_t values:
 * - #ACTION_MOVE if get "move";
 * - #ACTION_RESIZE_CORNER if get "resize_corner";
 * - #ACTION_RESIZE_SIDE if get "resize_side";
 * - #ACTION_FOCUS if get "focus";
 * - #ACTION_NONE if get "none";
 */
bool parse_pointer_action(char *s, pointer_action_t *a)
{
	if (streq("move", s)) {
		*a = ACTION_MOVE;
		return true;
	} else if (streq("resize_corner", s)) {
		*a = ACTION_RESIZE_CORNER;
		return true;
	} else if (streq("resize_side", s)) {
		*a = ACTION_RESIZE_SIDE;
		return true;
	} else if (streq("focus", s)) {
		*a = ACTION_FOCUS;
		return true;
	} else if (streq("none", s)) {
		*a = ACTION_NONE;
		return true;
	}
	return false;
}

/** Parse child polarity (side)
 *
 * \param [in ] s String to parse
 * \param [out] p Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to child_polarity_t values:
 * - #FIRST_CHILD if get "first_child";
 * - #SECOND_CHILD if get "second_child";
 */
bool parse_child_polarity(char *s, child_polarity_t *p)
{
	if (streq("first_child", s)) {
		*p = FIRST_CHILD;
		return true;
	} else if (streq("second_child", s)) {
		*p = SECOND_CHILD;
		return true;
	}
	return false;
}

/** Parse automatic split scheme
 *
 * \param [in ] s String to parse
 * \param [out] a Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to automatic_scheme_t values:
 * - #SCHEME_LONGEST_SIDE if get "longest_side";
 * - #SCHEME_ALTERNATE if get "alternate";
 * - #SCHEME_SPIRAL if get "spiral";
 */
bool parse_automatic_scheme(char *s, automatic_scheme_t *a)
{
	if (streq("longest_side", s)) {
		*a = SCHEME_LONGEST_SIDE;
		return true;
	} else if (streq("alternate", s)) {
		*a = SCHEME_ALTERNATE;
		return true;
	} else if (streq("spiral", s)) {
		*a = SCHEME_SPIRAL;
		return true;
	}
	return false;
}

/** Parse state transition
 *
 * \param [in ] s String to parse
 * \param [out] m Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to state_transition_t values:
 * - 0 if get "none";
 * - Both if get "all";
 * - #STATE_TRANSITION_ENTER if comma-separated list contains "enter";
 * - #STATE_TRANSITION_EXIT if comma-separated list contains "exit";
 * \todo Is it too complicated?
 */
bool parse_state_transition(char *s, state_transition_t *m)
{
	if (streq("none", s)) {
		*m = 0;
		return true;
	} else if (streq("all", s)) {
		*m = STATE_TRANSITION_ENTER | STATE_TRANSITION_EXIT;
		return true;
	} else {
		state_transition_t w = 0;
		char *x = copy_string(s, strlen(s));
		char *key = strtok(x, ",");
		while (key != NULL) {
			if (streq("enter", key)) {
				w |= STATE_TRANSITION_ENTER;
			} else if (streq("exit", key)) {
				w |= STATE_TRANSITION_EXIT;
			} else {
				free(x);
				return false;
			}
			key = strtok(NULL, ",");
		}
		free(x);
		if (w != 0) {
			*m = w;
			return true;
		} else {
			return false;
		}
	}
	return false;
}

/** Parse tightness \todo What is it?
 *
 * \param [in ] s String to parse
 * \param [out] t Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to tightness_t values:
 * - #TIGHTNESS_HIGH if get "high";
 * - #TIGHTNESS_LOW if get "low";
 */
bool parse_tightness(char *s, tightness_t *t)
{
	if (streq("high", s)) {
		*t = TIGHTNESS_HIGH;
		return true;
	} else if (streq("low", s)) {
		*t = TIGHTNESS_LOW;
		return true;
	}
	return false;
}

/** Parse degrees number
 *
 * \param [in ] s String to parse
 * \param [out] d Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to int values that is multiples of 90 in [0, 360).
 */
bool parse_degree(char *s, int *d)
{
	int i = atoi(s);
	while (i < 0)
		i += 360;
	while (i > 359)
		i -= 360;
	if ((i % 90) != 0) {
		return false;
	} else {
		*d = i;
		return true;
	}
}

/** Parse identifier
 *
 * \param [in ] s String to parse
 * \param [out] id Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to identification number value.
 */
bool parse_id(char *s, uint32_t *id)
{
	char *end;
	errno = 0;
	uint32_t v = strtol(s, &end, 0);
	if (errno != 0 || *end != '\0') {
		return false;
	}
	*id = v;
	return true;
}

/** Parse KEY=VALUE pair for boolean
 *
 * \param [in ] s String to parse
 * \param [out] key Parsed key
 * \param [out] value Parsed value
 * \param [out] state Current state action ???
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to key string and it boolean value.
 * If    VALUE provided, \p state sets to #ALTER_SET.
 * If no VALUE provided, \p state sets to #ALTER_TOGGLE, \p value not changed.
 */
bool parse_bool_declaration(char *s, char **key, bool *value, alter_state_t *state)
{
	*key = strtok(s, EQL_TOK);
	char *v = strtok(NULL, EQL_TOK);
	if (v == NULL) {
		*state = ALTER_TOGGLE;
		return true;
	} else {
		if (parse_bool(v, value)) {
			*state = ALTER_SET;
			return true;
		} else {
			return false;
		}
	}
	return false;
}

/** Parse node index
 *
 * \param [in ] s String to parse
 * \param [out] idx Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to index value — unsigned integer number — prefixed by '^' char.
 */
bool parse_index(char *s, uint16_t *idx)
{
	return (sscanf(s, "^%hu", idx) == 1);
}

/** Parse rectangle description
 *
 * \param [in ] s String to parse
 * \param [out] r Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to xcb_rectangle_t value inf form
 * "<width>x<height>+<x offset>+<y offset>".
 * \note \p r is unchanged if parsing failed.
 */
bool parse_rectangle(char *s, xcb_rectangle_t *r)
{
	uint16_t w, h;
	int16_t x, y;
	if (sscanf(s, "%hux%hu+%hi+%hi", &w, &h, &x, &y) != 4) {
		return false;
	}
	r->width = w;
	r->height = h;
	r->x = x;
	r->y = y;
	return true;
}

/** Parse subscriber mask
 *
 * \param [in ] s String to parse
 * \param [out] mask Parsed value
 * \return True if parsed sucessfully, false otherwise
 * \todo man bspc(1)
 *
 * Parse string to subscriber_mask_t values:
 * - #SBSC_MASK_ALL if get "all";
 * - #SBSC_MASK_NODE if get "node";
 * - #SBSC_MASK_DESKTOP if get "desktop";
 * - #SBSC_MASK_MONITOR if get "monitor";
 * - #SBSC_MASK_POINTER_ACTION if get "pointer_action";
 * - #SBSC_MASK_NODE_ADD if get "node_add";
 * - #SBSC_MASK_NODE_REMOVE if get "node_remove";
 * - #SBSC_MASK_NODE_SWAP if get "node_swap";
 * - #SBSC_MASK_NODE_TRANSFER if get "node_transfer";
 * - #SBSC_MASK_NODE_FOCUS if get "node_focus";
 * - #SBSC_MASK_NODE_PRESEL if get "node_presel";
 * - #SBSC_MASK_NODE_STACK if get "node_stack";
 * - #SBSC_MASK_NODE_ACTIVATE if get "node_activate";
 * - #SBSC_MASK_NODE_GEOMETRY if get "node_geometry";
 * - #SBSC_MASK_NODE_STATE if get "node_state";
 * - #SBSC_MASK_NODE_FLAG if get "node_flag";
 * - #SBSC_MASK_NODE_LAYER if get "node_layer";
 * - #SBSC_MASK_DESKTOP_ADD if get "desktop_add";
 * - #SBSC_MASK_DESKTOP_RENAME if get "desktop_rename";
 * - #SBSC_MASK_DESKTOP_REMOVE if get "desktop_remove";
 * - #SBSC_MASK_DESKTOP_SWAP if get "desktop_swap";
 * - #SBSC_MASK_DESKTOP_TRANSFER if get "desktop_transfer";
 * - #SBSC_MASK_DESKTOP_FOCUS if get "desktop_focus";
 * - #SBSC_MASK_DESKTOP_ACTIVATE if get "desktop_activate";
 * - #SBSC_MASK_DESKTOP_LAYOUT if get "desktop_layout";
 * - #SBSC_MASK_MONITOR_ADD if get "monitor_add";
 * - #SBSC_MASK_MONITOR_RENAME if get "monitor_rename";
 * - #SBSC_MASK_MONITOR_REMOVE if get "monitor_remove";
 * - #SBSC_MASK_MONITOR_SWAP if get "monitor_swap";
 * - #SBSC_MASK_MONITOR_FOCUS if get "monitor_focus";
 * - #SBSC_MASK_MONITOR_GEOMETRY if get "monitor_geometry";
 * - #SBSC_MASK_REPORT if get "report";
 */
bool parse_subscriber_mask(char *s, subscriber_mask_t *mask)
{
	if (streq("all", s)) {
		*mask = SBSC_MASK_ALL;
	} else if (streq("node", s)) {
		*mask = SBSC_MASK_NODE;
	} else if (streq("desktop", s)) {
		*mask = SBSC_MASK_DESKTOP;
	} else if (streq("monitor", s)) {
		*mask = SBSC_MASK_MONITOR;
	} else if (streq("pointer_action", s)) {
		*mask = SBSC_MASK_POINTER_ACTION;
	} else if (streq("node_add", s)) {
		*mask = SBSC_MASK_NODE_ADD;
	} else if (streq("node_remove", s)) {
		*mask = SBSC_MASK_NODE_REMOVE;
	} else if (streq("node_swap", s)) {
		*mask = SBSC_MASK_NODE_SWAP;
	} else if (streq("node_transfer", s)) {
		*mask = SBSC_MASK_NODE_TRANSFER;
	} else if (streq("node_focus", s)) {
		*mask = SBSC_MASK_NODE_FOCUS;
	} else if (streq("node_presel", s)) {
		*mask = SBSC_MASK_NODE_PRESEL;
	} else if (streq("node_stack", s)) {
		*mask = SBSC_MASK_NODE_STACK;
	} else if (streq("node_activate", s)) {
		*mask = SBSC_MASK_NODE_ACTIVATE;
	} else if (streq("node_geometry", s)) {
		*mask = SBSC_MASK_NODE_GEOMETRY;
	} else if (streq("node_state", s)) {
		*mask = SBSC_MASK_NODE_STATE;
	} else if (streq("node_flag", s)) {
		*mask = SBSC_MASK_NODE_FLAG;
	} else if (streq("node_layer", s)) {
		*mask = SBSC_MASK_NODE_LAYER;
	} else if (streq("desktop_add", s)) {
		*mask = SBSC_MASK_DESKTOP_ADD;
	} else if (streq("desktop_rename", s)) {
		*mask = SBSC_MASK_DESKTOP_RENAME;
	} else if (streq("desktop_remove", s)) {
		*mask = SBSC_MASK_DESKTOP_REMOVE;
	} else if (streq("desktop_swap", s)) {
		*mask = SBSC_MASK_DESKTOP_SWAP;
	} else if (streq("desktop_transfer", s)) {
		*mask = SBSC_MASK_DESKTOP_TRANSFER;
	} else if (streq("desktop_focus", s)) {
		*mask = SBSC_MASK_DESKTOP_FOCUS;
	} else if (streq("desktop_activate", s)) {
		*mask = SBSC_MASK_DESKTOP_ACTIVATE;
	} else if (streq("desktop_layout", s)) {
		*mask = SBSC_MASK_DESKTOP_LAYOUT;
	} else if (streq("monitor_add", s)) {
		*mask = SBSC_MASK_MONITOR_ADD;
	} else if (streq("monitor_rename", s)) {
		*mask = SBSC_MASK_MONITOR_RENAME;
	} else if (streq("monitor_remove", s)) {
		*mask = SBSC_MASK_MONITOR_REMOVE;
	} else if (streq("monitor_swap", s)) {
		*mask = SBSC_MASK_MONITOR_SWAP;
	} else if (streq("monitor_focus", s)) {
		*mask = SBSC_MASK_MONITOR_FOCUS;
	} else if (streq("monitor_geometry", s)) {
		*mask = SBSC_MASK_MONITOR_GEOMETRY;
	} else if (streq("report", s)) {
		*mask = SBSC_MASK_REPORT;
	} else {
		return false;
	}
	return true;
}


/** Get option modifier.
 * Helper macro for checking token to be option modifier
 */
#define GET_MOD(k) \
	} else if (streq(#k, tok)) { \
		sel->k = OPTION_TRUE; \
	} else if (streq("!" #k, tok)) { \
		sel->k = OPTION_FALSE;

/** Parse monitor modifiers from description selectors
 *
 * \param [in ] desc Monitor description selector string
 * \param [out] sel Monitor selector modifiers
 * \return True if parsed good, false otherwise
 * \note bspc(1) SELECTORS->Monitor->Modifiers
 *
 * Parse string to monitor modifiers:
 * - "focused" — Only consider the focused monitor
 * - "occupied" — Only consider monitors where the focused desktop is occupied
 */
bool parse_monitor_modifiers(char *desc, monitor_select_t *sel)
{
	char *tok;
	while ((tok = strrchr(desc, CAT_CHR)) != NULL) {
		tok[0] = '\0';
		tok++;
		if (streq("occupied", tok)) {
			sel->occupied = OPTION_TRUE;
		} else if (streq("!occupied", tok)) {
			sel->occupied = OPTION_FALSE;
		GET_MOD(focused)
		} else {
			return false;
		}
	}
	return true;
}

/** Parse desktop modifiers from description selectors
 *
 * \param [in ] desc Desktop description selector string
 * \param [out] sel Desktop selector modifiers
 * \return True if parsed good, false otherwise
 * \note bspc(1) SELECTORS->Desktop->Modifiers
 *
 * Parse string to desktop modifiers:
 * - "focused" — Only consider the focused desktop
 * - "occupied" — Only consider desktops where the focused desktop is occupied
 * - "active" — \todo Describe from README
 * - "urgent" — \todo Describe from README
 * - "local" — \todo Describe from README
 * - "tiled" — \todo Describe from README
 * - "monocle" — \todo Describe from README
 * - "user_tiled" — \todo Describe from README
 * - "user_monocle" — \todo Describe from README
 */
bool parse_desktop_modifiers(char *desc, desktop_select_t *sel)
{
	char *tok;
	while ((tok = strrchr(desc, CAT_CHR)) != NULL) {
		tok[0] = '\0';
		tok++;
		if (streq("occupied", tok)) {
			sel->occupied = OPTION_TRUE;
		} else if (streq("!occupied", tok)) {
			sel->occupied = OPTION_FALSE;
		GET_MOD(focused)
		GET_MOD(active)
		GET_MOD(urgent)
		GET_MOD(local)
		GET_MOD(tiled)
		GET_MOD(monocle)
		GET_MOD(user_tiled)
		GET_MOD(user_monocle)
		} else {
			return false;
		}
	}
	return true;

}


/** Parse node (window) modifiers from description selectors
 *
 * \param [in ] desc Node description selector string
 * \param [out] sel Node selector modifiers
 * \return True if parsed good, false otherwise
 * \note bspc(1) SELECTORS->Node->Modifiers
 *
 * Parse string to monitor modifiers:
 * - "focused" — Only consider the focused node
 * - "tiled" — \todo Describe from README
 * - "automatic" — \todo Describe from README
 * - "active" — \todo Describe from README
 * - "local" — \todo Describe from README
 * - "leaf" — \todo Describe from README
 * - "window" — \todo Describe from README
 * - "pseudo_tiled" — \todo Describe from README
 * - "floating" — \todo Describe from README
 * - "fullscreen" — \todo Describe from README
 * - "hidden" — \todo Describe from README
 * - "sticky" — \todo Describe from README
 * - "private" — \todo Describe from README
 * - "locked" — \todo Describe from README
 * - "marked" — \todo Describe from README
 * - "urgent" — \todo Describe from README
 * - "same_class" — \todo Describe from README
 * - "descendant_of" — \todo Describe from README
 * - "ancestor_of" — \todo Describe from README
 * - "below" — \todo Describe from README
 * - "normal" — \todo Describe from README
 * - "above" — \todo Describe from README
 * - "horizontal" — \todo Describe from README
 * - "vertical" — \todo Describe from README
 */
bool parse_node_modifiers(char *desc, node_select_t *sel)
{
	char *tok;
	while ((tok = strrchr(desc, CAT_CHR)) != NULL) {
		tok[0] = '\0';
		tok++;
		if (streq("tiled", tok)) {
			sel->tiled = OPTION_TRUE;
		} else if (streq("!tiled", tok)) {
			sel->tiled = OPTION_FALSE;
		GET_MOD(automatic)
		GET_MOD(focused)
		GET_MOD(active)
		GET_MOD(local)
		GET_MOD(leaf)
		GET_MOD(window)
		GET_MOD(pseudo_tiled)
		GET_MOD(floating)
		GET_MOD(fullscreen)
		GET_MOD(hidden)
		GET_MOD(sticky)
		GET_MOD(private)
		GET_MOD(locked)
		GET_MOD(marked)
		GET_MOD(urgent)
		GET_MOD(same_class)
		GET_MOD(descendant_of)
		GET_MOD(ancestor_of)
		GET_MOD(below)
		GET_MOD(normal)
		GET_MOD(above)
		GET_MOD(horizontal)
		GET_MOD(vertical)
		} else {
			return false;
		}
	}
	return true;
}

#undef GET_MOD
