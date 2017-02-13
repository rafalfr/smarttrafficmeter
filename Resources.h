/*

Resources.h

Copyright (C) 2016 Rafał Frączek

This file is part of Smart Traffic Meter.

Smart Traffic Meter is free software:
you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

Smart Traffic Meter is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Smart Traffic Meter.
If not, see http://www.gnu.org/licenses/.

*/

/**
 * @file Resources.h
 * @brief Class definition for the Resources
 *
 * This file contains the definition of the Resources class.
 *
 * @author Rafał Frączek
 * @bug No known bugs
 *
 */

#ifndef RESOURCES_H
#define RESOURCES_H

#include <cstdint>

class Resources
{

public:

	static uint32_t chart_js_length;

	static uint8_t chart_js[];

	static uint32_t background_image_length;

	static uint8_t background_image[];

	static uint32_t speed_update_js_length;

	static uint8_t speed_update_js[];

	static uint32_t smoothie_js_length;

    static uint8_t smoothie_js[];

	static uint32_t jquery_js_length;

    static uint8_t jquery_js[];

	static uint32_t jscolor_js_length;

    static uint8_t jscolor_js[];

    static uint32_t stopwatch_js_length;

    static uint8_t stopwatch_js[];
};

#endif // RESOURCES_H
