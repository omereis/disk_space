#include <stdio.h>
#include <sqlite3.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <cstddef>
#include <cwchar>
#include <exception>

#include "db_tst.h"
#include "fm.h"
#include "get_cli.h" 
#include "genfile.h" 
//-----------------------------------------------------------------------------
//using namespace std;
//-----------------------------------------------------------------------------

extern char *szNameBase;

void insert_items (sqlite3 *db, struct FileMaker *pfmi, char szName[]);
void delete_current_items (sqlite3 *db, struct FileMaker *pfm);
bool file_exists (const char szName[]);
void gen_one_file (char szName[], struct FileMaker *pfm);
char *read_file (char szName[], int &length);
void insert_item (sqlite3 *db, int n);
void remove_item (sqlite3 *db, int n);

//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    sqlite3 *db;
    char *zErrMsg = 0;
    char szDB[] = "db_blobs.sqlite";
	struct FileMaker fm;
	char *szName = new char[strlen(szNameBase) * 2];

	sprintf (szName, "%s.dat", szNameBase);
    if (open_database (szDB, &db, argc, argv)) {
        printf ("Database opened\n");
		create_blobs_table(db);
		printf ("  Table created\n");
		get_cli_params(&fm, argc, argv, (char*) "test_blob"); 
		printf ("Read command line parameters:\n\n");
		print_params (&fm);
		printf ("-----------------------------------\n\n");
		gen_one_file (szName, &fm);
		delete_current_items (db, &fm);
		printf ("  Old items deleted\n");
		insert_items(db, &fm, szName);
		printf ("  New items Added\n");
        sqlite3_close(db);
        printf ("Database Closed\n");
		remove (szName);
		printf ("Data file deleted\n");
    }
	delete[] szName;
}

//-----------------------------------------------------------------------------
void gen_one_file (char szName[], struct FileMaker *pfm)
{
	int nCount = pfm->count;

	pfm->count = 1;
	generate_file (szName, pfm);
	pfm->count = nCount;
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

static bool s_fBlobTableExists = false;
static const string strBlobTable = "t_blob";

//static const string strSqlCreateTable =
static const char szSqlCreateTable[] =
	"CREATE TABLE `%s` ("
	//"CREATE TABLE `t_blob` ("
		"`id`	INTEGER NOT NULL,"
		"`file_name`	TEXT,"
		"`file`	BLOB,"
		"PRIMARY KEY(`id`)"
	");"
	;

//-----------------------------------------------------------------------------
static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);
   
   for(i = 0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   
   printf("\n");
   return 0;
}

/*
//-----------------------------------------------------------------------------
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;

	for(i = 0; i<argc; i++)
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	printf("\n");
	return 0;
}
*/

//-----------------------------------------------------------------------------
void create_blobs_table (sqlite3 *db)
{
	char *zErrMsg = 0;

	if (!blob_table_exists(db)) {
		string strSqlCreateTable = get_blobs_table_create_sql ();
		int rc = sqlite3_exec(db, strSqlCreateTable.c_str(), callback, 0, &zErrMsg);
		if (rc != SQLITE_OK ){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
	}
}

bool s_fTableExists;
//-----------------------------------------------------------------------------
static int cbCreate(void *NotUsed, int argc, char **argv, char **azColName)
{
	s_fTableExists = true;
	return 0;
	int i;
	for(i = 0; i<argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	printf("\n");
	return 0;
}

//-----------------------------------------------------------------------------
bool blob_table_exists(sqlite3 *db)
{
	char *zErrMsg = 0;

	s_fTableExists = false;
    string strSql = "SELECT name FROM sqlite_master WHERE type='table' AND name='" + strBlobTable + "';";
    int rc = sqlite3_exec(db, strSql.c_str(), cbCreate, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(db);
	} else {
		fprintf(stdout, "Table created successfully\n");
	}
	return (s_fTableExists);
}

//-----------------------------------------------------------------------------
string get_blobs_table_create_sql ()
{
	char *szBuf;
	string str;

	szBuf = new char [strlen(szSqlCreateTable) + strBlobTable.length()];
	sprintf (szBuf, szSqlCreateTable, strBlobTable.c_str());
	str = string(szBuf);
	delete[] szBuf;
	return (str);
}

//-----------------------------------------------------------------------------
static int callbackInsert (void *data, int argc, char **argv, char **azColName)
{
	int i;
	fprintf(stderr, "Insert callback\nbackn%s: ", (const char*)data);

	for(i = 0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}

	printf("\n");
	return 0;
}

void insert_items(sqlite3 *db, struct FileMaker *pfm, char szName[])
//-----------------------------------------------------------------------------
{
	int n, rc, length;
	string strSql;
	char *zErrMsg = 0, *pData;
	FILE *fResults;
	sqlite3_stmt *ppStmt;
	const char  **pzTail;

	fResults = fopen (pfm->szOutFile, "w+");
	fprintf (fResults, "Number,Space\n");
	pData = read_file (szName, length);
	printf ("file read\n");
	printf ("read for %d items\n\n", pfm->count);
	for (n=0 ; n < pfm->count ; n++) {
		//insert_item (db, n);
/**/
		strSql = "insert into " + strBlobTable + "(id,file) values (" + std::to_string(n+1) + ", ?);";
		rc = sqlite3_prepare_v2(db, strSql.c_str(), -1, &ppStmt, NULL);
		if (rc != SQLITE_OK) {
			fprintf (stderr, "Error:\n%s\n", sqlite3_errmsg(db));
			exit (-1);
		}
		if(ppStmt) {
			sqlite3_bind_blob(ppStmt, 1, pData, length, SQLITE_TRANSIENT);
			sqlite3_step(ppStmt);
			sqlite3_finalize(ppStmt);
			sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);
		}
/**/
		if (((n+1) % 5) == 0)
			fprintf (stderr, "%d items inserted\r", (n + 1));
		fprintf (fResults, "%d,%ld\n", n+1, get_free_space());
	}
	fclose (fResults);
	delete (pData);
}

//-----------------------------------------------------------------------------
void insert_items_new(sqlite3 *db, struct FileMaker *pfm, char szName[])
{
	int n, rc, length;
	string strSql;
	char *zErrMsg = 0, *pData;
	FILE *fResults;
	sqlite3_stmt *ppStmt;
	const char  **pzTail;

	fResults = fopen (pfm->szOutFile, "w+");
	fprintf (fResults, "Number,Space\n");
	pData = read_file (szName, length);
	printf ("file read\n");
	printf ("read for %d items\n\n", pfm->count);
	for (n=0 ; n < pfm->count ; n++) {
		insert_item (db, n);
		if (((n+1) % 5) == 0)
			fprintf (stderr, "%d items inserted\r", (n + 1));
		fprintf (fResults, "%d,%ld\n", n+1, get_free_space());
	}
/*
	for (n=pfm->count  ; n > 0 ; n--) {
		remove_item (db, n);
		if (((n+1) % 5) == 0)
			fprintf (stderr, "%d items remained\r", (n + 1));
		fprintf (fResults, "%d,%ld\n", n+1, get_free_space());
	}
*/
	fclose (fResults);
	delete (pData);
}

//-----------------------------------------------------------------------------
void insert_item (sqlite3 *db, int n)
{
	int rc, length;
	string strSql;
	char *zErrMsg = 0, *pData;
	FILE *fResults;
	sqlite3_stmt *ppStmt;
	const char  **pzTail;

	strSql = "insert into " + strBlobTable + "(id,file) values (" + std::to_string(n+1) + ", ?);";
	rc = sqlite3_prepare_v2(db, strSql.c_str(), -1, &ppStmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf (stderr, "Error:\n%s\n", sqlite3_errmsg(db));
		exit (-1);
	}
	if(ppStmt) {
		sqlite3_bind_blob(ppStmt, 1, pData, length, SQLITE_TRANSIENT);
		sqlite3_step(ppStmt);
		sqlite3_finalize(ppStmt);
		sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);
	}
}
//-----------------------------------------------------------------------------
void remove_item (sqlite3 *db, int n)
{
	string strSql;
	char *szErrMsg = 0;

	strSql = "delete from " + strBlobTable + " where (id = " + std::to_string (n) + ");";
	int rc = sqlite3_exec (db, strSql.c_str(), callbackInsert, NULL, &szErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Database error: %s\n", szErrMsg);
		sqlite3_free(szErrMsg);
		sqlite3_close(db);
		exit(-1);
	}
	sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);
}

//-----------------------------------------------------------------------------
void delete_current_items (sqlite3 *db, struct FileMaker *pfm)
{
    int n, rc;
    string strSql;
    char *szErrMsg = 0;

	//for (n=0 ; n < pfm->count ; n++) {
        //strSql = "delete from " + strBlobTable + " where (id= " + std::to_string(n+1) + ");";
        strSql = "delete from " + strBlobTable + ";";
        rc = sqlite3_exec(db, strSql.c_str(), callbackInsert, 0, &szErrMsg);
        if (rc != SQLITE_OK ) {
            fprintf(stderr, "SQL error: %s\n", szErrMsg);
            sqlite3_free(szErrMsg);
        }
    //}
}

#include <fstream>
//-----------------------------------------------------------------------------
bool file_exists (const char szName[])
{
	std::ifstream ifile (szName);
	bool f = (bool) ifile;
	if (f)
		printf ("File %s exists\n", szName);
	return ((bool) ifile);
}

//-----------------------------------------------------------------------------
std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
//-----------------------------------------------------------------------------
char *read_file (char szName[], int &length)
{
	float rSize = (float) filesize(szName);
	length = (int) filesize(szName);

	char *pData = new char [length];
	int fd = open (szName, S_IRUSR);
	int read_bytes = read (fd, pData, length);
	close(fd);

	printf ("file %s size is %g\n", szName, rSize);
	printf ("%d bytes read, %d bytes in file\n", read_bytes, length);
	return (pData);
}
