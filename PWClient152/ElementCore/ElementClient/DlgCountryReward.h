// Filename	: DlgCountryReward.h
// Creator	: Han Guanghui
// Date		: 2012/8/10

#ifndef _DLGCOUNTRYREWARD_H_
#define _DLGCOUNTRYREWARD_H_

#include "DlgBase.h"

namespace GNET{
	class Protocol;
}

class CDlgCountryReward : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgCountryReward();
	virtual ~CDlgCountryReward();

	void OnCommandCancel(const char * szCommand);

	void OnPrtcCountryResult(GNET::Protocol* pProtocol);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
};

#endif // _DLGCOUNTRYREWARD_H_