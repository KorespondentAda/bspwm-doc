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

/**
 * \file
 * \todo Describe default values macros
 * \todo Group macros by purposes
 */

#ifndef BSPWM_SETTINGS_H
#define BSPWM_SETTINGS_H

#include "types.h"

#define POINTER_MODIFIER         XCB_MOD_MASK_4
#define POINTER_MOTION_INTERVAL  17
/** Initial value for #external_rules_command. \see load_settings() */
#define EXTERNAL_RULES_COMMAND   ""
/** Initial value for #status_prefix. \see load_settings() */
#define STATUS_PREFIX            "W"

/** Initial value for #normal_border_color. \see load_settings() */
#define NORMAL_BORDER_COLOR           "#30302f"
/** Initial value for #active_border_color. \see load_settings() */
#define ACTIVE_BORDER_COLOR           "#474645"
/** Initial value for #focused_border_color. \see load_settings() */
#define FOCUSED_BORDER_COLOR          "#817f7f"
/** Initial value for #presel_feedback_color. \see load_settings() */
#define PRESEL_FEEDBACK_COLOR         "#f4d775"

/** Initial value for #padding. \see load_settings() */
#define PADDING              {0, 0, 0, 0}
/** Initial value for #monocle_padding. \see load_settings() */
#define MONOCLE_PADDING      {0, 0, 0, 0}
/** Initial value for #window_gap. \see load_settings() */
#define WINDOW_GAP           6
/** Initial value for #border_width. \see load_settings() */
#define BORDER_WIDTH         1
/** Initial value for #split_ratio. \see load_settings() */
#define SPLIT_RATIO          0.5
/** Initial value for #automatic_scheme. \see load_settings() */
#define AUTOMATIC_SCHEME     SCHEME_LONGEST_SIDE
/** Initial value for #removal_adjustment. \see load_settings() */
#define REMOVAL_ADJUSTMENT   true

/** Initial value for #presel_feedback. \see load_settings() */
#define PRESEL_FEEDBACK             true
/** Initial value for #borderless_monocle. \see load_settings() */
#define BORDERLESS_MONOCLE          false
/** Initial value for #gapless_monocle. \see load_settings() */
#define GAPLESS_MONOCLE             false
/** Initial value for #single_monocle. \see load_settings() */
#define SINGLE_MONOCLE              false
/** Initial value for #borderless_singleton. \see load_settings() */
#define BORDERLESS_SINGLETON        false

/** Initial value for #focus_follows_pointer. \see load_settings() */
#define FOCUS_FOLLOWS_POINTER       false
/** Initial value for #pointer_follows_focus. \see load_settings() */
#define POINTER_FOLLOWS_FOCUS       false
/** Initial value for #pointer_follows_monitor. \see load_settings() */
#define POINTER_FOLLOWS_MONITOR     false
/** Initial value for #click_to_focus. \see load_settings() */
#define CLICK_TO_FOCUS              XCB_BUTTON_INDEX_1
/** Initial value for #swallow_first_click. \see load_settings() */
#define SWALLOW_FIRST_CLICK         false
/** Initial value for #ignore_ewmh_focus. \see load_settings() */
#define IGNORE_EWMH_FOCUS           false
/** Initial value for #ignore_ewmh_fullscreen. \see load_settings() */
#define IGNORE_EWMH_FULLSCREEN      0
/** Initial value for #ignore_ewmh_struts. \see load_settings() */
#define IGNORE_EWMH_STRUTS          false

/** Initial value for #center_pseudo_tiled. \see load_settings() */
#define CENTER_PSEUDO_TILED         true
/** Initial value for #honor_size_hints. \see load_settings() */
#define HONOR_SIZE_HINTS            false
/** Initial value for #mapping_events_count. \see load_settings() */
#define MAPPING_EVENTS_COUNT        1

/** Initial value for #remove_disabled_monitors. \see load_settings() */
#define REMOVE_DISABLED_MONITORS    false
/** Initial value for #remove_unplugged_monitors. \see load_settings() */
#define REMOVE_UNPLUGGED_MONITORS   false
/** Initial value for #merge_overlapping_monitors. \see load_settings() */
#define MERGE_OVERLAPPING_MONITORS  false

extern char external_rules_command[MAXLEN];
extern char status_prefix[MAXLEN];

extern char normal_border_color[MAXLEN];
extern char active_border_color[MAXLEN];
extern char focused_border_color[MAXLEN];
extern char presel_feedback_color[MAXLEN];

extern padding_t padding;
extern padding_t monocle_padding;
extern int window_gap;
extern unsigned int border_width;
extern double split_ratio;
extern child_polarity_t initial_polarity;
extern automatic_scheme_t automatic_scheme;
extern bool removal_adjustment;
extern tightness_t directional_focus_tightness;

extern uint16_t pointer_modifier;
extern uint32_t pointer_motion_interval;
extern pointer_action_t pointer_actions[3];
extern int8_t mapping_events_count;

extern bool presel_feedback;
extern bool borderless_monocle;
extern bool gapless_monocle;
extern bool single_monocle;
extern bool borderless_singleton;

extern bool focus_follows_pointer;
extern bool pointer_follows_focus;
extern bool pointer_follows_monitor;
extern int8_t click_to_focus;
extern bool swallow_first_click;
extern bool ignore_ewmh_focus;
extern bool ignore_ewmh_struts;
extern state_transition_t ignore_ewmh_fullscreen;

extern bool center_pseudo_tiled;
extern bool honor_size_hints;

extern bool remove_disabled_monitors;
extern bool remove_unplugged_monitors;
extern bool merge_overlapping_monitors;

void run_config(int run_level);
void load_settings(void);

#endif
