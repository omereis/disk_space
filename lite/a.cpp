#include <stdio.h>
#include "get_cli.h"

int main ()
{
	printf ("Free space: %ld M bytes\n", get_free_space() / (1024 * 1024));
	return (0);
}

