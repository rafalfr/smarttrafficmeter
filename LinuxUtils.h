#ifndef LINUXUTILS_H
#define LINUXUTILS_H

#ifdef __linux

#include <map>
#include <string>
#include "InterfaceInfo.h"

#ifndef __pi__

#include <bfd.h>

#endif // __pi__

class LinuxUtils
{

private:

#ifndef __pi__

	static bfd* abfd;
	static asymbol **syms;
	static asection *text;

#endif // __pi__

public:

    static void * MeterThread( void );

    static void signal_handler( int signal );

    static int BecomeDaemon( int flags );

    static std::string get_mac( char* name );

    static std::map<std::string, InterfaceInfo> get_all_interfaces( void );

    static bool check_one_instance( void );

    static void set_signals_handler(void);

    static string resolve( const unsigned long address );

    static string get_get_program_path(const char* argv0);

    static void get_user_host(string& user,string& host);

    static void get_os_info(string& os_info);

};

#endif // __linux

#endif // LINUXUTILS_H
