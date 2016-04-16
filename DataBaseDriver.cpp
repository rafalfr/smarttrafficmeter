#include "DataBaseDriver.h"

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
void DataBaseDriver::set_database(const string& _database) noexcept
{
	database.clear();
	database.append(_database);
}

/** @brief get_daily_stats
  *
  * @todo: document this function
  */
const map<string, InterfaceStats> DataBaseDriver::get_daily_stats(const string& _mac,const string& _table,const struct date& _from, const struct date& _to)
{

}

