#include "DataBaseDriver.h"
#include "Logger.h"
#ifdef use_sqlite
#include "sqlite3.h"

vector < map<string, string> > DataBaseDriver::query_results;

#endif // use_sqlite

/** @brief DataBaseDriver
  *
  * @todo: document this function
  */
DataBaseDriver::DataBaseDriver() : database_dir( "" ), database_type( "" )
{

}

/** @brief ~DataBaseDriver
  *
  * @todo: document this function
  */
DataBaseDriver::~DataBaseDriver()
{

}

/** @brief set_database
  *
  * @todo: document this function
  */
void DataBaseDriver::set_database_type( const string& _database_type ) noexcept
{
    database_type.clear();
    database_type.append( _database_type );
}

/** @brief set_database_dir
  *
  * @todo: document this function
  */
void DataBaseDriver::set_database_dir( const string& _database_dir ) noexcept
{
    database_dir.clear();
    database_dir.append( _database_dir );
}



/** @brief get_daily_stats
  *
  * @todo: document this function
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
        return results;
    }

    Logger::LogInfo( "database opened" );


    string from;
    string to;

    if ( _table.compare( "hourly" ) == 0 )
    {
        from = std::to_string( _from.year ) + "-" + std::to_string( _from.month ) + "-" + std::to_string( _from.day ) + "_" + std::to_string( _from.hour ) + ":00-" + std::to_string( _from.hour + 1 ) + ":00";
        to = std::to_string( _to.year ) + "-" + std::to_string( _to.month ) + "-" + std::to_string( _to.day ) + "_" + std::to_string( _to.hour ) + ":00-" + std::to_string( _to.hour + 1 ) + ":00";
    }
    else if ( _table.compare( "daily" ) == 0 )
    {
        from = std::to_string( _from.year ) + "-" + std::to_string( _from.month ) + "-" + std::to_string( _from.day );
        to = std::to_string( _to.year ) + "-" + std::to_string( _to.month ) + "-" + std::to_string( _to.day );
    }
    else if ( _table.compare( "monthly" ) == 0 )
    {
        from = std::to_string( _from.year ) + "-" + std::to_string( _from.month );
        to = std::to_string( _to.year ) + "-" + std::to_string( _to.month );
    }
    else if ( _table.compare( "yearly" ) == 0 )
    {
        from = std::to_string( _from.year );
        to = std::to_string( _to.year );
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
    Logger::LogInfo( query );
    query_results.clear();
    rc = sqlite3_exec( db, query.c_str(), DataBaseDriver::callback, nullptr, &zErrMsg );

    if ( rc == SQLITE_OK )
    {
        for ( auto & result : query_results )
        {

            row = result["row"];

            try
            {
                rx_bytes = std::stoull( result["rx_bytes"] );
            }
            catch ( ... )
            {
                rx_bytes = 0ULL;
            }

            try
            {
                tx_bytes = std::stoull( result["tx_bytes"] );
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

#endif // use_sqlite


    return results;

}

/** @brief callback
  *
  * @todo: document this function
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



