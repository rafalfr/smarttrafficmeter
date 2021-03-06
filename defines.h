/*

defines.h

Copyright (C) 2020 Rafał Frączek

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
 * @file defines.h
 * @brief Globally defined structs
 *
 * This file contains the definition of the globally defined structs
 *
 * @author Rafał Frączek
 * @bug No known bugs
 *
 */


#ifndef DEFINES_H_INCLUDED
#define DEFINES_H_INCLUDED

struct date
{
    uint32_t year;
    uint32_t month;
    uint32_t day;
    uint32_t hour;
};

#endif // DEFINES_H_INCLUDED
