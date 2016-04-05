#ifndef UTILS_H
#define UTILS_H
#include <map>
#include <string>
#include <vector>
#include "InterfaceInfo.h"

using namespace std;

class Utils
{
public:
	static map<string, InterfaceInfo> get_all_interfaces( void );
	static string get_mac( char* name );
	static void get_time( uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h );
	static bool contians( const string& str, const string& key );
	static bool starts_with( const string& str, const string& key );
	static string replace(const string& pattern, const string& with, const string& in) noexcept;
	static vector<string> split( const string& str, const string& delim );
	static string to_string( uint64_t value );
	static string trim( const std::string &s );
	static bool check_one_instance( void );
};

#endif // UTILS_H
