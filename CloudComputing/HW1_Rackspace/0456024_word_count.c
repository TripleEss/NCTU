#include<stdio.h>
#include<string.h>

int main()
{
	char str[1000];
	scanf("%[^\n]s",str);

	int word_count=0;
 	char *pch=strtok(str," \t");
 	while(pch!=NULL)
    {
        word_count++;
        pch=strtok(NULL," \t");
    }

    printf("%d\n",word_count);

	return 0;
}
