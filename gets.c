#include<stdio.h>
#include<string.h>

int main()
{
    int a;
    char s[4096];
    char c;
    scanf("%d", &a);
    scanf("%c", &c);
    memset(s, 0, sizeof(s));
    gets(s);
    printf("%d %s\n", a, s);

}
