#ifndef LOGGER_H
#define LOGGER_H
#include <string>

using namespace std;

class Logger
{
public:
	static void LogDebug( const string& );
	static void LogInfo( const string& );
	static void LogError( const string& );
};

#endif // LOGGER_H
