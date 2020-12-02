#include "LogReader.h"

#include <malloc.h>
#include <string.h>
#include <io.h>
#include <iostream>

const uint32_t DATA_SIZE = 20480;

bool isdigit__(int c)
{
	return (c >= '0' && c <= '9' ? true : false);
}

CLogReader::CLogReader()
	:m_file(NULL)
	,m_file_size(0)
	,m_data_file(NULL)
	,m_data_size(0)
	,m_start(NULL)
	,m_end(NULL)
	,m_part(false)
	,m_filter(NULL)
{
	m_data_file = (char*)malloc(DATA_SIZE + 1);
}

CLogReader::~CLogReader()
{
	Close();

	if (m_filter != NULL)
	{
		free(m_filter);
		m_filter = NULL;
	}
	if (m_data_file != NULL)
	{
		free(m_data_file);
		m_data_file = NULL;
	}
}

bool CLogReader::Open(const char *namefile )
{
	bool result = false;

	Close();

	m_file = fopen(namefile, "rb");
	if (m_file == NULL)
	{
		return result;
	}

	//получим длину файла
	m_file_size = _filelengthi64(_fileno(m_file));

	if (m_file_size != ((size_f)-1))
	{
		result = true;
	}

	return result;
}

void CLogReader::Close()
{
	if (m_file != NULL)
	{
		fclose(m_file);
		m_file = NULL;
	}
}

bool CLogReader::SetFilter(const char *filter)
{
	bool         result = false;
	const char * state = filter;

	if (filter == NULL || !strlen(filter))
	{
		return result;
	}

	if ((m_filter = (char*)malloc(sizeof(char) * (strlen(filter) + 1))) == NULL)
	{
		return result;
	}

	int i = 0;
	m_filter[i++] = *state;
	while (*++state != '\0')
	{
		switch (*state)
		{
		case '*':
		{
			if (m_filter[i - 1] == '*') break;

			while ( i > 0 && m_filter[i - 1] == '?')
			{
				--i;
			}

			m_filter[i++] = *state;
			break;
		}
		case '?':
		{
			if (m_filter[i - 1] == '*') break;

			m_filter[i++] = *state;
			break;
		}

		default:
			m_filter[i++] = *state;
			break;
		}
	}

	m_filter[i] = '\0';
	result = true;
	return result;
}

bool CLogReader::GetNextLine(char *buf, const int bufsize)
{
	if ((m_filter == NULL) || (bufsize < 2))
	{
		return false;
	}

	//если строка еще не вс€ записана то записываем еЄ
	if (m_part)
	{
		m_part = StrCopy(buf, bufsize);
		return true;
	}

	while (m_file_size > 0 || m_data_size > 0)
	{
		//если буфер пустой и все считали то выходим
		if (m_data_size || ReadFile())
		{
			GetLine();
			if (CheckStr(m_start, m_filter))
			{
				m_part = StrCopy(buf, bufsize);
				return true;
			}
		}
		else
		{
			break;
		}
	} 

	return false;
}

bool CLogReader::IsPart() const
{
	return m_part;
}

/****************************************************************/
//ReadFile считывает часть файла, максимального размера DATA_SIZE
/****************************************************************/
bool CLogReader::ReadFile()
{
	bool result = false;
	
	if (DATA_SIZE > m_file_size)
	{
		m_data_size = (uint32_t)m_file_size;
	}
	else
	{
		m_data_size = DATA_SIZE;
	}

	if (fread(m_data_file, 1, m_data_size, m_file) == m_data_size)
	{
		//если это не конец строки, нужно найти. 
		if (m_data_size != (uint32_t)m_file_size)
		{
			if (!FindSetEnd())
			{
				return result;
			}
		}
		else
		{
			m_data_file[m_data_size] = '\n';
		}

		m_file_size -= m_data_size;
		m_end = m_start = NULL;
		result = true;
	}

	return result;
}

/****************************************************************/
//FindSetEnd() - Ќаходит конец последней строки и устанавливает позицию.
/****************************************************************/
bool CLogReader::FindSetEnd()
{
	bool result = false;

	int old = m_data_size;
	do
	{
		if (m_data_file[m_data_size - 1] == '\n')
		{
			break;
		}
	} while (--m_data_size);


	if (old != m_data_size && m_data_size)
	{
		fpos_t position;
		fgetpos(m_file, &position);
		position -= old - m_data_size;
		fsetpos(m_file, &position);
		result = true;
	}

	return result;
}

/****************************************************************/
//GetLine() - получает начало и конец строки.
/****************************************************************/
void CLogReader::GetLine()
{
	if (m_end == NULL)
	{
		m_end = m_start = m_data_file;
	}
	else
	{
		m_start = m_end + 1;
	}
		
	while (*++m_end != '\n');

	if (*(m_end - 1) == '\r')
	{
		*(m_end - 1) = 0;
	}
	else
	{
		*m_end = 0;
	}

	uint32_t def = m_end - m_start + 1;
	if (def > m_data_size)
	{
		m_data_size = 0;
	}
	else
	{
		m_data_size -= def;
	}
}

/****************************************************************/
//CheckStr() - провер€ет строку по фильтру.
/****************************************************************/
bool CLogReader::CheckStr(const char* str, const char* sub)
{
	register const char* p_str = str;
	register const char* p_sub = sub;

	bool ismany = false;

	for (; *str != 0; str += 1 )
	{

		p_str = str;
		while (*p_str != 0)
		{
			if (*p_sub == 0)
			{
				return ismany;
			}

			if (*p_sub == '*')
			{
				sub = p_sub;
				++p_sub;
				ismany = true;
			}
			else if (*p_sub == '?')
			{
				++p_sub;
			}
			else if (*p_sub == '#')
			{
				if (!isdigit__(*p_str))
				{
					break;
				}
				++p_sub;
				if (ismany)
				{
					ismany = false;
				}
			}
			else if (*p_sub == *p_str)
			{	
				++p_sub;
				if (ismany)
				{
					ismany = false;
				}
			}
			else if (!ismany )
			{
				break;
			}

			++p_str;
		}

		if (*p_str == 0)
		{
			return *p_sub == 0;
		}
		
		p_sub = sub;
		if (*p_sub != '*')
		{
			return false;
		}
	}

	return false;
}

/****************************************************************/
//StrCopy() - копирует строку в буфер.
/****************************************************************/
bool CLogReader::StrCopy(char *buf, const int bufsize)
{
	bool result = false;

	int len = m_end - m_start;	
	if (bufsize > len)
	{
		strncpy(buf, m_start, len);
		buf[len] = '\0';
	}
	else
	{
		strncpy(buf, m_start, bufsize - 1);
		buf[bufsize - 1] = '\0';
		m_start += bufsize - 1;
		result = true;
	}

	return result;
}