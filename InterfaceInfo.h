/*

InterfaceInfo.h

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

#ifndef INTERFACEINFO_H
#define INTERFACEINFO_H
#include <string>

using namespace std;

class InterfaceInfo
{
private:
    string mac;
    string name;
    string desc;
    string ip4;
    string ip6;

public:
    InterfaceInfo();
    InterfaceInfo(const InterfaceInfo& info);

    ~InterfaceInfo();

    InterfaceInfo& operator=(const InterfaceInfo& right);

    string get_mac ( void ) const;
    string get_name ( void ) const;
    string get_desc(void) const;
    string get_ip4 ( void ) const;
    string get_ip6 ( void ) const;
    void set_mac ( const char* t_mac );
    void set_name ( const char* t_name );
    void set_desc(const char* t_desc);
    void set_ip4 ( const char* t_ip4 );
    void set_ip6 ( const char* t_ip6 );
};

#endif // INTERFACEINFO_H
