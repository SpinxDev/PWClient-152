#ifndef _EC_IVTRDESTROYINGESSENCE_H
#define _EC_IVTRDESTROYINGESSENCE_H

#include "EC_IvtrItem.h"
#include "EC_IvtrTypes.h"

struct DESTROYING_ESSENCE;

class CECIvtrDestroyingEssence : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrDestroyingEssence(int tid, int expire_date);
	CECIvtrDestroyingEssence(const CECIvtrDestroyingEssence& s);
	virtual ~CECIvtrDestroyingEssence();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrDestroyingEssence(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	const DESTROYING_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	IVTR_ESSENCE_DESTROYING m_Essence;

	//	Data in database
	DESTROYING_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

#endif
