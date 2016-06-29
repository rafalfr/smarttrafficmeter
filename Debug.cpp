#include "Debug.h"
#include "Utils.h"
#include "Logger.h"


#ifdef __linux
#include <execinfo.h>
#include <unistd.h>
#include "LinuxUtils.h"
#endif // __linux



/** @brief get_backtrace
  *
  * @todo: document this function
  */
string Debug::get_backtrace( void )
{

    string out;

#ifdef _WIN32

#endif // _WIN32

#ifdef __linux
    void* array[64];
    size_t size, i;

    size = backtrace( array, 64 );

#ifndef __pi__

    //we skip the first two entries
    //because they refer to the Debug::get_backtrace
    for ( i = 2; i < size; i++ )
    {
        out += LinuxUtils::resolve( ( unsigned long )array[i] );
        out += "\n";
    }

#endif // __pi__

#ifdef __pi__

    char** strs = backtrace_symbols( array, size );

    for ( i = 0; i < size; ++i )
    {
        out += strs[i];
        out += "\n";
    }

    free( strs );

#endif // __pi__

#endif // __linux

    return Utils::trim( out );

}
