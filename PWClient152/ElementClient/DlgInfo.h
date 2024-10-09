// Filename	: DlgInfo.h
// Creator	: Tom Zhou
// Date		: October 19, 2005

#pragma once

#include "DlgBase.h"
#include "EC_IvtrItem.h"

class CECScriptContext;

class CDlgInfo : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	enum
	{
		INFO_NULL = 0,
		INFO_QUESTION,
		INFO_SYSTEM,
		INFO_FRIEND,
		INFO_HELP,
		INFO_QUESTIONTASK,	// 20141115ÐÂÔö
		INFO_NUM
	};
	typedef struct _INFORMATION
	{
		AString strType;
		ACString strMsg;
		DWORD dwLife;
		DWORD dwLifeBegin;
		DWORD dwData1;
		DWORD dwData2;
		DWORD dwData3;
		
		~_INFORMATION()
		{
			DeleteLinkedItem();
		}

		_INFORMATION(const _INFORMATION &rhs)
			: dwLife(0)
			, dwLifeBegin(0)
			, dwData1(0)
			, dwData2(0)
			, dwData3(0)
		{
			*this = rhs;
		}

		_INFORMATION & operator=(const _INFORMATION &rhs)
		{
			DeleteLinkedItem();
			
			strType = rhs.strType;
			strMsg = rhs.strMsg;
			dwLife = rhs.dwLife;
			dwLifeBegin = rhs.dwLifeBegin;
			dwData1 = rhs.dwData1;
			dwData2 = rhs.dwData2;

			CopyLinkedItem(rhs);

			return *this;
		}

		_INFORMATION()
			: dwLife(0)
			, dwLifeBegin(0)
			, dwData1(0)
			, dwData2(0)
			, dwData3(0)
		{}

		void DeleteLinkedItem()
		{
			if (!stricmp(strType, "Game_FriendMsg"))
			{
				if (dwData3)
				{
					CECIvtrItem *pItem = (CECIvtrItem *)(dwData3);
					delete pItem;
					dwData3 = 0;
				}
			}
		}

		void CopyLinkedItem(const _INFORMATION &rhs)
		{
			dwData3 = rhs.dwData3;
			if (!stricmp(strType, "Game_FriendMsg"))
			{
				if (rhs.dwData3)
				{
					CECIvtrItem *pItem = (CECIvtrItem *)(rhs.dwData3);
					dwData3 = (DWORD)pItem->Clone();
				}
			}
		}

	} INFORMATION, * P_INFORMATION;

public:
	CDlgInfo();
	virtual ~CDlgInfo();

	void OnCommand_CANCEL(const char * szCommand);

	void OnEventLButtonUp_Icon(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void BuildInfoList(int nType);
	void AddInfo(int nType, INFORMATION Info);
	static void RemoveInfo(int nType, INFORMATION Info);
	void SetScriptContext(CECScriptContext * pContext);

protected:
	CECScriptContext * m_pScriptContext;
	static abase::vector<INFORMATION> m_vecInfo[INFO_NUM];

	virtual void OnTick();
};
