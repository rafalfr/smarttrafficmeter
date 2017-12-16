/*

InterfaceInfo.cpp

Copyright (C) 2018 Rafał Frączek

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

#include "InterfaceInfo.h"

/** @brief InterfaceInfo
 *
 * The default InterfaceInfo constructor
 *
 * @param void
 * @return void
 *
 */
InterfaceInfo::InterfaceInfo() : mac( "" ), name( "" ), desc( "" ), ip4( "" ), ip6( "" )
{
}

/** @brief InterfaceInfo
 *
 * The copy constructor
 *
 * @param InterfaceInfo
 * @return void
 *
 */
InterfaceInfo::InterfaceInfo( const InterfaceInfo& info ) : mac( info.get_mac() ), name( info.get_name() ), desc( "" ), ip4( info.get_ip4() ), ip6( info.get_ip6() )
{
}

/** @brief ~InterfaceInfo
 *
 * destructor
 *
 * @param void
 * @return void
 *
 */
InterfaceInfo::~InterfaceInfo()
{
    mac.clear();
    ip4.clear();
    ip6.clear();
    name.clear();
    desc.clear();
}

/** @brief operator=
  *
  * The assignment operator
  *
  * @param InterfaceInfo
  * @return InterfaceInfo
  *
  */
InterfaceInfo& InterfaceInfo::operator=( const InterfaceInfo& right )
{
    if ( & right != this )
    {
        mac.clear();
        mac.append( right.get_mac() );
        ip4.clear();
        ip4.append( right.get_ip4() );
        ip6.clear();
        ip6.append( right.get_ip6() );
        name.clear();
        name.append( right.get_name() );
        desc.clear();
        desc.append( right.get_desc() );
    }

    return * this;
}

/** @brief get_mac
 *
 * The function returns the network interface MAC address
 *
 * @param void
 * @return network interface MAC
 *
 */
string InterfaceInfo::get_mac( void ) const
{
    return mac;
}

/** @brief get_name
 *
 * The function returns the network interface name
 *
 * @param void
 * @return network interface name
 *
 */
string InterfaceInfo::get_name( void ) const
{
    return name;
}

/** @brief get_desc
  *
  * The function returns the network interface description
  *
  * @param void
  * @return network interface description
  *
  */
string InterfaceInfo::get_desc( void ) const
{
    return desc;
}

/** @brief get_ip4
 *
 * The function returns the network interface IP4 address
 *
 * @param void
 * @return network interface IP4 address
 *
 */
string InterfaceInfo::get_ip4( void ) const
{
    return ip4;
}

/** @brief get_ip6
 *
 * The function returns the network interface IP6 address
 *
 * @param void
 * @return network interface IP6 address
 *
 */
string InterfaceInfo::get_ip6( void ) const
{
    return ip6;
}

/** @brief set_mac
 *
 * The function sets the MAC address for the network interface
 *
 * @param MAC address as a string
 * @return void
 *
 */
void InterfaceInfo::set_mac( const char* t_mac )
{
    mac.clear();
    mac.append( t_mac );
}

/** @brief set_name
 *
 * The function sets the MAC address for the network interface
 *
 * @param network interface name
 * @return void
 *
 */
void InterfaceInfo::set_name( const char* t_name )
{
    name.clear();
    name.append( t_name );
}

/** @brief set_desc
  *
  * The function sets the MAC address for the network interface
  *
  * @param network interface description
  * @return void
  *
  */
void InterfaceInfo::set_desc( const char* t_desc )
{
    desc.clear();
    desc.append( t_desc );
}

/** @brief set_ip4
 *
 * The function sets the IP4 address for the network interface
 *
 * @param network interface IP4 address
 * @return void
 *
 */
void InterfaceInfo::set_ip4( const char* t_ip4 )
{
    ip4.clear();
    ip4.append( t_ip4 );
}

/** @brief set_ip6
 *
 * The function sets the IP6 address for the network interface
 *
 * @param network interface IP6 address
 * @return void
 *
 */
void InterfaceInfo::set_ip6( const char* t_ip6 )
{
    ip6.clear();
    ip6.append( t_ip6 );
}
