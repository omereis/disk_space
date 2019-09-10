#include <stdio.h>
#include <sqlite3.h> 
#include <string>
//-----------------------------------------------------------------------------
using namespace std;
//-----------------------------------------------------------------------------
bool open_database (char *szDB, sqlite3 **db, int argc, char *argv[]);
void create_blobs_table();
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	sqlite3 *db;
	char *zErrMsg = 0;
	char szDB[] = "db_blobs.sqlite";
	int rc;

/*
	rc = sqlite3_open(szDB, &db);
	if( rc ) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return(0);
	}
	else {
		printf ("Database opened\n");
		sqlite3_close(db);
		printf ("Database closed\n");
		//return (0);
	}
*/
	if (open_database (szDB, &db, argc, argv)) {
	//if (open_database (szDB, &db, argc, argv)) {
		printf ("Database opened\n");
		create_blobs_table();
		sqlite3_close(db);
		printf ("Database Closed\n");
	}
/*
	rc = sqlite3_open("test.db", &db);

	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return(0);
	}
	else {
		fprintf(stderr, "Opened database successfully\n");
	}
	sqlite3_close(db);
*/
}
//-----------------------------------------------------------------------------
bool open_database (char *szDB, sqlite3 **db, int argc, char *argv[])
{
	int rc = sqlite3_open(szDB, db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
		return(0);
	}
	else {
		printf ("Database opened\n");
	}
	return (rc == 0);
}
//-----------------------------------------------------------------------------
void create_blobs_table()
{
	const string strTableName = "tbl_blob";
	printf ("table created\n");	
}
//-----------------------------------------------------------------------------

