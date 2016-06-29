#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

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

#endif // CONFIG_H_INCLUDED
