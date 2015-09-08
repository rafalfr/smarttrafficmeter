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
};

#endif // UTILS_H
