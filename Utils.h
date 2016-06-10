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

	static void load_data_from_sqlite( void );

	static void load_data_from_files( void );

	static void save_stats_to_sqlite( void );

	static void save_stats_to_files( void );

	static string get_mac( char* name );

	static void get_time( uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h );

	static bool contians( const string& str, const string& key );

	static bool starts_with( const string& str, const string& key );

	static string replace(const string& pattern, const string& with, const string& in) noexcept;

	static vector<string> split( const string& str, const string& delim );

	static string to_string( uint64_t value, uint32_t min_string_lenght=0 );

	static string to_string( uint32_t value, uint32_t min_string_lenght=0 );

	static vector<string> hexcolor_to_strings(string& hex_color);

	static string trim( const std::string &s );

	static string date_str(const string& type, uint32_t y,  uint32_t m, uint32_t d, uint32_t h );

	static bool check_one_instance( void );

};

#endif // UTILS_H
