#ifdef use_mysql
#include "MySQLInterface.h"
#include <iostream>


MySQLInterface::MySQLInterface()
{
	conn = NULL;
	queryresult = NULL;
	queryrow = NULL;
	num_fields = 0;
}

MySQLInterface::~MySQLInterface()
{
	if ( conn != NULL )
	{
		mysql_close( conn );
	}

	mysql_library_end();
}



bool MySQLInterface::Connect( const char* server, const char* user, const char* password, const char* database )
{
	conn = mysql_init( NULL );

	if ( conn == NULL )
	{
		cout << __FILE__ << " " << __LINE__ << endl;
		return false;
	}

	if ( mysql_real_connect( conn, server, user, password, NULL, 0, NULL, 0 ) != NULL )
	{
		string query( "CREATE DATABASE IF NOT EXISTS " );
		query.append( database );

		mysql_query( conn, query.c_str() );
		queryresult = mysql_store_result( conn );
		mysql_free_result( queryresult );
	}
	else
	{
		cout << mysql_error( conn ) << endl;
		mysql_close( conn );
		conn = NULL;
		return false;
	}
}

bool MySQLInterface::SQLQuery( const char* query )
{
	if ( conn == NULL )
	{
		return false;
	}

	mysql_query( conn, query );

	if ( ( queryresult = mysql_store_result( conn ) )==NULL )
	{
		cout<<"queryresult"<<endl;
		return false;
	}

	num_fields = mysql_num_fields( queryresult );

	return true;
}

uint32_t MySQLInterface::GetFieldsCount( void )
{
	return num_fields;
}

MYSQL_ROW MySQLInterface::FetchNextRow( void )
{
	if ( queryresult != NULL )
	{
		return mysql_fetch_row( queryresult );
	}
	else
	{
		return NULL;
	}
}

bool MySQLInterface::FreeResult( void )
{
	if ( queryresult != NULL )
	{
		mysql_free_result( queryresult );
	}

	return true;
}
#endif
