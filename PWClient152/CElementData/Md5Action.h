#pragma once

#ifndef _MD5_ACTION_H_
#define _MD5_ACTION_H_


class CMd5Action
{
public:
	CMd5Action(void);
	~CMd5Action(void);
public:
	bool Check(const char* szPath);
	bool Generate(const char* szPath);
};

#endif
