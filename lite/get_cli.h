#ifndef	_GET_CLI_H
#define	_GET_CLI_H

#include <stdio.h>
#include <sqlite3.h>
#include <string>
#include <string.h>
#include <stdlib.h>
//#include "db_tst.h"
#include "fm.h"
//-----------------------------------------------------------------------------
void get_cli_params(struct FileMaker *pfm,  int argc, char *argv[], char szAppName[]);
void print_params(struct FileMaker *pfm);
off_t get_free_space ();
#endif
