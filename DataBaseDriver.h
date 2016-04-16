#ifndef DATABASEDRIVER_H
#define DATABASEDRIVER_H
#include <map>
#include <string>
#include "defines.h"
#include "InterfaceStats.h"

using namespace std;

class DataBaseDriver
{
private:
	string database;

	public:
		/** Default constructor */
		DataBaseDriver();
		/** Default destructor */
		~DataBaseDriver();

	void set_database(const string& _database) noexcept;

	const map<string, InterfaceStats> get_daily_stats(const string& _mac,const string& _table,const struct date& _from,const struct date& _to);
};

#endif // DATABASEDRIVER_H
