/*

Utils.h

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

class Utils
{

private:

    static std::map<std::string, std::string> table_columns;

    static int callback( void *, int argc, char **argv, char **azColName );

public:

    static std::map<std::string, InterfaceInfo> get_all_interfaces( void );

    static void load_stats( const std::string& a_mac );

    static void load_data_from_sqlite( const std::string& a_mac );

    static void load_data_from_files( const std::string& a_mac );

    static void save_stats( const std::string& a_mac );

    static void save_stats_to_sqlite( const std::string& a_mac );

    static void save_stats_to_files( const std::string& a_mac );

    static void save_stats_to_mysql( const std::string& a_mac );

    static void save_pid_file( const std::string& pid_file_path );

    static bool check_database_integrity( const std::string& a_mac );

    static bool repair_broken_database( const std::string& a_mac );

    static std::string get_mac( char* name );

    static void get_user_host( std::string& user, std::string& host );

    static void get_os_info( std::string& os_info );

    static void get_time( uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h );

    static void get_time( uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h, uint32_t* minute );

    static void seconds_to_date( time_t t, uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h );

    static time_t date_to_seconds( uint32_t y, uint32_t m, uint32_t d, uint32_t h = 0, uint32_t minute = 0, uint32_t sec = 0 );

    static bool contains( const std::string& str, const std::string& key );

    static bool starts_with( const std::string& str, const std::string& key );

    static std::string replace( const std::string& pattern, const std::string& with, const std::string& in ) noexcept;

    static std::vector<std::string> split( const std::string& str, const std::string& delim );

    static std::string float_to_string( float value, int32_t precision = 3 );

    static std::string to_string( uint64_t value, uint32_t min_string_length = 0 );

    static std::string to_string( uint32_t value, uint32_t min_string_length = 0 );

    static int32_t stoi( const std::string& str );

    static uint32_t stoui( const std::string& str );

    static int32_t hstoi( const string& str );

    static uint64_t stoull( const std::string& str );

    static std::vector<std::string> hexcolor_to_strings( std::string& hex_color );

    static std::string trim( const std::string &s );

    static std::string date_str( const std::string& type, uint32_t y,  uint32_t m, uint32_t d, uint32_t h );

    static void str2date( const std::string& str, const std::string& type, uint32_t* y,  uint32_t* m, uint32_t* d, uint32_t* h );

    static uint32_t days_in_month( uint32_t month, uint32_t year );

    static bool check_one_instance( void );

    static void remove_instance_object( void );

    static void set_signals_handler( void );

    static int32_t BecomeDaemon( void );

    static void MeterThread( void );

    static void make_program_run_at_startup( void );

    static std::string get_program_path( char** argv );

    static std::string get_path( const std::string& full_file_path );

    static void sleep_seconds( uint32_t seconds );

    static void set_endsession_handler( void );

    static bool dir_exists( const char *path );

    static int32_t make_path( const std::string& _s, mode_t mode );

    static std::string  rfc1123_datetime( time_t time );

    static bool save_settings( void );

    static std::string gz_compress( const std::string& data );

    static int32_t ensure_dir( const string& dir );

    static int32_t launch_default_browser( const string& url );
};

#endif // UTILS_H
