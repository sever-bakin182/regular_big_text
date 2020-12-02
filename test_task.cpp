#include "LogReader.h"
#include <iostream>
#define SIZE_BUFFER 1024

bool safe_input(const char* text, char* buf)
{
	printf("%s : ", text);
	buf[0] = 0;
	return EOF != scanf("%s", buf);
}

int main()
{
	char buffer[SIZE_BUFFER];
	char filter[256] = {0};
	char name_file[128] = {0};

	CLogReader Reader;

	if (!safe_input("Enter name file", name_file))
	{
		printf("<error> Enter name file.");
		return 0;
	}

	if (!Reader.Open(name_file))
	{
		printf("<error> file not opened : %s", name_file);
		return 0;
	}

	if (!safe_input("Enter mask", filter))
	{
		printf("<error> Enter filter.");
		return 0;
	}

	if (!Reader.SetFilter(filter))
	{
		printf("<error> bad filter : %s", filter);
		return 0;
	}

	int i = 0, j = 0;
	while (Reader.GetNextLine(buffer, SIZE_BUFFER))
	{
		if (Reader.IsPart())
		{
			if (++j == 1)
			{
				printf("raw%5d : %s", ++i, buffer);
			}
			else
			{
				printf("%s", buffer);
			}		
		}
		else
		{
			if (j > 0)
			{
				j = 0;
				printf("%s\n", buffer);
			}
			else
			{
				printf("raw%5d : %s\n", ++i, buffer);
			}	
		}
	}

	return std::cin.get(), 0;
}

