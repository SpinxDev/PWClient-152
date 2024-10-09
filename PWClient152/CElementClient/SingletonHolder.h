/********************************************************************
	created:	2005/08/31
	created:	31:8:2005   11:05
	file name:	SingletonHolder.h
	author:		yaojun
	
	purpose:	singleton
*********************************************************************/

#pragma once

template<typename Host>
class SingletonHolder
{
private:
	static Host * pHost_;
private:
	static Host * Create()
	{
		static Host host;
		return &host;
	}

	SingletonHolder() {}
	~SingletonHolder() {}
public:
	static Host * Instance()
	{
		if (!pHost_)
		{
			pHost_ = Create();
		}
		return pHost_;
	}
};

template<typename Host>
Host * SingletonHolder<Host>::pHost_ = 0;
