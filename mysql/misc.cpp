#include  "misc.h"

#include <fstream>
//-----------------------------------------------------------------------------
std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

//-----------------------------------------------------------------------------
char *read_file (const char szName[], int &length)
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

