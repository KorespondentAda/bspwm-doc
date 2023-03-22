/* Copyright (c) 2012, Bastien Dejean
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file
 * Program settings
 *
 * Defines settings variables and its initializing functions
 * \note bspc(1) SETTINGS
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "bspwm.h"
#include "settings.h"

/** Absolute path to the command used to retrieve rule consequences
 *
 * \note bspc(1) SETTINGS->Global Settings->external_rules_command
 * \note Default: "" (#EXTERNAL_RULES_COMMAND)
 *
 * The command will receive the following arguments:
 * window ID, class name, instance name, and intermediate consequences.
 * The output of that command must have the following format:
 * `key1=value1 key2=value2 ...`
 * (the valid key/value pairs are given in the description of the rule command).
 */
char external_rules_command[MAXLEN];
/** Status line prefix
 *
 * \note bspc(1) SETTINGS->Global Settings->status_prefix
 * \note Default: "W" (#STATUS_PREFIX)
 *
 * Prefix prepended to each of the status lines.
 */
char status_prefix[MAXLEN];

/** Color of the border of an unfocused window
 * \note bspc(1) SETTINGS->Global Settings->normal_border_color
 * \note Default: "#30302f" (#NORMAL_BORDER_COLOR)
 */
char normal_border_color[MAXLEN];
/** Color of the border of a focused window of an unfocused monitor
 * \note bspc(1) SETTINGS->Global Settings->active_border_color
 * \note Default: "#474645" (#ACTIVE_BORDER_COLOR)
 */
char active_border_color[MAXLEN];
/** Color of the border of a focused window of a focused monitor
 * \note bspc(1) SETTINGS->Global Settings->focused_border_color
 * \note Default: "#817f7f" (#FOCUSED_BORDER_COLOR)
 */
char focused_border_color[MAXLEN];
/** Color of the `node --presel-{dir,ratio}` message feedback area
 * \note bspc(1) SETTINGS->Global Settings->presel_feedback_color
 * \note Default: "#f4d775" (#PRESEL_FEEDBACK_COLOR)
 */
char presel_feedback_color[MAXLEN];

/** Monitor and Desktop padding
 *
 * \note bspc(1) SETTINGS->Monitor and Desktop Settings->top_padding, right_padding, bottom_padding, left_padding
 * \note Default: {0, 0, 0, 0} (#PADDING)
 *
 * Padding space added at the sides of the monitor or desktop.
 */
padding_t padding;
/** Desktop padding
 *
 * \note bspc(1) SETTINGS->Global Settings->top_monocle_padding, right_monocle_padding, bottom_monocle_padding, left_monocle_padding
 * \note Default: {0, 0, 0, 0} (#MONOCLE_PADDING)
 *
 * Set the desktop layout to monocle if there’s only one tiled window in the tree.
 */
padding_t monocle_padding;
/** Windows gap size
 *
 * \note bspc(1) SETTINGS->Desktop Settings->window_gap
 * \note Default: 6 (#WINDOW_GAP)
 *
 * Size of the gap that separates windows.
 */
int window_gap;
/** Window border width
 *
 * \note bspc(1) SETTINGS->Node Settings->border_width
 * \note Default: 1 (#BORDER_WIDTH)
 */
unsigned int border_width;
/** Default split ratio
 *
 * \note bspc(1) SETTINGS->Global Settings->split_ratio
 * \note Default: 0.5 (#SPLIT_RATIO)
 */
double split_ratio;
/** Initial tree polarity
 *
 * \note bspc(1) SETTINGS->Global Settings->initial_polarity
 * \todo Default: \c second_child (#SECOND_CHILD)
 *
 * On which child should a new window be attached when
 * adding a window on a single window tree in automatic mode.
 * Accept the following values: first_child, second_child.
 */
child_polarity_t initial_polarity;
/** Automatic insertion scheme
 *
 * \note bspc(1) SETTINGS->Global Settings->automatic_scheme
 * \todo Default: \c longest_side (#AUTOMATIC_SCHEME)
 *
 * The insertion scheme used when the insertion point is in automatic mode.
 * Accept the following values: longest_side, alternate, spiral.
 */
automatic_scheme_t automatic_scheme;
/** Adjustment on brother node removal
 *
 * \note bspc(1) SETTINGS->Global Settings->removal_adjustment
 * \note Default: \c true (#REMOVAL_ADJUSTMENT)
 *
 * \todo Not \c false by default, but not mentioned in manpage
 *
 * Adjust the brother when unlinking a node from the tree in accordance with
 * the automatic insertion scheme.
 */
bool removal_adjustment;
/** Directional focus tightness
 *
 * \note bspc(1) SETTINGS->Global Settings->directional_focus_tightness
 * \note Default: \c high (#TIGHTNESS_HIGH)
 *
 * The tightness of the algorithm used to decide whether a window
 * is on the [DIR](#direction_t) side of another window.
 * Accept the following values: high, low.
 */
tightness_t directional_focus_tightness;

/** Window interaction key
 *
 * \note bspc(1) SETTINGS->Global Settings->pointer_modifier
 * \note Default: \c mod4 (#POINTER_MODIFIER)
 *
 * Keyboard modifier used for moving or resizing windows.
 * Accept the following values:
 * shift, control, lock, mod1, mod2, mod3, mod4, mod5.
 */
uint16_t pointer_modifier;
/** Pointer motion notify interval
 *
 * \note bspc(1) SETTINGS->Global Settings->pointer_motion_interval
 * \note Default: 17 (#POINTER_MOTION_INTERVAL)
 *
 * The minimum interval, in milliseconds, between two motion notify events.
 */
uint32_t pointer_motion_interval;
/** Pointer buttons actions
 *
 * \note bspc(1) SETTINGS->Global Settings->pointer_action1, pointer_action2, pointer_action3
 * \note Default: {\c move, \c resize_side, \c resize_corner} (#ACTION_MOVE, #ACTION_RESIZE_SIDE, #ACTION_RESIZE_CORNER)
 *
 * \todo Pointer button docs?
 * Action performed when pressing #pointer_modifier + #button<n>.
 * Accept the following values: move, resize_side, resize_corner, focus, none.
 */
pointer_action_t pointer_actions[3];
/** Number of mapping notify events to handle
 *
 * \note bspc(1) SETTINGS->Global Settings->mapping_events_count
 * \note Default: 1 (#MAPPING_EVENTS_COUNT)
 *
 * Handle the next #mapping_events_count mapping notify events.
 * A negative value implies that every event needs to be handled.
 */
int8_t mapping_events_count;

/** Feedback from preselection
 *
 * \note bspc(1) SETTINGS->Global Settings->presel_feedback
 * \note Default: \c true (#PRESEL_FEEDBACK)
 *
 * Draw the preselection feedback area. Defaults to \c true.
 */
bool presel_feedback;
/** Borderless monocle mode
 *
 * \note bspc(1) SETTINGS->Global Settings->borderless_monocle
 * \note Default: \c false (#BORDERLESS_MONOCLE)
 *
 * Remove borders of tiled windows for the monocle desktop layout.
 */
bool borderless_monocle;
/** Gapless monocle mode
 *
 * \note bspc(1) SETTINGS->Global Settings->gapless_monocle
 * \note Default: \c false (#GAPLESS_MONOCLE)
 *
 * Remove gaps of tiled windows for the monocle desktop layout.
 */
bool gapless_monocle;
/** Automatic monocle
 *
 * \note bspc(1) SETTINGS->Global Settings->single_monocle
 * \note Default: \c false (#SINGLE_MONOCLE)
 *
 * Set the desktop layout to monocle if there’s only one tiled window
 * in the tree.
 */
bool single_monocle;
/** \todo Purpose?
 *
 * \note Not described in manpage
 * \note Default: \c false (#BORDERLESS_SINGLETON)
 */
bool borderless_singleton;

/** Focus the window under the pointer.
 *
 * \note bspc(1) SETTINGS->Global Settings->focus_follows_pointer
 * \note Default: \c false (#FOCUS_FOLLOWS_POINTER)
 */
bool focus_follows_pointer;
/** Pointer follows focused windows
 *
 * \note bspc(1) SETTINGS->Global Settings->pointer_follows_focus
 * \note Default: \c false (#POINTER_FOLLOWS_FOCUS)
 *
 * When focusing a window, put the pointer at its center.
 */
bool pointer_follows_focus;
/** Pointer follows focused monitors
 *
 * \note bspc(1) SETTINGS->Global Settings->pointer_follows_monitor
 * \note Default: \c false (#POINTER_FOLLOWS_MONITOR)
 *
 * When focusing a monitor, put the pointer at its center.
 */
bool pointer_follows_monitor;
/** Focus pointer button
 *
 * \note bspc(1) SETTINGS->Global Settings->click_to_focus
 * \note Default: \c button1 (#CLICK_TO_FOCUS)
 *
 * Button used for focusing a window (or a monitor).
 * The possible values are: button1, button2, button3, any, none.
 * Defaults to button1.
 */
int8_t click_to_focus;
/** Focusing click without action
 *
 * \note bspc(1) SETTINGS->Global Settings->swallow_first_click
 * \note Default: \c false (#SWALLOW_FIRST_CLICK)
 *
 * Don’t replay the click that makes a window focused if
 * #click_to_focus isn’t \c none.
 */
bool swallow_first_click;
/** Ignore focus requests
 *
 * \note bspc(1) SETTINGS->Global Settings->ignore_ewmh_focus
 * \note Default: \c false (#IGNORE_EWMH_FOCUS)
 *
 * Ignore EWMH focus requests coming from applications.
 */
bool ignore_ewmh_focus;
/** Ignore struts requests
 *
 * \note bspc(1) SETTINGS->Global Settings->ignore_ewmh_struts
 * \note Default: \c false (#IGNORE_EWMH_STRUTS)
 *
 * Ignore strut hinting from clients requesting to
 * reserve space (i.e. task bars).
 */
bool ignore_ewmh_struts;
/** Ignore fullscreen change requests
 *
 * \note bspc(1) SETTINGS->Global Settings->ignore_ewmh_fullscreen
 * \note Default: 0 (#IGNORE_EWMH_FULLSCREEN)
 *
 * Block the fullscreen state transitions that originate from an EWMH request.
 * The possible values are: none, all, or a comma separated list
 * of the following values: enter, exit.
 * \todo Maybe change by {none, enter, exit, all} list?
 */
state_transition_t ignore_ewmh_fullscreen;

/** Center pseudo tiled windows
 *
 * \note bspc(1) SETTINGS->Global Settings->center_pseudo_tiled
 * \note Default: \c true (#CENTER_PSEUDO_TILED)
 *
 * Center pseudo tiled windows into their tiling rectangles.
 * Defaults to true.
 */
bool center_pseudo_tiled;
/** Apply size hints
 *
 * \note bspc(1) SETTINGS->Global Settings->honor_size_hints
 * \note Default: \c false (#HONOR_SIZE_HINTS)
 *
 * Apply ICCCM window size hints.
 */
bool honor_size_hints;

/** Consider disabled monitors as disconnected
 *
 * \note bspc(1) SETTINGS->Global Settings->remove_disabled_monitors
 * \note Default: \c false (#REMOVE_DISABLED_MONITORS)
 */
bool remove_disabled_monitors;
/** Remove unplugged monitors
 *
 * \note bspc(1) SETTINGS->Global Settings->remove_unplugged_monitors
 * \note Default: \c false (#REMOVE_UNPLUGGED_MONITORS)
 */
bool remove_unplugged_monitors;
/** Merge overlapping monitors (the bigger remains).
 *
 * \note bspc(1) SETTINGS->Global Settings->merge_overlapping_monitors
 * \note Default: \c false (#MERGE_OVERLAPPING_MONITORS)
 */
bool merge_overlapping_monitors;

/** Run configuration script with \p run_level
 *
 * \param run_level
 *
 * \todo What is \p run_level?
 */
void run_config(int run_level)
{
	/** Here is the function workflow: */
	/** Fork then execute self */
	if (fork() == 0) {
		/** Close opened X connection */
		if (dpy != NULL) {
			close(xcb_get_file_descriptor(dpy));
		}
		setsid();
		char arg1[2];
		snprintf(arg1, 2, "%i", run_level);
		execl(config_path, config_path, arg1, (char *) NULL);
		/** Error in starting script is fatal */
		err("Couldn't execute the configuration file.\n");
	}
}

/** Load settings
 *
 * Initialize settings by macro-defined values
 */
void load_settings(void)
{
	/** Here is the default values table:
	 *
	 * | Variable | Value |
	 * |---|---|
	 * | #external_rules_command | #EXTERNAL_RULES_COMMAND |
	 * | #status_prefix | #STATUS_PREFIX |
	 * | #normal_border_color | #NORMAL_BORDER_COLOR |
	 * | #active_border_color | #ACTIVE_BORDER_COLOR |
	 * | #focused_border_color | #FOCUSED_BORDER_COLOR |
	 * | #presel_feedback_color | #PRESEL_FEEDBACK_COLOR |
	 * | #padding | #PADDING |
	 * | #monocle_padding | #MONOCLE_PADDING |
	 * | #window_gap | #WINDOW_GAP |
	 * | #border_width | #BORDER_WIDTH |
	 * | #split_ratio | #SPLIT_RATIO |
	 * | #initial_polarity | #SECOND_CHILD |
	 * | #automatic_scheme | #AUTOMATIC_SCHEME |
	 * | #removal_adjustment | #REMOVAL_ADJUSTMENT |
	 * | #directional_focus_tightness | #TIGHTNESS_HIGH |
	 * | #pointer_modifier | #POINTER_MODIFIER |
	 * | #pointer_motion_interval | #POINTER_MOTION_INTERVAL |
	 * | #pointer_actions | { #ACTION_MOVE, #ACTION_RESIZE_SIDE, #ACTION_RESIZE_CORNER } |
	 * | #mapping_events_count | #MAPPING_EVENTS_COUNT |
	 * | #presel_feedback | #PRESEL_FEEDBACK |
	 * | #borderless_monocle | #BORDERLESS_MONOCLE |
	 * | #gapless_monocle | #GAPLESS_MONOCLE |
	 * | #single_monocle | #SINGLE_MONOCLE |
	 * | #borderless_singleton | #BORDERLESS_SINGLETON |
	 * | #focus_follows_pointer | #FOCUS_FOLLOWS_POINTER |
	 * | #pointer_follows_focus | #POINTER_FOLLOWS_FOCUS |
	 * | #pointer_follows_monitor | #POINTER_FOLLOWS_MONITOR |
	 * | #click_to_focus | #CLICK_TO_FOCUS |
	 * | #swallow_first_click | #SWALLOW_FIRST_CLICK |
	 * | #ignore_ewmh_focus | #IGNORE_EWMH_FOCUS |
	 * | #ignore_ewmh_fullscreen | #IGNORE_EWMH_FULLSCREEN |
	 * | #ignore_ewmh_struts | #IGNORE_EWMH_STRUTS |
	 * | #center_pseudo_tiled | #CENTER_PSEUDO_TILED |
	 * | #honor_size_hints | #HONOR_SIZE_HINTS |
	 * | #remove_disabled_monitors | #REMOVE_DISABLED_MONITORS |
	 * | #remove_unplugged_monitors | #REMOVE_UNPLUGGED_MONITORS |
	 * | #merge_overlapping_monitors | #MERGE_OVERLAPPING_MONITORS |
	 */
	snprintf(external_rules_command, sizeof(external_rules_command), "%s", EXTERNAL_RULES_COMMAND);
	snprintf(status_prefix, sizeof(status_prefix), "%s", STATUS_PREFIX);

	snprintf(normal_border_color, sizeof(normal_border_color), "%s", NORMAL_BORDER_COLOR);
	snprintf(active_border_color, sizeof(active_border_color), "%s", ACTIVE_BORDER_COLOR);
	snprintf(focused_border_color, sizeof(focused_border_color), "%s", FOCUSED_BORDER_COLOR);
	snprintf(presel_feedback_color, sizeof(presel_feedback_color), "%s", PRESEL_FEEDBACK_COLOR);

	padding = (padding_t) PADDING;
	monocle_padding = (padding_t) MONOCLE_PADDING;
	window_gap = WINDOW_GAP;
	border_width = BORDER_WIDTH;
	split_ratio = SPLIT_RATIO;
	initial_polarity = SECOND_CHILD;
	automatic_scheme = AUTOMATIC_SCHEME;
	removal_adjustment = REMOVAL_ADJUSTMENT;
	directional_focus_tightness = TIGHTNESS_HIGH;

	pointer_modifier = POINTER_MODIFIER;
	pointer_motion_interval = POINTER_MOTION_INTERVAL;
	pointer_actions[0] = ACTION_MOVE;
	pointer_actions[1] = ACTION_RESIZE_SIDE;
	pointer_actions[2] = ACTION_RESIZE_CORNER;
	mapping_events_count = MAPPING_EVENTS_COUNT;

	presel_feedback = PRESEL_FEEDBACK;
	borderless_monocle = BORDERLESS_MONOCLE;
	gapless_monocle = GAPLESS_MONOCLE;
	single_monocle = SINGLE_MONOCLE;
	borderless_singleton = BORDERLESS_SINGLETON;

	focus_follows_pointer = FOCUS_FOLLOWS_POINTER;
	pointer_follows_focus = POINTER_FOLLOWS_FOCUS;
	pointer_follows_monitor = POINTER_FOLLOWS_MONITOR;
	click_to_focus = CLICK_TO_FOCUS;
	swallow_first_click = SWALLOW_FIRST_CLICK;
	ignore_ewmh_focus = IGNORE_EWMH_FOCUS;
	ignore_ewmh_fullscreen = IGNORE_EWMH_FULLSCREEN;
	ignore_ewmh_struts = IGNORE_EWMH_STRUTS;

	center_pseudo_tiled = CENTER_PSEUDO_TILED;
	honor_size_hints = HONOR_SIZE_HINTS;

	remove_disabled_monitors = REMOVE_DISABLED_MONITORS;
	remove_unplugged_monitors = REMOVE_UNPLUGGED_MONITORS;
	merge_overlapping_monitors = MERGE_OVERLAPPING_MONITORS;
}
