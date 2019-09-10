#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/statvfs.h> 
#include <math.h>
#include "fm.h"

#include "get_cli.h"
#include "genfile.h"

//-----------------------------------------------------------------------------
void print_usage();
bool match(const char *pattern, const char *candidate, int p, int c);
void create_files (struct FileMaker *pfm);
size_t get_size(struct FileMaker *pfm);
size_t get_size(struct FileMaker *pfm);
std::string get_file_name (int nFile, int nCount);
void copy_file (const std::string &strSource, const std::string &strDest);

//-----------------------------------------------------------------------------
static const char *szNameBase = "tstfile";

//-----------------------------------------------------------------------------
int main (int argc, char *argv[])
{
	struct FileMaker fm;
	char appname[] = "filetst";
	int nSize;
	printf ("Creating file with givn size\n");
	get_cli_params(&fm, argc, argv, appname);
	print_params(&fm);
	create_files (&fm);
	exit(0);
}
/*
//-----------------------------------------------------------------------------
int get_cli_params(struct FileMaker *pfm,  int argc, char *argv[])
{
	int n, ok, c;
	char str[1024], mult;
	char *szSize, *szCount, *szMultiplier, cMult, *szOut;
	szSize = szCount = szMultiplier = szOut = NULL;

	pfm->count = 10;
	pfm->size = 5;
	pfm->mult = 'M';
	strcpy (pfm->szOutFile, "usage.csv");

	while ((c = getopt (argc, argv, "Hhs:c:m:o:")) != -1) {
		switch (c) {
			case 's':
			case 'S':
				szSize = optarg;
				break;
			case 'c':
			case 'C':
				szCount = optarg;
				break;
			case 'm':
				szMultiplier = optarg;
				break;
			case 'o':
				strcpy (pfm->szOutFile, optarg);
				break;
			case 'h':
			case 'H':
				print_usage();
				exit(0);
			default:
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				fprintf (stderr, "proceeding with defaults\n");
				break;
		}
	}
	if (szSize)
		pfm->size = atoi(szSize);
	if (szCount)
		pfm->count = atoi(szCount);
	if (szMultiplier) {
		if (strlen(szMultiplier) == 1) {
			if (tolower(szMultiplier[0]) == 'm')
				pfm->mult = 'M';
			else if (tolower(szMultiplier[0]) == 'g')
				pfm->mult = 'G';
			else
				pfm->mult = 'K';
		}
	}
	printf("fm.count=%d\n", pfm->count);
	printf("fm.size=%d\n", pfm->size);
	printf("fm.mult=%c\n", pfm->mult);
	printf("Output file: '%s'\n", pfm->szOutFile);
}
//-----------------------------------------------------------------------------
void print_usage()
{
	printf ("Usage:\nmkfiles [-s <size> -c <count> -m <K | M | G>]\n");
}
*/
//-----------------------------------------------------------------------------
//static const char *szNameBase = "tstfile";
//-----------------------------------------------------------------------------
void remove_old ()
{
	DIR *dir;
	struct dirent *item;
	char szPattern[100];
	int n = 0;

	sprintf (szPattern, "%s*", szNameBase);
	dir = opendir (".");
	while ((item = readdir(dir)) != NULL) {
		if (match(szPattern, item->d_name, 0, 0)) {
			remove(item->d_name);
			n++;
			//printf ("  %s\n", item->d_name);
		}
	}
	closedir (dir);
	//printf ("removed %d files\n", n);
}
//-----------------------------------------------------------------------------
bool match(const char *pattern, const char *candidate, int p, int c) {
	if (pattern[p] == '\0') {
		return candidate[c] == '\0';
	}
	else if (pattern[p] == '*') {
		for (; candidate[c] != '\0'; c++) {
			if (match(pattern, candidate, p+1, c))
				return true;
		}
		return match(pattern, candidate, p+1, c);
	}
	else if (pattern[p] != '?' && pattern[p] != candidate[c]) {
		return false;
	}
	else {
		return match(pattern, candidate, p+1, c+1);
	}
}

#include "genfile.h"
//-----------------------------------------------------------------------------
void create_files (struct FileMaker *pfm)
{
	FILE *file, *fOut;
	char szNameFmt[1000];
	int iOuter, nInner, fd, i, nInserts;
	size_t sizeFile, sTotalSize, stBefore, stAfter;
	clock_t cStart;
	double dSeconds;
	std::string strFileName, strSourceFileName ;

	sprintf (szNameFmt, "%s%%0%dd.dat", szNameBase, pfm->count);
	sizeFile = get_size(pfm);
	printf ("sizeFile = %ld\n", sizeFile);
	remove_old ();
	if (sizeFile > 0) {
		strSourceFileName = get_file_name (0, pfm->count);
		//sTotalSize = generate_file (strSourceFileName.c_str(), pfm);
		fOut = fopen (pfm->szOutFile, "w+");
		printf ("results file opened\n");
		fprintf (fOut, "Files,Before,Size,After,Time\n");
		printf ("results file header printed\n");
		printf ("\ncont:%d\n\n", pfm->count);
		for (nInserts=0 ; nInserts < pfm->insert_count ; nInserts++) {
			sTotalSize = generate_file (strSourceFileName.c_str(), pfm);
			sTotalSize = 0;
			for (iOuter=0 ; iOuter < pfm->count ; iOuter++) {
				cStart = clock();
				stBefore = get_free_space();
				//sprintf (str, szNameFmt, iOuter);
				strFileName = get_file_name (iOuter + 1, pfm->count);
				copy_file (strSourceFileName, strFileName);
				sTotalSize += sizeFile;
				//sTotalSize = generate_file (strFileName.c_str(), pfm);
				stAfter = get_free_space();
				//if ((iOuter % 5) == 0)
					fprintf (stderr, "File %s created\r", strFileName.c_str());
				dSeconds = ((double) (clock() - cStart)) / ((double) CLOCKS_PER_SEC);
				fprintf (fOut, "%d,%ld,%ld,%ld,%g\n", nInner, stBefore, sTotalSize, stAfter, dSeconds);
			}
			if (nInserts < (pfm->insert_count - 1)) {
				fprintf (fOut, "\n");
				//fprintf (stderr, "completing insert # %d\n", nInserts + 1);
				printf ("before deleting all, free space: %g ...\n", (double) get_free_space() / (1024.0 * 1024.0));
				//getc(stdin);
				remove_old();
				printf ("after deleting all, free space: %g ...\n", (double) get_free_space() / (1024.0 * 1024.0));
				//getc(stdin);
			}
		}
		fprintf (fOut, "\n");
		fprintf (stderr, "\n");
		if (pfm->del) {
			fprintf (stderr, "Deleting files\n");
			for ( ; iOuter >= 0 ; iOuter--) {
				cStart = clock();
				stBefore = get_free_space();
				strFileName = get_file_name (iOuter + 1, pfm->count);
				remove(strFileName.c_str());
				sTotalSize -= sizeFile;
				stAfter = get_free_space();
				if ((iOuter % 5) == 0)
					fprintf (stderr, "File %s deleted\r", strFileName.c_str());
				dSeconds = ((double) (clock() - cStart)) / ((double) CLOCKS_PER_SEC);
				fprintf (fOut, "%d,%ld,%ld,%ld,%g\n", nInner, stBefore, sTotalSize, stAfter, dSeconds);
			}
			cStart = clock();
			remove(strSourceFileName.c_str());
			dSeconds = ((double) (clock() - cStart)) / ((double) CLOCKS_PER_SEC);
			fprintf (fOut, "Finalize time: %g\n", dSeconds);
		//for ( ; iOuter >= 0)
		}
		fclose (fOut);
	}
    /*delete ptr;*/
	fprintf (stderr, "\nDone\n");
	//remove_old ();
}

//-----------------------------------------------------------------------------
std::string get_file_name (int nFile, int nCount)
{
	char *szBuf, *szNameFmt = new char [strlen(szNameBase) + 5];;
	std::string str;

	sprintf (szNameFmt, "%s%%0%dd.dat", szNameBase, (int) (log10(nCount) + 1));
	szBuf = new char [strlen(szNameFmt) * 2];
	sprintf (szBuf, szNameFmt, nFile);
	str = std::string(szBuf);

	//fprintf (stderr, "format string: '%s'\n", szNameFmt);
	//fprintf (stderr, "Data file name: '%s'\n", str.c_str());
	delete[] szBuf;
	delete szNameFmt;
	//exit (0);
	return (str);
}

/*
//-----------------------------------------------------------------------------
off_t get_free_space ()
{
	struct statvfs vfs;
	statvfs (".", &vfs); 
	return (vfs.f_bfree);
}
*/
//-----------------------------------------------------------------------------
size_t SizeFromMult (char cMult)
{
	size_t s = 1;
	char (c) = tolower(cMult);
	
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
size_t get_size(struct FileMaker *pfm)
{
	size_t sMult = SizeFromMult (pfm->mult);
	
	return (sMult * pfm->size);
}

#include <iostream>
#include <sys/sendfile.h>  // sendfile
#include <fcntl.h>         // open
#include <unistd.h>        // close
#include <sys/stat.h>      // fstat
#include <sys/types.h>     // fstat
#include <ctime>
//-----------------------------------------------------------------------------
void copy_file (const std::string &strSource, const std::string &strDest)
{
	int source = open(strSource.c_str(), O_RDONLY, 0);
    int dest = open(strDest.c_str(), O_WRONLY | O_CREAT , 0644);
	struct stat stat_source;

    fstat(source, &stat_source);

    sendfile(dest, source, 0, stat_source.st_size);

    close(source);
    close(dest);
}

