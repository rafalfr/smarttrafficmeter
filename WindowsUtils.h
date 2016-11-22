/*

WindowsUtils.h

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
 * @file WindowsUtils.h
 * @brief Class definition for the WindowsUtils
 *
 * This file contains the definition of the WindowsUtils class.
 *
 * @author Rafał Frączek
 * @bug No known bugs
 *
 */

#ifndef WINDOWSUTILS_H
#define WINDOWSUTILS_H

#ifdef __WIN32

#include <map>
#include <string>
#include <windows.h>
#include "InterfaceInfo.h"


class WindowsUtils
{

public:

    static void MeterThread(void);

    static std::map<std::string, InterfaceInfo> get_all_interfaces( void );

    static LONG signal_handler( LPEXCEPTION_POINTERS p );

    static BOOL WINAPI console_ctrl_handler( DWORD fdwCtrlType );

    static void set_signals_handler(void);

    static int become_daemon(void);

    static void make_program_run_at_startup(void);

    static bool check_one_instance( void );

    static string get_program_path(void);

    static void handle_endsession_message(void);

    static bool dir_exists( const char *path );

    static int32_t make_path( const string& _s, mode_t mode );

private:

    static LRESULT APIENTRY WndProc(HWND handle, UINT umsg, WPARAM wparam, LPARAM lparam);
};

#endif // _WIN32

#endif // WINDOWSUTILS_H
