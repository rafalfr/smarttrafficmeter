/*

InterfaceStats.cpp

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

#include <limits>
#include "InterfaceStats.h"

/** @brief InterfaceStats
 *
 * The default constructor
 *
 * @param void
 * @return void
 *
 */
InterfaceStats::InterfaceStats() : first_update( true ), rx( 0ULL ), tx( 0ULL ), p_rx( 0ULL ), p_tx( 0ULL )
{
}

/** @brief InterfaceStats
 *
 * The copy constructor
 *
 * @param InterfaceStats object
 * @return void
 *
 */
InterfaceStats::InterfaceStats( const InterfaceStats& stats ) : first_update( stats.first_update ), rx( stats.rx ), tx( stats.tx ), p_rx( stats.p_rx ), p_tx( stats.p_tx )
{
}

/** @brief operator=
  *
  * The assignment operator
  *
  */
InterfaceStats& InterfaceStats::operator=(const InterfaceStats& is )
{
    if ( this != &is )
    {
        first_update = is.first_update;
        rx = is.rx;
        tx = is.tx;
        p_rx = is.p_rx;
        p_tx = is.p_tx;
    }

    return *this;
}

/** @brief ~InterfaceStats
 *
 * The default destructor
 *
 * @param void
 * @return void
 *
 */
InterfaceStats::~InterfaceStats()
{
}


/** @brief set_first_update
  *
  * @todo: document this function
  */
void InterfaceStats::set_first_update( bool _first_update )
{
    first_update = _first_update;
}

/** @brief set_initial_stats
 *
 * The method sets the initial statistics
 *
 * @param transmitted bytes
 * @param received bytes
 * @return void
 *
 */
void InterfaceStats::set_initial_stats( uint64_t _tx, uint64_t _rx )
{
    rx = _rx;
    tx = _tx;
    first_update = true;
}

/** @brief set_current_stats
 *
 * The method sets the current statistics
 *
 * @param transmitted bytes
 * @param received bytes
 * @return void
 *
 */
void InterfaceStats::set_current_stats( uint64_t _tx, uint64_t _rx )
{
    rx = _rx;
    tx = _tx;
    first_update = false;
}

/** @brief update
 *
 * The method updates the current statistics
 *
 * @param transmitted bytes
 * @param received bytes
 * @return void
 *
 */
void InterfaceStats::update( uint64_t _tx, uint64_t _rx )
{
    if ( first_update == true )
    {
        p_rx = _rx;
        p_tx = _tx;
        first_update = false;
    }

    if ( _rx >= p_rx )
    {
        rx +=  _rx - p_rx ;
    }
    else
    {
#ifdef __linux
        // In linux we sometimes need to perform this operation because the current value
        // obtained from the os kernel can be lower than the previous one.
        // This is because the os uses only 32-bits unsigned integer to store
        // the number of transmitted and received bytes.
        rx += ( _rx + ( uint64_t ) std::numeric_limits<uint32_t>::max() ) - p_rx;
#else
        p_rx = _rx;
#endif // __linux
    }

    if ( _tx >= p_tx )
    {
        tx +=  _tx - p_tx ;
    }
    else
    {
#ifdef __linux
        tx += ( _tx + ( uint64_t ) std::numeric_limits<uint32_t>::max() ) - p_tx;
#else
        p_tx = _tx;
#endif // __linux
    }

    p_rx = _rx;
    p_tx = _tx;
}

/** @brief received
 *
 * The method returns the number of received bytes
 *
 * @param void
 * @return the number of received bytes
 *
 */
uint64_t InterfaceStats::received( void ) const
{
    return rx;
}

/** @brief transmitted
 *
 * The method returns the number of transmitted bytes
 *
 * @param void
 * @return the number of transmitted bytes
 *
 */
uint64_t InterfaceStats::transmitted( void ) const
{
    return tx;
}
