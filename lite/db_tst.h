#ifndef	_DB_TEST_H
#define	_DB_TEST_H
#include <stdio.h>
#include <sqlite3.h>
#include <string>
#include <string.h>
#include <stdlib.h>
//-----------------------------------------------------------------------------
using namespace std;

// prototypes
bool open_database (char *szDB, sqlite3 **db, int argc, char *argv[]);
void create_blobs_table(sqlite3* db);
string get_blobs_table_create_sql ();
bool blob_table_exists(sqlite3 *db);
#endif
