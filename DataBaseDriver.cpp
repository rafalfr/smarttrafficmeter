#include "DataBaseDriver.h"

#ifdef use_sqlite
#include "sqlite3.h"

map<string, string> DataBaseDriver::table_columns;

#endif // use_sqlite

/** @brief DataBaseDriver
  *
  * @todo: document this function
  */
DataBaseDriver::DataBaseDriver()
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
const map<string, InterfaceStats> DataBaseDriver::get_daily_stats( const string& _mac, const string& _table, const struct date& _from, const struct date& _to )
{

    map<string, InterfaceStats> results;

    uint64_t rx_bytes;
    uint64_t tx_bytes;

#ifdef use_sqlite
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    string row;
    string query;

    rc = sqlite3_open_v2( ( database_dir + "/" + _mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE, NULL );

    if ( rc != SQLITE_OK )
    {
        return results;
    }

    query.clear();
    query += "SELECT * from " + _table + " ";
    query += "WHERE row>=";
    query += "'";
    query += std::to_string( _from.year ) + "-" + std::to_string( _from.month ) + "-" + std::to_string( _from.day ) + "_" + std::to_string( _from.hour ) + ":00-" + std::to_string( _from.hour + 1 ) + ":00";
    query += "'";
    query += " AND ";
    query += " row<=";
    query += "'";
    query += std::to_string( _to.year ) + "-" + std::to_string( _to.month ) + "-" + std::to_string( _to.day ) + "_" + std::to_string( _to.hour ) + ":00-" + std::to_string( _to.hour + 1 ) + ":00";
    query += "'";
    query += ";";
    table_columns.clear();
    rc = sqlite3_exec( db, query.c_str(), DataBaseDriver::callback, NULL, &zErrMsg );

    if ( rc == SQLITE_OK )
    {
        row = table_columns["row"];

        try
        {
            rx_bytes = std::stoull( table_columns["rx_bytes"] );
        }
        catch ( ... )
        {
            rx_bytes = 0ULL;
        }

        try
        {
            tx_bytes = std::stoull( table_columns["tx_bytes"] );
        }
        catch ( ... )
        {
            tx_bytes = 0ULL;
        }

        InterfaceStats ystats;
        //all_stats[mac]["yearly"][row] = ystats;
        //all_stats[mac]["yearly"][row].set_initial_stats( tx_bytes, rx_bytes );
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

    if ( argc > 0 )
    {
        for ( i = 0; i < argc; i++ )
        {
            table_columns[string( azColName[i] )] = string( argv[i] ? argv[i] : "0" );
        }

        return 0;
    }
    else
    {
        return 1;
    }
}



