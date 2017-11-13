/*

MySQLInterface.cpp

Copyright (C) 2017 Rafał Frączek

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
