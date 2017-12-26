#include<sys/types.h>
#include<sys/stat.h>
int main()
{
	mkdir("hh",S_IRWXU);
	char filename[40];	
	getcwd(filename, 41);
	printf("%s\n", filename);
}
