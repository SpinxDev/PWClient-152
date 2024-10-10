// File		: EC_StubbornNetSender.h
// Creator	: Xu Wenbin
// Date		: 2014/4/28

#ifndef _ELEMENTCLIENT_EC_STUBBORNNETSENDER_H_
#define _ELEMENTCLIENT_EC_STUBBORNNETSENDER_H_

#include "EC_TimeSafeChecker.h"
#include "EC_Faction.h"

#include <vector.h>

class CECStubbornNetSender
{
public:
	virtual ~CECStubbornNetSender(){}
	virtual bool IsFinished()const{ return true; }
	virtual void Tick(){}
	virtual void Reset(){}
};

class CECStubbornFactionInfoSender : public CECStubbornNetSender, public CECFactionManObserver
{
	CECTimeSafeChecker	m_timer;

	typedef abase::vector<int>	FactionIDArray;
	FactionIDArray		m_factionIDs;

public:
	CECStubbornFactionInfoSender();
	virtual ~CECStubbornFactionInfoSender();

	//	派生自 CECStubbornNetSender
	virtual bool IsFinished()const;
	virtual void Tick();
	virtual void Reset();

	//	派生自 CECFactionManObserver
	virtual void OnModelChange(const CECFactionMan *p, const CECObservableChange *q);

	void Add(int iNumFaction, const int* aFactionIDs);
};

#endif
