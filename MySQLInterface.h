/*

MySQLInterface.h

Copyright (C) 2020 Rafał Frączek

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
 * @file MySQLInterface.h
 * @brief Class definition for the MySQLInterface
 *
 * This file contains the definition of the MySQLInterface class.
 *
 * @author Rafał Frączek
 * @bug No known bugs
 *
 */

#ifndef MYSQLINTERFACE_H
#define MYSQLINTERFACE_H

#include "config.h"

#ifdef use_mysql
#include <cstdint>
#include "mysql.h"

using namespace std;

class MySQLInterface
{

private:

	MYSQL *conn;

	MYSQL_RES *queryresult;

	MYSQL_ROW queryrow;

	uint32_t num_fields;

public:

	MySQLInterface();

	~MySQLInterface();

	bool Connect ( const char *server, const char *user, const char *password, const char *database );

	bool SQLQuery ( const char* query );

	uint32_t GetFieldsCount ( void );

	MYSQL_ROW FetchNextRow ( void );

	bool FreeResult ( void );

};

#endif // use_mysql
#endif // MYSQLINTERFACE_H
