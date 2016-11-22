/*

DataBaseDriver.cpp

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

#include "config.h"
#include "DataBaseDriver.h"
#include "Logger.h"
#include "Utils.h"

#ifdef use_sqlite
#include "sqlite3.h"

/**< vector containing results of database query */
vector < map<string, string> > DataBaseDriver::query_results;

#endif // use_sqlite

/** @brief DataBaseDriver
  *
  * default constructor
  *
  * @param None
  * @return None
  *
  */
DataBaseDriver::DataBaseDriver() : database_dir( "" ), database_type( "" )
{
}

/** @brief ~DataBaseDriver
  *
  * default destructor
  *
  * @param None
  * @return None
  *
  */
DataBaseDriver::~DataBaseDriver()
{
}

/** @brief set_database
  *
  * This method sets the default database type.
  *
  * @param string with data base type
  * @return void
  *
  */
void DataBaseDriver::set_database_type( const string& _database_type ) noexcept
{
    database_type.clear();
    database_type.append( _database_type );
}

/** @brief set_database_dir
  *
  * This method sets database directory
  *
  * @param string with full path to the directory where database is stored
  * @return void
  *
  */
void DataBaseDriver::set_database_dir( const string& _database_dir ) noexcept
{
    database_dir.clear();
    database_dir.append( _database_dir );
}

/** @brief get_stats
  *
  * The function returns stats for a given network interface and time period
  *
  * @param mac address
  * @param table name (hourly, daily, monthly, yearly)
  * @param start date
  * @param end date
  *
  */
const map<string, InterfaceStats> DataBaseDriver::get_stats( const string& _mac, const string& _table, const struct date& _from, const struct date& _to )
{

    map<string, InterfaceStats> results;

    uint64_t rx_bytes;
    uint64_t tx_bytes;

#ifdef use_sqlite
    sqlite3 *db;
    char *zErrMsg = nullptr;
    int rc;
    string row;
    string query;

    rc = sqlite3_open_v2( ( database_dir + "/" + _mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE, nullptr );

    if ( rc != SQLITE_OK )
    {
        Logger::LogDebug( "can't open the database" );
        sqlite3_close_v2(db);
        return results;
    }

    string from;
    string to;

    if ( _table.compare( "hourly" ) == 0 )
    {
        from = Utils::to_string( _from.year ) + "-" + Utils::to_string( _from.month, 2 ) + "-" + Utils::to_string( _from.day, 2 ) + "_" + Utils::to_string( _from.hour, 2 ) + ":00-" + Utils::to_string( _from.hour + 1, 2 ) + ":00";
        to = Utils::to_string( _to.year ) + "-" + Utils::to_string( _to.month, 2 ) + "-" + Utils::to_string( _to.day, 2 ) + "_" + Utils::to_string( _to.hour, 2 ) + ":00-" + Utils::to_string( _to.hour + 1, 2 ) + ":00";
    }
    else if ( _table.compare( "daily" ) == 0 )
    {
        from = Utils::to_string( _from.year ) + "-" + Utils::to_string( _from.month, 2 ) + "-" + Utils::to_string( _from.day, 2 );
        to = Utils::to_string( _to.year ) + "-" + Utils::to_string( _to.month, 2 ) + "-" + Utils::to_string( _to.day, 2 );
    }
    else if ( _table.compare( "monthly" ) == 0 )
    {
        from = Utils::to_string( _from.year ) + "-" + Utils::to_string( _from.month, 2 );
        to = Utils::to_string( _to.year ) + "-" + Utils::to_string( _to.month, 2 );
    }
    else if ( _table.compare( "yearly" ) == 0 )
    {
        from = Utils::to_string( _from.year );
        to = Utils::to_string( _to.year );
    }

    query.clear();
    query += "SELECT * from " + _table + " ";
    query += "WHERE row>=";
    query += "'";
    query += from;
    query += "'";
    query += " AND ";
    query += " row<=";
    query += "'";
    query += to;
    query += "'";
    query += ";";
    query_results.clear();
    rc = sqlite3_exec( db, query.c_str(), DataBaseDriver::callback, nullptr, &zErrMsg );

    if ( rc == SQLITE_OK )
    {
        for ( auto & result : query_results )
        {

            row = result["row"];

            try
            {
                rx_bytes = Utils::stoull( result["rx_bytes"] );
            }
            catch ( ... )
            {
                rx_bytes = 0ULL;
            }

            try
            {
                tx_bytes = Utils::stoull( result["tx_bytes"] );
            }
            catch ( ... )
            {
                tx_bytes = 0ULL;
            }

            InterfaceStats stats;
            results[row] = stats;
            results[row].set_initial_stats( tx_bytes, rx_bytes );
        }
    }
    else
    {
        Logger::LogDebug( "sqlite3_exec error" );
    }

    sqlite3_close_v2(db);

#endif // use_sqlite

    return results;
}

/** @brief callback
  *
  * This method is a callback function that is used by the underlying
  * database driver (e.g. sqlite)
  * The function is not directly used in other program files.
  *
  */
int DataBaseDriver::callback( void*, int argc, char** argv, char** azColName )
{
    int i;

    map <string, string> columns;

    if ( argc > 0 )
    {
        for ( i = 0; i < argc; i++ )
        {
            columns[string( azColName[i] )] = string( argv[i] ? argv[i] : "0" );
        }

        query_results.push_back( columns );

        return 0;
    }
    else
    {
        return 1;
    }
}
