#include <stdio.h>
#include <sys/statvfs.h>

int main (int argc, char *argv []) {
	struct statvfs st;

	printf ("Block for '/home/one4'\n");
	statvfs ("/home/one4", &st);
	printf("Block size: %ld\n", st.f_bsize);
	printf ("Free blocks: %ld\n", st.f_bfree);
	printf ("Free blocks: %ld K\n", st.f_bfree / 1024);
	printf ("Free blocks: %ld M\n", st.f_bfree / (1024 * 1024));

	printf ("Block for '/home'\n");
	statvfs ("/home", &st);
	printf ("Free blocks: %ld\n", st.f_bfree);
	printf ("Free blocks: %ld K\n", st.f_bfree / 1024);
	printf ("Free blocks: %ld M\n", st.f_bfree / (1024 * 1024));

	printf ("Block for '/'\n");
	statvfs ("/", &st);
	printf ("Free blocks: %ld\n", st.f_bfree);
	printf ("Free blocks: %ld K\n", st.f_bfree / 1024);
	printf ("Free blocks: %ld M\n", st.f_bfree / (1024 * 1024));
	return (0);

}
