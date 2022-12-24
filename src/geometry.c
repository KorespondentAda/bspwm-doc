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
 * Geometry calculations
 */

#include <math.h>
#include "types.h"
#include "settings.h"
#include "geometry.h"

/**
 * Check if point \p p is inside of rectangle \p r
 *
 * \param p Point to check
 * \param r Rectangle to search within
 * \return True if \p p inside \p r including up and left boundings
 */
bool is_inside(xcb_point_t p, xcb_rectangle_t r)
{
	return (p.x >= r.x && p.x < (r.x + r.width) &&
	        p.y >= r.y && p.y < (r.y + r.height));
}

/**
 * Check if rectangle \p a fully contains rectangle \p b or they equals
 *
 * \param a Outer rectangle
 * \param b Inner rectangle
 * \return True if \p a contains \p b
 */
bool contains(xcb_rectangle_t a, xcb_rectangle_t b)
{
	return (a.x <= b.x && (a.x + a.width) >= (b.x + b.width) &&
	        a.y <= b.y && (a.y + a.height) >= (b.y + b.height));
}

/**
 * Calculate area of rectangle
 *
 * \param r Rectangle to calculate area
 * \return Area of given rectangle
 */
unsigned int area(xcb_rectangle_t r)
{
	return r.width * r.height;
}

/**
 * Calculate distance between opposite edges of rectangles
 *
 * \todo Maybe rewrite with something like ABS macro?
 * \param r1 First rectangle
 * \param r2 Second rectangle
 * \param dir Direction for edge selection
 * \return Distance between the \p dir edge of \p r1 and the opposite \p dir edge of \p r2.
 */
uint32_t boundary_distance(xcb_rectangle_t r1, xcb_rectangle_t r2, direction_t dir)
{
	xcb_point_t r1_max = {r1.x + r1.width - 1, r1.y + r1.height - 1};
	xcb_point_t r2_max = {r2.x + r2.width - 1, r2.y + r2.height - 1};
	switch (dir) {
		case DIR_NORTH:
			return r2_max.y > r1.y ? r2_max.y - r1.y : r1.y - r2_max.y;
			break;
		case DIR_WEST:
			return r2_max.x > r1.x ? r2_max.x - r1.x : r1.x - r2_max.x;
			break;
		case DIR_SOUTH:
			return r2.y < r1_max.y ? r1_max.y - r2.y : r2.y - r1_max.y;
			break;
		case DIR_EAST:
			return r2.x < r1_max.x ? r1_max.x - r2.x : r2.x - r1_max.x;
			break;
		default:
			return UINT32_MAX;
	}
}

/**
 * Check if second rectangle is on specified direction of first rectangle
 *
 * \param r1 First rectangle
 * \param r2 Second rectangle
 * \param dir Direction to check
 * \return True if \p r2 on the \p dir side of \p r1
 */
bool on_dir_side(xcb_rectangle_t r1, xcb_rectangle_t r2, direction_t dir)
{
	/** Here is the function workflow: */
	xcb_point_t r1_max = {r1.x + r1.width - 1, r1.y + r1.height - 1};
	xcb_point_t r2_max = {r2.x + r2.width - 1, r2.y + r2.height - 1};

	/** Eliminate rectangles on the opposite side */
	switch (directional_focus_tightness) {
		case TIGHTNESS_LOW:
			switch (dir) {
				case DIR_NORTH:
					if (r2.y > r1_max.y) {
						return false;
					}
					break;
				case DIR_WEST:
					if (r2.x > r1_max.x) {
						return false;
					}
					break;
				case DIR_SOUTH:
					if (r2_max.y < r1.y) {
						return false;
					}
					break;
				case DIR_EAST:
					if (r2_max.x < r1.x) {
						return false;
					}
					break;
				default:
					return false;
			}
			break;
		case TIGHTNESS_HIGH:
			switch (dir) {
				case DIR_NORTH:
					if (r2.y >= r1.y) {
						return false;
					}
					break;
				case DIR_WEST:
					if (r2.x >= r1.x) {
						return false;
					}
					break;
				case DIR_SOUTH:
					if (r2_max.y <= r1_max.y) {
						return false;
					}
					break;
				case DIR_EAST:
					if (r2_max.x <= r1_max.x) {
						return false;
					}
					break;
				default:
					return false;
			}
			break;
		default:
			return false;
	}

	/** Is there a shared vertical/horizontal range? */
	switch (dir) {
		case DIR_NORTH:
		case DIR_SOUTH:
			return
				(r2.x >= r1.x && r2.x <= r1_max.x) ||
				(r2_max.x >= r1.x && r2_max.x <= r1_max.x) ||
				(r1.x > r2.x && r1.x < r2_max.x);
			break;
		case DIR_WEST:
		case DIR_EAST:
			return
				(r2.y >= r1.y && r2.y <= r1_max.y) ||
				(r2_max.y >= r1.y && r2_max.y <= r1_max.y) ||
				(r1.y > r2.y && r1_max.y < r2_max.y);
			break;
		default:
			return false;
	}
}

/**
 * Check if two rectangles is equals
 *
 * \todo Rename arguments to r1 and r2 for consistance
 *
 * \param a First rectangle
 * \param b Second rectangle
 * \return True if edge coordinates of both rectangles equals
 */
bool rect_eq(xcb_rectangle_t a, xcb_rectangle_t b)
{
	return (a.x == b.x && a.y == b.y &&
	        a.width == b.width && a.height == b.height);
}

/**
 * Compare rectangles
 *
 * First rectangle greater than second if minimal coordinate of first rectangle
 * not lesser than maximal of second.
 * If rectangles crossed, their areas compared.
 *
 * \param r1 First rectangle
 * \param r2 Second rectangle
 * \return Comparsion result
 */
int rect_cmp(xcb_rectangle_t r1, xcb_rectangle_t r2)
{
	if (r1.y >= (r2.y + r2.height)) {
		return 1;
	} else if (r2.y >= (r1.y + r1.height)) {
		return -1;
	} else {
		if (r1.x >= (r2.x + r2.width)) {
			return 1;
		} else if (r2.x >= (r1.x + r1.width)) {
			return -1;
		} else {
			return area(r2) - area(r1);
		}
	}
}
