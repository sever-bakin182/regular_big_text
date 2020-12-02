#pragma once

#include <stdio.h>
#include <stdint.h>

//тип для размера файла
typedef unsigned long long size_f;

class CLogReader
{
public:
			CLogReader();
			~CLogReader();
	bool	Open( const char *namefile );
	void	Close();
	bool	SetFilter(const char *filter);
	bool	GetNextLine(char *buf,
						const int bufsize);

	bool    IsPart() const;

private:
	//эти функции можно было вынести и в .cpp
	bool    ReadFile();
	bool    FindSetEnd();
	bool    CheckStr(const char* str,
					 const char* sub);
	bool    StrCopy(char *buf,
		            const int bufsize);
	void    GetLine();

private:
	FILE*        m_file;         //дескриптор файла
	size_f       m_file_size;    //длина файла

	char*        m_data_file;    //буфер данных (часть файла)
	uint32_t     m_data_size;    //длина данных

	char*        m_start;        //начало строки
	char*        m_end;          //конец строки
	bool         m_part;         //флаг, который определяет часть ли строки

	char*        m_filter;       //маска
};

