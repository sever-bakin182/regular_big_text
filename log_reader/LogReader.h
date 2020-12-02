#pragma once

#include <stdio.h>

const size_t MAXSIZE_DATA = 4096;
const size_t MAXSIZE_LINE = 1024;

class CLogReader
{
public:
			CLogReader();
			~CLogReader();
	bool	Open( const char *namefile );
	void	Close();
	bool	SetFilter(const char *filter);
	bool	GetNextLine(char *buf
					   ,const int bufsize);
private:

	bool    ReadFile(); 

	void    StrCopy(char *buf,
		            const int bufsize);
	bool    NextLine();                       //get a next line
	size_t  SetEndLine();                     //set a end line
	bool    SetNextStart(const size_t shift); //set start of a next line

private:
	FILE*        m_file;                      //descriptor of a file
	bool         m_isRead;                    //status reading 
	bool         m_isEOF;                     //status end of a file

	char         m_data[MAXSIZE_DATA];        //data of a file that read
	char *       m_dataEnd;                   //end point of a data

	char *       m_strStart;                  //start of a string
	char *       m_strEnd;                    //end of a string
	char *       m_strNext;                   //next start of a string

	char         m_filter[MAXSIZE_LINE];      //buffer for a filter
};

