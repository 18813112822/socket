#include<stdio.h>
#include<string.h>

int main()
{
    int a;
    char s[4096];
    char c;
    memset(s, 0, sizeof(s));
    gets(s);
    printf("%s\n", s);
    memset(s, 0, sizeof(s));
    gets(s);
    printf("%s\n", s);
}
