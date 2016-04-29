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
	string database_dir;
	string database_type;
	static map<string, string> table_columns;

	static int callback( void *, int argc, char **argv, char **azColName );

	public:
		DataBaseDriver();
		~DataBaseDriver();

	void set_database_type(const string& _database_type) noexcept;
	void set_database_dir(const string& _database_dir) noexcept;

	const map<string, InterfaceStats> get_stats(const string& _mac,const string& _table,const struct date& _from,const struct date& _to);
};

#endif // DATABASEDRIVER_H
