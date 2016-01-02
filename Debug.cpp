#include "Debug.h"
#include "Utils.h"
#include "Logger.h"
#include <execinfo.h>
#include <unistd.h>


bfd* Debug::abfd = 0;
asymbol ** Debug::syms = 0;
asection* Debug::text = 0;

/** @brief resolve
  *
  * @todo: document this function
  */
string Debug::resolve( const long address )
{
	string out;

	if ( !abfd )
	{
		char ename[1024];
		int l = readlink( "/proc/self/exe", ename, sizeof( ename ) );

		if ( l == -1 )
		{
			Logger::LogError( "Failed to find executable." );
			return out;
		}

		ename[l] = 0;

		bfd_init();

		abfd = bfd_openr( ename, 0 );

		if ( !abfd )
		{
			Logger::LogError( "bfd_openr failed" );
			return out;
		}

		/* oddly, this is required for it to work... */
		bfd_check_format( abfd, bfd_object );

		unsigned storage_needed = bfd_get_symtab_upper_bound( abfd );
		syms = ( asymbol ** ) malloc( storage_needed );
		unsigned cSymbols = bfd_canonicalize_symtab( abfd, syms );

		text = bfd_get_section_by_name( abfd, ".text" );
	}

	long offset = ( ( long )address ) - text->vma;

	if ( offset > 0 )
	{
		const char *file;
		const char *func;
		unsigned line;

		if ( bfd_find_nearest_line( abfd, text, syms, offset, &file, &func, &line ) && file )
		{
			string file_str( file );

			out.clear();
			out += "file: ";
			out += file_str.substr( file_str.rfind( "/" ) + 1, string::npos );
			out += "\t";
			out += "func: ";
			out += func;
			out += "\t";
			out += "line: ";
			out += std::to_string( line );
		}
	}

	return out;
}

/** @brief get_backtrace
  *
  * @todo: document this function
  */
string Debug::get_backtrace( void )
{
	void* array[64];
	size_t size, i;
	string out;

	size = backtrace( array, 64 );


	//we skip the first two entries
	//because they refer to the Debug::get_backtrace
	for ( i = 2; i < size; i++ )
	{
		out += resolve( ( long )array[i] );

		if ( i < ( size - 1 ) )
		{
			out += "\n";
		}
	}

	return Utils::trim( out );
}
