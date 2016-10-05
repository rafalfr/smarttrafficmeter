/*

InterfaceStats.h

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

#ifndef INTERFACESTATS_H
#define INTERFACESTATS_H
#include <stdint.h>

class InterfaceStats
{
private:
    bool first_update;
    uint64_t rx;
    uint64_t tx;
    uint64_t p_rx;
    uint64_t p_tx;

public:
    InterfaceStats();
    InterfaceStats( const InterfaceStats& );
    ~InterfaceStats();

    void set_initial_stats( uint64_t _tx, uint64_t _rx );
    void set_current_stats( uint64_t _tx, uint64_t _rx );
    void update( uint64_t _tx, uint64_t _rx );
    uint64_t recieved( void ) const;
    uint64_t transmited( void ) const;
};

#endif // INTERFACESTATS_H
