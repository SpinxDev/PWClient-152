// Filename	: EC_InstanceReenter.h
// Creator	: Xu Wenbin
// Date		: 2014/8/18

#ifndef _ELEMENTCLIENT_EC_INSTANCEREENTER_H_
#define _ELEMENTCLIENT_EC_INSTANCEREENTER_H_

#include "EC_Global.h"

namespace S2C{
	struct cmd_instance_reenter_notify;
}

class CECInstanceReenter{
	int		m_worldTag;
	int		m_deadline;

	ELEMENTCLIENT_DECLARE_SINGLETON(CECInstanceReenter);

private:
	bool HasReenterNotify()const;
	int  GetTimeLeft()const;
	bool HasTimeLeft()const;

	bool IsGoodTimeToShowNotify()const;
	ACString GetNotifyMessage()const;
	void ShowNotify();
	void HideNotify();
	void UpdateShownNotify();
	bool IsNotifyShown()const;
	
public:
	
	static const char *GetNotifyContext();

	void OnNotify(const S2C::cmd_instance_reenter_notify &cmd);
	void Tick();
	void Reenter(bool agree);
	void Clear();
};

#endif	//	_ELEMENTCLIENT_EC_INSTANCEREENTER_H_