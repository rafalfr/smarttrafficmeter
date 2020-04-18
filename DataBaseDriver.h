/*

DataBaseDriver.h

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
 * @file DataBaseDriver.h
 * @brief Class definition for the DatabaseDriver
 *
 * This file contains the definition of the DatabaseDriver class.
 *
 * @author Rafał Frączek
 * @bug No known bugs
 *
 */

#ifndef DATABASEDRIVER_H
#define DATABASEDRIVER_H
#include <map>
#include <vector>
#include <string>
#include "defines.h"
#include "InterfaceStats.h"

using namespace std;

class DataBaseDriver
{
private:
	string database_dir;
	string database_type;
	static vector < map<string, string> > query_results;

	static int callback( void *, int argc, char **argv, char **azColName );

	public:
		DataBaseDriver();
		~DataBaseDriver();

	void set_database_type(const string& _database_type) noexcept;
	void set_database_dir(const string& _database_dir) noexcept;

	const map<string, InterfaceStats> get_stats(const string& _mac,const string& _table,const struct date& _from,const struct date& _to);
};

#endif // DATABASEDRIVER_H
