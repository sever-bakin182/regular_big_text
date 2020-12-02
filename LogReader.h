#pragma once

#include <stdio.h>
#include <stdint.h>

//��� ��� ������� �����
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
	//��� ������� ����� ���� ������� � � .cpp
	bool    ReadFile();
	bool    FindSetEnd();
	bool    CheckStr(const char* str,
					 const char* sub);
	bool    StrCopy(char *buf,
		            const int bufsize);
	void    GetLine();

private:
	FILE*        m_file;         //���������� �����
	size_f       m_file_size;    //����� �����

	char*        m_data_file;    //����� ������ (����� �����)
	uint32_t     m_data_size;    //����� ������

	char*        m_start;        //������ ������
	char*        m_end;          //����� ������
	bool         m_part;         //����, ������� ���������� ����� �� ������

	char*        m_filter;       //�����
};

