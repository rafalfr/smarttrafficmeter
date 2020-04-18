/*

LinuxUtils.h

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
 * @file LinuxUtils.h
 * @brief Class definition for the LinuxUtils
 *
 * This file contains the definition of the LinuxUtils class.
 *
 * @author Rafał Frączek
 * @bug No known bugs
 *
 */

#ifndef LINUXUTILS_H
#define LINUXUTILS_H

#ifdef __linux

#include <map>
#include <string>
#include "InterfaceInfo.h"
#include "config.h"
#include <bfd.h>

class LinuxUtils
{

private:

    static bfd* abfd;
    static asymbol **syms;
    static asection *text;

public:

    static void * MeterThread( void );

    static void signal_handler( int signal );

    static int BecomeDaemon( int flags );

    static std::string get_mac( char* name );

    static std::map<std::string, InterfaceInfo> get_all_interfaces( void );

    static bool check_one_instance( void );

    static void remove_instance_object( void );

    static void set_signals_handler( void );

    static string resolve( const unsigned long address );

    static string get_program_path( const char* argv0 );

    static void get_user_host( string& user, string& host );

    static void get_os_info( string& os_info );

    static bool dir_exists( const char *path );

    static int32_t make_path( const string& _s, mode_t mode );

    static void save_pid_file( const string& pid_file_path );

    static int32_t ensure_dir( const string& dir );

    static int32_t launch_default_browser( const string & url );
};

#endif // __linux

#endif // LINUXUTILS_H
