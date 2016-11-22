/*

config.h

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
 * @file config.h
 * @brief Global definitions and constants
 *
 * This file contains the global definitions
 * used in the program.
 *
 * @author Rafał Frączek
 * @bug No known bugs
 *
 */

#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include "version.h"

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR "\\"
#define PATH_SEPARATOR_CAHR '\\'
#else
#define PATH_SEPARATOR "/"
#define PATH_SEPARATOR_CAHR '/'
#endif

/* Bit-mask values for 'flags' argument of becomeDaemon() */

#define BD_NO_CHDIR           01    /* Don't chdir("/") */
#define BD_NO_CLOSE_FILES     02    /* Don't close all open files */
#define BD_NO_REOPEN_STD_FDS  04    /* Don't reopen stdin, stdout, and
                                       stderr to /dev/null */
#define BD_NO_UMASK0         010    /* Don't do a umask(0) */

#define BD_MAX_CLOSE  8192          /* Maximum file descriptors to close if
                                       sysconf(_SC_OPEN_MAX) is indeterminate */

#define use_sqlite
//#define use_mysql

//#define _NO_WEBSERVER

#define PACKAGE "SmartTrafficMeter"
#define PACKAGE_VERSION FULLVERSION_STRING

#define BOOST_COROUTINE_NO_DEPRECATION_WARNING

#endif // CONFIG_H_INCLUDED
