#include "LogReader.h"

#include <string.h>

const char ESTR = 0; //end of a string
const char ENDL = '\n';

namespace
{
	inline void  InvertIf(bool& _val, const bool _condition)
	{
		if (_condition == _val) _val = !_condition;
	}

	inline bool SetPosOfFile(FILE* _file, const size_t _pos)
	{
		if (0 == _pos)
		{
			return true;
		}

		fpos_t position;
		if (0 == fgetpos(_file, &position))
		{
			position -= _pos;
			if (0 == fsetpos(_file, &position))
			{
				return true;
			}
		}

		return false;
	}

	inline bool IsDigit(char _d)
	{
		return (_d >= '0' && _d <= '9' ? true : false);
	}

	//return length of a string to the symbol(search from start of a string)
	inline size_t StrLen(const char* _buf, const char _symbol = ESTR)
	{
		if (NULL == _buf)
		{
			return 0;
		}

		size_t i = 0;
		while (i <= MAXSIZE_LINE)
		{
			if (_symbol == _buf[i])
			{
				return i;
			}
			++i;
		}

		return 0;
	}

	//return length of a string to the symbol(search from end of a string)
	inline size_t RStrLen(const char* _buf, const size_t _len, const char _symbol = ESTR)
	{
		if (0 == _len || NULL == _buf)
		{
			return 0;
		}

		size_t curLen = _len;
		while (0 != curLen)
		{
			if (_symbol == _buf[--curLen])
			{
				return (_len - (curLen + 1));
			}
		}

		return 0;
	}

	//check string by the filter
	//_start - start point of the string
	//_end   - end point of the string
	//_filter   - filter
	inline bool CheckStr(const char* _start, const char* _end, const char* _filter)
	{
		if (NULL == _start || NULL == _end || NULL == _filter)
		{
			return false;
		}

		register const char* p_str = NULL;
		register const char* p_f = _filter;

		bool isAll = false;

		for (; _end != _start; ++_start)
		{
			p_str = _start;
			while (_end != p_str)
			{
				if (ESTR == *p_f)
				{
					return isAll;
				}
				else if (*p_f == *p_str)
				{
					 InvertIf(isAll, true);
					++p_f;
				}
				else if ('*' == *p_f)
				{
					_filter = p_f;
					isAll = true;
					++p_f;
					continue;
				}
				else if ('?' == *p_f)
				{
					++p_f;
				}
				else if ('#' == *p_f)
				{
					if (!IsDigit(*p_str))
					{
						break;
					}

					InvertIf(isAll, true);
					++p_f;
				}
				else if (!isAll)
				{
					break;
				}

				++p_str;
			}

			if (_end == p_str)
			{
				return (ESTR == *p_f) || ('*' == *p_f);
			}

			if ('*' != *_filter)
			{
				return false;
			}

			p_f = _filter;
		}

		return false;
	}
}

CLogReader::CLogReader()
	: m_file(NULL)
	, m_isRead(false)
	, m_isEOF(false)
	, m_dataEnd(NULL)
	, m_strStart(NULL)
	, m_strEnd(NULL)
	, m_strNext(NULL)
{
	memset(m_data, ESTR, MAXSIZE_DATA);
	memset(m_filter, ESTR, MAXSIZE_LINE);
}

CLogReader::~CLogReader()
{
	Close();
}

bool CLogReader::Open(const char *namefile )
{
	m_file = fopen(namefile, "rb");

	return (NULL != m_file);
}

void CLogReader::Close()
{
	if (NULL != m_file)
	{
		fclose(m_file);
		m_file = NULL;
	}
}

bool CLogReader::SetFilter(const char *_filter)
{
	if (NULL == _filter)
	{
		return false;
	}

	size_t sizeFilter = StrLen(_filter);
	if (0 == sizeFilter || MAXSIZE_LINE < sizeFilter)
	{
		return false;
	}

	size_t i = 0;
	size_t mi = 0;
	m_filter[mi] = _filter[i];
	while (sizeFilter > ++i)
	{
		switch (_filter[i])
		{
		case '*':
		{
			switch (m_filter[mi])
			{
			case '?':
			{
				m_filter[mi] = '*';
				break;
			}
			case '*':
			{
				break;
			}
			case '#':
			default:
			{
				m_filter[++mi] = '*';
				break;
			}
			}
			break;
		}
		case '?':
		{
			if ('*' != m_filter[mi])
			{
				m_filter[++mi] = '?';
			}

			break;
		}
		case '#':
		default:
		{
			m_filter[++mi] = _filter[i];
			break;
		}
		}
	}
	m_filter[++mi] = ESTR;

	return true;
}

bool CLogReader::GetNextLine(char *_buf, const int _bufsize)
{
	if (NULL == _buf || 0 == _bufsize)
	{
		return false;
	}

	while (1)
	{
		if (!m_isRead)
		{
			if (!ReadFile())
			{
				break;
			}
		}

		if (!NextLine())
		{
			return false;
		}

		if (CheckStr(m_strStart, m_strEnd, m_filter))
		{
			StrCopy(_buf, _bufsize);
			return true;
		}
	}

	return false;
}

bool CLogReader::ReadFile()
{
	if (m_isEOF)
	{
		return false;
	}

	size_t sizeRead = 0;
	size_t sizeDiff = 0;
	
	sizeRead = fread(m_data, 1, MAXSIZE_DATA, m_file);
	if (MAXSIZE_DATA == sizeRead)
	{
		sizeDiff = RStrLen(m_data, sizeRead, ENDL);
		if (sizeDiff == sizeRead)
		{
			return false;
		}
		else if (!SetPosOfFile(m_file, sizeDiff))
		{
			return false;
		}

		m_dataEnd = m_data + (sizeRead - sizeDiff - 1);
	}
	else if (0 != feof(m_file))
	{
		m_dataEnd = m_data + sizeRead;
		*m_dataEnd = ENDL;
		m_isEOF = true;
	}
	else 
	{
		return false;
	}

	m_strNext = m_data;
	m_strEnd = m_strStart = NULL;
	m_isRead = true;
	return true;
}

bool CLogReader::NextLine()
{
	m_strStart = m_strNext;

	size_t shift = SetEndLine();
	return SetNextStart(shift);
}

inline size_t CLogReader::SetEndLine()
{
	size_t shift = 0;
	size_t len = StrLen(m_strStart, ENDL);

	m_strEnd = m_strStart + len;
	if (0 != len)
	{
		if ('\r' == *(m_strEnd - 1))
		{
			--m_strEnd;
			shift = 1;
		}
	}

	return shift;
}

inline bool CLogReader::SetNextStart(const size_t _shift)
{
	int diff = m_dataEnd - m_strEnd;
	if (_shift == (size_t)diff)
	{
		m_isRead = false;
	}
	else
	{
		if (0 > diff || (size_t)diff < (_shift + 1))
		{
			return false;
		}
		else
		{
			m_strNext = m_strEnd + _shift + 1;
		}
	}

	return true;
}

void CLogReader::StrCopy(char *_buf, const int _bufsize)
{
	int len = m_strEnd - m_strStart;	
	if (_bufsize > len)
	{
		strncpy(_buf, m_strStart, (size_t)len);
		_buf[len] = ESTR;
	}
}