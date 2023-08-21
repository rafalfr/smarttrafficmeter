/*

InterfaceSpeedMeter.h

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
 * @file InterfaceSpeedMeter.h
 * @brief Class definition for the InterfaceSpeedMeter
 *
 * This file contains the definition of the InterfaceSpeedMeter class.
 *
 * @author Rafał Frączek
 * @bug No known bugs
 *
 */

#ifndef INTERFACESPEEDMETER_H
#define INTERFACESPEEDMETER_H

#include <map>
#include <list>
#include <string>
#include <cstdint>

using std::map;
using std::list;
using std::string;

class InterfaceSpeedMeter
{

private:

    list < map< string, uint64_t> >::size_type max_buf_items;

    list < map< string, uint64_t> > buf;

    static uint64_t get_time_milisecs( void );

public:

    InterfaceSpeedMeter();

    InterfaceSpeedMeter( const InterfaceSpeedMeter& );

    InterfaceSpeedMeter& operator=(const InterfaceSpeedMeter& );

    virtual ~InterfaceSpeedMeter();

    uint64_t get_rx_speed( void ) const;

    uint64_t get_tx_speed( void ) const;

    void update( uint64_t _rx, uint64_t _tx );

};

#endif // INTERFACESPEEDMETER_H
