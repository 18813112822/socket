#include<stdio.h>
int main()
{
    char a[10] = "123456789";
    char b[4];
    char c[7];
    memset(b, 0, sizeof(b));
    strncpy(b, a, sizeof(b)-1);
    strcpy(c, a+3);
    printf("%s\n", b);
    printf("%s\n", c);
}
