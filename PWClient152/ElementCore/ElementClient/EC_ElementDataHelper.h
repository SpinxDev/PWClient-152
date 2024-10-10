// Filename	: EC_ElementDataHelper.h
// Creator	: Xu Wenbin
// Date		: 2014/11/07

#pragma once

#include "ExpTypes.h"

//	�ṩ�� ElementData �ĸ���ȫ�ֲ���
namespace CECElementDataHelper
{
	const MINE_ESSENCE * GetMine(int id);
	int GetGatherStartActionConfig(int gatherItemID);
	int GetGatherLoopActionConfig(int gatherItemID);

	const SELL_CERTIFICATE_ESSENCE *GetCertificate(int id);
};
