#ifndef UTILS_H
#define UTILS_H
#include <map>
#include <string>
#include "InterfaceInfo.h"

using namespace std;

class Utils
{
public:
	static map<string, InterfaceInfo> get_all_interfaces ( void );
	static string get_mac ( char* name );
	static void get_time( uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h );
};

#endif // UTILS_H
