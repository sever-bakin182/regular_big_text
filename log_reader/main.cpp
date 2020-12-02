#include "LogReader.h"

#define SIZE_BUFFER 1024

#define INPUT_VARIABLES 3
#define INDEX_NAME_FILE 1
#define INDEX_FILTER 2

int main(int _argc, char* _argv[])
{
	if (_argc != INPUT_VARIABLES)
	{
		printf("ERROR : no enough input variables.");
		return -1;
	}
	
	CLogReader Reader;

	if (!Reader.Open(_argv[INDEX_NAME_FILE]))
	{
		printf("ERROR : file not opened : %s", _argv[INDEX_NAME_FILE]);
		return -1;
	}

	if (!Reader.SetFilter(_argv[INDEX_FILTER]))
	{
		printf("ERROR : bad filter : %s", _argv[INDEX_FILTER]);
		return -1;
	}

	char buffer[SIZE_BUFFER] = { 0 };
	while (Reader.GetNextLine(buffer, SIZE_BUFFER))
	{
		printf("%s\n",buffer);
	}

	return 0;
}

