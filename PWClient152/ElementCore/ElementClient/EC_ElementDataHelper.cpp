// Filename	: EC_ElementDataHelper.cpp
// Creator	: Xu Wenbin
// Date		: 2014/11/07

#include "EC_ElementDataHelper.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "elementdataman.h"

namespace CECElementDataHelper
{	
	const MINE_ESSENCE * GetMine(int id){
		const MINE_ESSENCE *result = NULL;
		if (id > 0){
			DATA_TYPE DataType = DT_INVALID;
			const void* pDataPtr = g_pGame->GetElementDataMan()->get_data_ptr(id, ID_SPACE_ESSENCE, DataType);
			if (pDataPtr && DataType == DT_MINE_ESSENCE){
				result = (const MINE_ESSENCE *)pDataPtr;
			}
		}
		return result;
	}
	int GetGatherStartActionConfig(int gatherItemID){
		int result(0);
		if (const MINE_ESSENCE *pMine = GetMine(gatherItemID)){
			result = pMine->gather_start_action_config;
		}
		return result;
	}
	int GetGatherLoopActionConfig(int gatherItemID){
		int result(0);
		if (const MINE_ESSENCE *pMine = GetMine(gatherItemID)){
			result = pMine->gather_loop_action_config;
		}
		return result;
	}
	const SELL_CERTIFICATE_ESSENCE * GetCertificate(int id){
		const SELL_CERTIFICATE_ESSENCE *result = NULL;
		if (id > 0){
			DATA_TYPE DataType = DT_INVALID;
			const void* pDataPtr = g_pGame->GetElementDataMan()->get_data_ptr(id, ID_SPACE_ESSENCE, DataType);
			if (pDataPtr && DataType == DT_SELL_CERTIFICATE_ESSENCE){
				result = (const SELL_CERTIFICATE_ESSENCE *)pDataPtr;
			}
		}
		return result;
	}
}
