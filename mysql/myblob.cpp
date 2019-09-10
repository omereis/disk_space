#inclde <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

struct my_connection {
	string server;
	string username;
	string password;
	string database;
};

//-----------------------------------------------------------------------------
MYSQL* mysql_connection_setup(struct connection_details mysql_details)
{
	// first of all create a mysql instance and initialize the variables within
	MYSQL *connection = mysql_init(NULL);

	// connect to the database with the details attached.
	if (!mysql_real_connect(connection,mysql_details.server, mysql_details.user, mysql_details.password, mysql_details.database, 0, NULL, 0)) {
		printf("Conection error : %s\n", mysql_error(connection));
		exit(1);
	}
	return connection;
}

//-----------------------------------------------------------------------------

