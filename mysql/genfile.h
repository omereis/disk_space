#ifndef	_GENFILE_H
#define	_GINFILE_H

#include <string>
#include <string.h>
#include "fm.h"
//-----------------------------------------------------------------------------
size_t generate_file (const char szName[], struct FileMaker *pfm);
std::string gen_one_file (const char szName[], struct FileMaker *pfm);

#endif
