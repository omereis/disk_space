#include <stdlib.h>
#include <iostream>
#include <string>

using namespace std;

/* MySQL Connector/C++ specific headers */
#include <driver.h>
#include <connection.h>
#include <statement.h>
#include <prepared_statement.h>
#include <cppconn/resultset.h>
#include <metadata.h>
#include <resultset_metadata.h>
#include <exception.h>
#include <warning.h>

using namespace sql;

void print_sql_error (sql::SQLException &e, const string &strFile, const string &strFunction, int nLine);
void print_error (exception &e, const string &strFile, const string &strFunction, int nLine);

