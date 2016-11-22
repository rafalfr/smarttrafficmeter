/*

InterfaceSpeedMeter.cpp

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
#include <sys/time.h>
#include "InterfaceSpeedMeter.h"

/** \copydoc my_func1 */
InterfaceSpeedMeter::InterfaceSpeedMeter() : max_buf_items( 5 ), buf()
{
    map<string, uint64_t> item;

    uint64_t time = get_time_milisecs();

    item["rx"] = 0ULL;
    item["tx"] = 0ULL;
    item["time"] = time;
    buf.push_front( item );
}


InterfaceSpeedMeter::InterfaceSpeedMeter( const InterfaceSpeedMeter& ism ) : max_buf_items( ism.max_buf_items ), buf()
{
    //copy constructor with a buf deep copy
    for ( auto const & stats : ism.buf )
    {
        uint64_t rx = stats.at( "rx" );
        uint64_t tx = stats.at( "tx" );
        uint64_t time = stats.at( "time" );
        map<string, uint64_t> item;

        item["rx"] = rx;
        item["tx"] = tx;
        item["time"] = time;

        buf.push_front( item );
    }
}

InterfaceSpeedMeter::~InterfaceSpeedMeter()
{

}

uint64_t InterfaceSpeedMeter::get_rx_speed( void ) const
{
    const map<string, uint64_t> & first = buf.front();
    const map<string, uint64_t> & last = buf.back();
    uint64_t data = first.at( "rx" ) - last.at( "rx" );
    uint64_t time = first.at( "time" ) - last.at( "time" );

    if ( time > 0ULL )
    {
        /*time is in miliseconds, so we multiply the result by 1000 to get speed in bits/s */
        return ( 8ULL * 1000ULL * data ) / time;
    }
    else
    {
        return 0ULL;
    }
}

uint64_t InterfaceSpeedMeter::get_tx_speed( void ) const
{
    const map<string, uint64_t> & first = buf.front();
    const map<string, uint64_t> & last = buf.back();
    uint64_t data = first.at( "tx" ) - last.at( "tx" );
    uint64_t time = first.at( "time" ) - last.at( "time" );

    if ( time > 0ULL )
    {
        return ( 8ULL * 1000ULL * data ) / time;
    }
    else
    {
        return 0ULL;
    }
}

uint64_t InterfaceSpeedMeter::get_time_milisecs( void )
{
    struct timeval  tv;
    gettimeofday( &tv, nullptr );

    return ( tv.tv_sec ) * 1000LL + ( tv.tv_usec ) / 1000ULL ;
}


void InterfaceSpeedMeter::update( uint64_t _rx, uint64_t _tx )
{
    map<string, uint64_t> item;

    uint64_t time = get_time_milisecs();

    item["rx"] = _rx;
    item["tx"] = _tx;
    item["time"] = time;

    buf.push_front( item );

    while ( buf.size() >= max_buf_items )
    {
        buf.pop_back();
    }
}
