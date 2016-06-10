#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif


#define use_sqlite
//#define use_mysql

#endif // CONFIG_H_INCLUDED
