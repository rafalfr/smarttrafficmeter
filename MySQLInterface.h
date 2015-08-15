#ifndef MYSQLINTERFACE_H
#define MYSQLINTERFACE_H
#include "mysql.h"


//http://www.tol.it/doc/MySQL/chapter4.html
//http://dev.mysql.com/doc/refman/5.0/en/mysql-fetch-row.html
//libmysqlclient-dev

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

#endif // MYSQLINTERFACE_H
