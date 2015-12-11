#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <time.h>

#include "Logger.h"

using namespace std;

/** @brief LogDebug
  *
  * @todo: document this function
  */
void Logger::LogDebug( const string& message )
{
	ofstream f;

	f.open( "stm_log.txt", std::ios::app );

	if ( f.good() )
	{
		time_t     now = time( 0 );
		struct tm  tstruct;
		char       buf[256];
		tstruct = *localtime( &now );

		memset( buf, 0, 256 * sizeof( char ) );

		strftime( buf, sizeof( buf ), "%Y-%m-%d, %X", &tstruct );

		f << "[debug]\t";
		f << buf << "\t";
		f << message;
		f << endl;

		f.close();
	}
}

/** @brief LogInfo
  *
  * @todo: document this function
  */
void Logger::LogInfo( const string& message )
{
	ofstream f;

	f.open( "stm_log.txt", std::ios::app );

	if ( f.good() )
	{
		time_t     now = time( 0 );
		struct tm  tstruct;
		char       buf[256];
		tstruct = *localtime( &now );

		memset( buf, 0, 256 * sizeof( char ) );

		strftime( buf, sizeof( buf ), "%Y-%m-%d, %X", &tstruct );

		f << "[info]\t";
		f << buf << "\t";
		f << message;
		f << endl;

		f.close();
	}
}


/** @brief LogError
  *
  * @todo: document this function
  */
void Logger::LogError( const string& message )
{
	ofstream f;

	f.open( "stm_log.txt", std::ios::app );

	if ( f.good() )
	{
		time_t     now = time( 0 );
		struct tm  tstruct;
		char       buf[256];
		tstruct = *localtime( &now );

		memset( buf, 0, 256 * sizeof( char ) );

		strftime( buf, sizeof( buf ), "%Y-%m-%d, %X", &tstruct );

		f << "[error]\t";
		f << buf << "\t";
		f << message;
		f << endl;

		f.close();
	}
}
