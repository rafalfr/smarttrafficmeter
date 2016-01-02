#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <bfd.h>

//http://www.opensource.apple.com/source/gdb/gdb-1515/src/binutils/addr2line.c
//https://github.com/dinhviethoa/linux-sample/blob/master/etpan-symbols.c
//https://gist.github.com/jvranish/4441299
//http://stackoverflow.com/questions/8278691/how-to-fix-backtrace-line-number-error-in-c

//http://opensource.apple.com/source/X11libs/X11libs-40.2/cairo/cairo-1.8.6/util/backtrace-symbols.c?txt

//https://panthema.net/2008/0901-stacktrace-demangled/
//http://webcache.googleusercontent.com/search?q=cache:rcg0-9FvfdIJ:https://en.wikibooks.org/wiki/Linux_Applications_Debugging_Techniques/The_call_stack+&cd=3&hl=pl&ct=clnk&gl=pl
//https://github.com/adarqui/darqbot/blob/master/test/how-to-generate-a-stacktrace-when-my-gcc-c-app-crashes
//http://spin.atomicobject.com/2013/01/13/exceptions-stack-traces-c/

using namespace std;

class Debug
{
private:
	static bfd* abfd;
	static asymbol **syms;
	static asection *text;

static string resolve( const long address );


public:

	static string get_backtrace(void);

};

#endif // DEBUG_H
