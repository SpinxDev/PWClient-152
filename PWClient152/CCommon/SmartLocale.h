// Filename	: SmartLocale.h
// Creator	: Xu Wenbin
// Date		: 2011/08/18

#pragma once

#include <locale.h>
#include <string>

class SmartLocale
{
public:

	SmartLocale(const char *s){
		m_oldLocate = setlocale(LC_ALL, NULL);
		m_newLocate = setlocale(LC_ALL, s);
	}

	~SmartLocale(){
		setlocale(LC_ALL, m_oldLocate.c_str());
	}

private:

	SmartLocale(const SmartLocale &);
	SmartLocale & operator=(const SmartLocale &);

	std::string	m_oldLocate;
	std::string	m_newLocate;
};