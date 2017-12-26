#include <stdio.h>
#include <dirent.h>

int main()
{
	struct dirent* ent = NULL;
	DIR* p;
	if((p = opendir("/home/ubuntu/Downloads")) == NULL)
	{
		printf("erro\n");
		return 0;
	}
	while((ent = readdir(p)) != NULL)
	{
		printf("%s\n", ent->d_name);
	}
	closedir(p);
}
