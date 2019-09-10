#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

long GetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

int main ()
{
	printf ("File size: %ld\n", GetFileSize ("tstfile.dat"));
	return (0);
}
