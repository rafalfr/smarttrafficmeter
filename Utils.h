/*

Utils.h

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
 * @file Utils.h
 * @brief Class definition for the Utils
 *
 * This file contains the definition of the Utils class.
 *
 * @author Rafał Frączek
 * @bug No known bugs
 *
 */

#ifndef UTILS_H
#define UTILS_H

#include <map>
#include <string>
#include <vector>
#include "InterfaceInfo.h"

using namespace std;

class Utils
{

private:

	static map<string, string> table_columns;

	static int callback( void *, int argc, char **argv, char **azColName );

public:

	static map<string, InterfaceInfo> get_all_interfaces( void );

	static void load_stats(const string& a_mac);

	static void load_data_from_sqlite(const string& a_mac );

	static void load_data_from_files(const string& a_mac );

	static void save_stats(const string& a_mac);

	static void save_stats_to_sqlite( const string& a_mac );

	static void save_stats_to_files( const string& a_mac );

	static void save_stats_to_mysql( const string& a_mac );

	static void save_pid_file(const string& pid_file_path);

	static string get_mac( char* name );

	static void get_user_host(string& user, string& host);

	static void get_os_info(string& os_info);

	static void get_time( uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h );

	static void seconds_to_date(time_t t, uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h );

	static time_t date_to_seconds(uint32_t y, uint32_t m, uint32_t d, uint32_t h=0, uint32_t minute=0, uint32_t sec=0);

	static bool contains( const string& str, const string& key );

	static bool starts_with( const string& str, const string& key );

	static string replace(const string& pattern, const string& with, const string& in) noexcept;

	static vector<string> split( const string& str, const string& delim );

	static string float_to_string(float value, int32_t precision=3);

	static string double_to_string(double value, int32_t precision=3);

	static string to_string( uint64_t value, uint32_t min_string_length=0 );

	static string to_string( uint32_t value, uint32_t min_string_length=0 );

	static int32_t stoi(const string& str);

	static uint32_t stoui(const string& str);

	static int32_t hstoi(const string& str);

	static float stof(const string& str);

	static uint64_t stoull(const string& str);

	static vector<string> hexcolor_to_strings(string& hex_color);

	static string trim( const std::string &s );

	static string date_str(const string& type, uint32_t y,  uint32_t m, uint32_t d, uint32_t h );

	static void str2date(const string& str, const string& type, uint32_t* y,  uint32_t* m, uint32_t* d, uint32_t* h );

	static bool check_one_instance( void );

	static void remove_instance_object(void);

	static void set_signals_handler(void);

	static int32_t BecomeDaemon(void);

	static void MeterThread(void);

	static void make_program_run_at_startup(void);

	static string get_program_path(char** argv);

	static string get_path(const string& full_file_path);

	static void sleep_seconds(uint32_t seconds);

	static string to_narrow(const wchar_t * src);

	static void set_endsession_handler(void);

	static bool dir_exists( const char *path );

	static int32_t make_path( const string& _s, mode_t mode );

	static string  rfc1123_datetime( time_t time );

	static bool save_settings(void);

};

#endif // UTILS_H
