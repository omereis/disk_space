#include "perr.h"
//-----------------------------------------------------------------------------
void print_sql_error (sql::SQLException &e, const string &strFile, const string &strFunction, int nLine)
{
    print_error (e, strFile, strFunction, nLine);
    cout << " (MySQL error code: " << e.getErrorCode();
    cout << ", SQLState: " << e.getSQLState() << " )" << endl << endl;
}

//-----------------------------------------------------------------------------
void print_error (exception &e, const string &strFile, const string &strFunction, int nLine)
{
    cout << "# ERR: SQLException in " << strFile << endl;
    cout << "(" << strFile << ") on line " << std::to_string (nLine) << endl;
    cout << "# ERR: " << e.what();
}


