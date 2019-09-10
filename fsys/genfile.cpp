#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <string.h>
#include <string>

using namespace std;

#include "genfile.h"
//#include "fm.h"
//-----------------------------------------------------------------------------
size_t get_file_size(struct FileMaker *pfm);
size_t size_from_mult (char cMult);
//-----------------------------------------------------------------------------
static const int s_nDataBufSize = 1024;
const char *szNameBase = "tstfile";

//-----------------------------------------------------------------------------
size_t generate_file (const char szName[], struct FileMaker *pfm)
{
	size_t i, sizeFile;
	FILE *fOut;
	int n;
	char *pData;

	pData = new char[s_nDataBufSize + 100];
	sizeFile = get_file_size(pfm);
	for (n=0 ; n < pfm->count ; n++) {
		//fOut = fopen (szFileName, "w+b");
		fOut = fopen (szName, "w+b");
		for (i=0 ; i < sizeFile ; i++)
			fwrite(pData, 1, s_nDataBufSize, fOut);
		fflush(fOut);
		fclose (fOut);
	}
	delete [] pData;
	//delete[] szFileName;
	//printf ("data deleted\n");
	return (sizeFile);
}

//-----------------------------------------------------------------------------
size_t get_file_size(struct FileMaker *pfm)
{
	size_t sMult = size_from_mult (pfm->mult);
	
	return (sMult * pfm->size);
} 
//-----------------------------------------------------------------------------
size_t size_from_mult (char cMult)
{
	size_t s = 1;
	char c;

	c = tolower(cMult);
	if (c == 'k') // Kilo
		s = 1;
	else if (c == 'm') // Mega
		s = 1024;
	else if (c == 'g') // giga
		s = 1024 * 1024;
	else
		s = 0;
	return (s);
}

//-----------------------------------------------------------------------------
string gen_one_file (const char szName[], struct FileMaker *pfm)
{
	int nCount = pfm->count;
	char *szFileName;
	string strFileName;
	
	if (strlen(szName) > 0) {
		szFileName = new char [strlen(szName) + 1];
		strcpy (szFileName, szName);
	}
	else {
		szFileName = new char [strlen(szNameBase) + 5];
		sprintf (szFileName, "%s.dat", szNameBase);
	}
	pfm->count = 1;
	generate_file (szFileName, pfm);
	pfm->count = nCount;
	strFileName = string(szFileName);
	delete[] szFileName;
	return (strFileName);
}

//-----------------------------------------------------------------------------
