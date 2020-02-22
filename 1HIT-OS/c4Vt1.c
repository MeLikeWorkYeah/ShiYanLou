#include <stdio.h>
#include <stdarg.h>

void var_test(char* format,...)
{
	va_list list;
	va_start(list, format);

	char* ch;
	while(1)
	{
		ch = va_arg(list,char*);

		if(strcmp(ch,"")==0)
		{
			printf("\n");
			break;
		}
		printf("%s",ch);
	}
	va_end(list);
}

int main(void)
{
	var_test("test","this","is","a","test","");
	return 0;
}
