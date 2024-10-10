/********************************************************************
  created:	   9/5/2006  
  filename:	   LuaDlgApi.cpp
  author:      Wangkuiwu  
  description: 
  Copyright (c) , All Rights Reserved.
*********************************************************************/

#include "AUI.h"

#include "vector.h"
#include "A3DMacros.h"
#include "AUIDef.h"
#include "LuaDlgApi.h"
#include "LuaAPI.h"
#include "LuaState.h"
#include "AUILuaManager.h"
#include "AUILuaDialog.h"
#include "AUIProgress.h"
#include "AUIImagePicture.h"
#include "AUICheckBox.h"
#include "AUIRadioButton.h"
#include "AUIListBox.h"
#include "AUIComboBox.h"
#include "AUIScroll.h"
#include "AUIConsole.h"
#include "AUIStillImageButton.h"
#include "AUITextArea.h"
#include "AUITreeView.h"
#include "AUILabel.h"
#include "A2DSprite.h"

using namespace LuaBind;
using namespace abase;

#define SET_SCRIPTVALUE(key, value)\
	val.SetVal(key);\
	veckey.push_back(val);\
	val.SetVal(value);\
	vecvalue.push_back(val);

#define RETRIEVE_ASTRING(dst, src) \
	AString dst;\
	src.RetrieveAString(dst);

#define RETRIEVE_AWSTRING(dst, src) \
	AWString dst;\
	src.RetrieveAWString(dst);

void PopupMessageBox(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_NUMBER ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.PopupMessageBox() parameters error");
		return;
	}
	
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.PopupMessageBox() no ui man");
		return;
	}

	int type = args[0].GetInt();
	RETRIEVE_ASTRING(messagebox_name, args[1])
	RETRIEVE_AWSTRING(message, args[2])
	A3DCOLOR color = A3DCOLORRGBA(255, 255, 255, 160);
	if(args.size()>=4)
	{
		color = (A3DCOLOR)args[3].GetDouble();
	}
	pUIMan->MessageBox(messagebox_name, message, type, color);
}
IMPLEMENT_SCRIPT_API(PopupMessageBox);

void RespawnMessage(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.RespawnMessage() no ui man");
		return;
	}

	pUIMan->RespawnMessage();
}
IMPLEMENT_SCRIPT_API(RespawnMessage);

void ConvertChatString(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 1 ||
		args[0].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.ConvertChatString() parameters error");
		return;
	}
	
	RETRIEVE_AWSTRING(strText, args[0])
	ACHAR *szText = new ACHAR[strText.GetLength() * 2 + 1];
	AUI_ConvertChatString_S(strText, strText.GetLength(), szText, strText.GetLength()*2+1);
	CScriptValue val;
	val.SetVal(szText);
	results.push_back(val);
	delete []szText;
}
IMPLEMENT_SCRIPT_API(ConvertChatString);

void GetWindowScale(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetWindowScale() no ui man");
		return;
	}

	CScriptValue val;
	val.SetVal(pUIMan->GetWindowScale());
	results.push_back(val);
}
IMPLEMENT_SCRIPT_API(GetWindowScale);

void GetWindowRect(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetWindowRect() no ui man");
		return;
	}

	A3DRECT rc = pUIMan->GetRect();
	CScriptValue val;
	val.SetVal(rc.left);
	results.push_back(val);
	val.SetVal(rc.top);
	results.push_back(val);
	val.SetVal(rc.right);
	results.push_back(val);
	val.SetVal(rc.bottom);
	results.push_back(val);
}
IMPLEMENT_SCRIPT_API(GetWindowRect);

void GetCurrentIMEName(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	CScriptValue val;
	val.SetVal(AUIEditBox::GetCurrentIMEName());
	results.push_back(val);
}
IMPLEMENT_SCRIPT_API(GetCurrentIMEName);

void ShowDialog(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_BOOL )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.ShowDialog() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.ShowDialog() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	bool bShow = args[1].GetBool();
	bool bModal = false;
	if( args.size() >= 3 && args[2].m_Type == CScriptValue::SVT_BOOL )
		bModal = args[2].GetBool();
	bool bActive = true;
	if( args.size() >= 4 && args[3].m_Type == CScriptValue::SVT_BOOL )
		bActive = args[3].GetBool();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		pDlg->Show(bShow, bModal, bActive);
	}
}
IMPLEMENT_SCRIPT_API(ShowDialog);

void IsDialogShow(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 1 ||
		args[0].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.IsDialogShow() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.IsDialogShow() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		results.push_back(CScriptValue(pDlg->IsShow()));
	}
}
IMPLEMENT_SCRIPT_API(IsDialogShow);

void IsDialogActive(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 1 ||
		args[0].m_Type != CScriptValue::SVT_STRING )

	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.IsDialogActive() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.IsDialogActive() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])

	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		results.push_back(CScriptValue(pDlg->IsActive()));
	}
}
IMPLEMENT_SCRIPT_API(IsDialogActive);

void EnableDialog(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_BOOL )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.EnableDialog() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.EnableDialog() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	bool bEnable = args[1].GetBool();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		pDlg->SetEnable(bEnable);
	}
}
IMPLEMENT_SCRIPT_API(EnableDialog);

void SetDialogPosition(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_NUMBER ||
		args[2].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetDialogPosition() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetDialogPosition() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		int offsetX = args[1].GetInt();
		int offsetY = args[2].GetInt();
		if( args.size() >= 8 && 
			args[3].m_Type == CScriptValue::SVT_STRING &&
			args[4].m_Type == CScriptValue::SVT_NUMBER &&
			args[5].m_Type == CScriptValue::SVT_NUMBER &&
			args[6].m_Type == CScriptValue::SVT_NUMBER &&
			args[7].m_Type == CScriptValue::SVT_NUMBER )
		{
			RETRIEVE_ASTRING(dlg_name_align_to, args[3])
			int nInsideX = args[4].GetInt();
			int nAlignX = args[5].GetInt();
			int nInsideY = args[6].GetInt();
			int nAlignY = args[7].GetInt();
			pDlg->AlignTo(pUIMan->GetDialog(dlg_name_align_to), nInsideX, nAlignX, nInsideY, nAlignY, offsetX, offsetY);
		}
		else
		{
			pDlg->AlignTo(NULL, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_LEFT, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_TOP, 
				offsetX, offsetY);
		}
	}
}
IMPLEMENT_SCRIPT_API(SetDialogPosition);

void SetDialogSize(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_NUMBER ||
		args[2].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetDialogSize() parameters error");
		return;
	}
	
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetDialogSize() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	int w = args[1].GetInt();
	int h = args[2].GetInt();
	
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
		pDlg->SetSize(w, h);
}
IMPLEMENT_SCRIPT_API(SetDialogSize);

void SetDialogScale(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetDialogScale() parameters error");
		return;
	}
	
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetDialogScale() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	float scale = (float)args[1].GetDouble();
	
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
		pDlg->SetScale(scale);
}
IMPLEMENT_SCRIPT_API(SetDialogScale);

void SetDialogAlpha(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetDialogAlpha() parameters error");
		return;
	}
	
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetDialogAlpha() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	int alpha = args[1].GetInt();
	
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
		pDlg->SetAlpha(alpha);
}
IMPLEMENT_SCRIPT_API(SetDialogAlpha);

void SetDialogWholeAlpha(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetDialogWholeAlpha() parameters error");
		return;
	}
	
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetDialogWholeAlpha() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	int alpha = args[1].GetInt();
	
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
		pDlg->SetWholeAlpha(alpha);
}
IMPLEMENT_SCRIPT_API(SetDialogWholeAlpha);

void SetDialogFrame(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetDialogFrame() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetDialogFrame() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(frame_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIOBJECT_SETPROPERTY p;
		strcpy(p.fn, frame_name);
		pDlg->SetProperty("Frame Image", &p);
	}
}
IMPLEMENT_SCRIPT_API(SetDialogFrame);

void SetDialogTimer(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_BOOL )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetDialogTimer() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetDialogTimer() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	bool bTimer = args[1].GetBool();
	AUILuaDialog * pDlg = (AUILuaDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		pDlg->SetTimer(bTimer);
	}
}
IMPLEMENT_SCRIPT_API(SetDialogTimer);

void GetHitItem(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetHitItem() no ui man");
		return;
	}

	PAUIDIALOG pDlgOver;
	PAUIOBJECT pObjOver;
	if( args.size() == 0 )
	{
		pUIMan->GetMouseOn(&pDlgOver, &pObjOver);
	}
	else if( args.size() >= 2 &&
		args[0].m_Type == CScriptValue::SVT_NUMBER &&
		args[1].m_Type == CScriptValue::SVT_NUMBER )
	{
		int x = args[0].GetInt();
		int y = args[1].GetInt();
		(AUIDialog*)pUIMan->HitTest(x, y, &pDlgOver, &pObjOver);
	}
	else
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetHitItem() parameters error");
	}

	CScriptValue val;
	if( pDlgOver )
	{
		val.SetVal(pDlgOver->GetName());
		results.push_back(val);

		if( pObjOver )
		{
			val.SetVal(pObjOver->GetName());
			results.push_back(val);
		}
	}
}
IMPLEMENT_SCRIPT_API(GetHitItem);

void GetNameLink(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 4 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER ||
		args[3].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetNameLink() parameters error");
		return;
	}
	
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetNameLink() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int x = args[2].GetInt();
	int y = args[3].GetInt();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_TEXTAREA )
			{
				A3DRECT rcWindow = pUIMan->GetRect();
				POINT ptPos = pObj->GetPos();
				PAUITEXTAREA pText = (PAUITEXTAREA)pObj;
				if( pText->GetHitArea(x - ptPos.x, y - ptPos.y) == AUITEXTAREA_RECT_FRAME )
				{
					int i;
					abase::vector<AUITEXTAREA_NAME_LINK> &vecNameLink = pText->GetNameLink();

					x += rcWindow.left;
					y += rcWindow.top;
					for( i = 0; i < (int)vecNameLink.size(); i++ )
					{
						if( vecNameLink[i].rc.PtInRect(x, y) )
						{
							CScriptValue val;
							val.SetVal(vecNameLink[i].strName);
							results.push_back(val);
							return;
						}
					}
				}
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(GetNameLink);

void GetItemLink(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 4 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER ||
		args[3].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetItemLink() parameters error");
		return;
	}
	
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetItemLink() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int x = args[2].GetInt();
	int y = args[3].GetInt();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_TEXTAREA )
			{
				A3DRECT rcWindow = pUIMan->GetRect();
				POINT ptPos = pObj->GetPos();
				PAUITEXTAREA pText = (PAUITEXTAREA)pObj;
				if( pText->GetHitArea(x - ptPos.x, y - ptPos.y) == AUITEXTAREA_RECT_FRAME )
				{
					int i;
					abase::vector<AUITEXTAREA_EDITBOX_ITEM> &vecItemLink = pText->GetEditBoxItems();

					x += rcWindow.left;
					y += rcWindow.top;
					for( i = 0; i < (int)vecItemLink.size(); i++ )
					{
						if( vecItemLink[i].rc.PtInRect(x, y) )
						{
							CScriptValue val;
							val.SetVal(vecItemLink[i].m_pItem->GetInfo());
							results.push_back(val);
							return;
						}
					}
				}
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(GetItemLink);

void GetTextAreaLineInfo(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetTextAreaLineInfo() parameters error");
		return;
	}
	
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetTextAreaLineInfo() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_TEXTAREA )
			{
				PAUITEXTAREA pText = (PAUITEXTAREA)pObj;
				CScriptValue val, ret;
				abase::vector<CScriptValue> veckey;
				abase::vector<CScriptValue> vecvalue;

				SET_SCRIPTVALUE("lines", pText->GetLines())
				SET_SCRIPTVALUE("linesperpage", pText->GetLinesPP())
				SET_SCRIPTVALUE("firstline", pText->GetFirstLine())

				ret.SetArray(vecvalue, veckey);
				results.push_back(ret);
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(GetTextAreaLineInfo);


void OnCommand(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.OnCommand() parameters error");
		return;
	}
	
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.OnCommand() no ui man");
		return;
	}
	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(command_str, args[1])
	AUIDialog* pDlg = (AUIDialog* )pUIMan->GetDialog(dlg_name);
	if (!pDlg)
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.OnCommand() no dialog");
		return;
	}
	if (!command_str.GetLength())
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.OnCommand() no command");
		return;
	}
	pUIMan->OnCommand(command_str, pDlg);
}
IMPLEMENT_SCRIPT_API(OnCommand);

void SetDialogProp(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_ARRAY )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetDialogProp() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetDialogProp() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		for(int i = 0; i < (int)args[1].m_ArrKey.size(); i++)
			if( args[1].m_ArrKey[i].m_Type == CScriptValue::SVT_STRING )
			{
				RETRIEVE_ASTRING(strText, args[1].m_ArrKey[i])
				if( strText == "canmove" )
					 pDlg->SetCanMove(args[1].m_ArrVal[i].GetBool());
				else if( strText == "canoutofwindow" )
					 pDlg->SetCanOutOfWindow(args[1].m_ArrVal[i].GetBool());
			}
	}
}
IMPLEMENT_SCRIPT_API(SetDialogProp);

void GetDialogProp(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 1 ||
		args[0].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetDialogProp() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetDialogProp() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		CScriptValue val, ret;
		abase::vector<CScriptValue> veckey;
		abase::vector<CScriptValue> vecvalue;

		SET_SCRIPTVALUE("x", pDlg->GetPos().x)
		SET_SCRIPTVALUE("y", pDlg->GetPos().y)
		SET_SCRIPTVALUE("width", pDlg->GetSize().cx)
		SET_SCRIPTVALUE("height", pDlg->GetSize().cy)
		SET_SCRIPTVALUE("canmove", pDlg->IsCanMove())
		SET_SCRIPTVALUE("canoutofwindow", pDlg->IsCanOutOfWindow())

		ret.SetArray(vecvalue, veckey);
		results.push_back(ret);
	}
}
IMPLEMENT_SCRIPT_API(GetDialogProp);

void ChangeFocus(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.ChangeFocus() parameters error");
		return;
	}
	
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.ChangeFocus() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if( !pDlg->IsActive() )
			pDlg->Show(true);
		pDlg->ChangeFocus(pObj);
	}
}
IMPLEMENT_SCRIPT_API(ChangeFocus);

void ShowItem(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_BOOL )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.ShowItem() parameters error");
		return;
	}
	
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.ShowItem() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	bool bShow = args[2].GetBool();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			pObj->Show(bShow);
		}
	}
}
IMPLEMENT_SCRIPT_API(ShowItem);

void IsItemShow(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.IsItemShow() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.IsItemShow() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			results.push_back(CScriptValue(pObj->IsShow()));
		}
	}
}
IMPLEMENT_SCRIPT_API(IsItemShow);

void SetItemPos(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 4 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER ||
		args[3].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetItemPos() parameters error");
		return;
	}
	
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetItemPos() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int x = args[2].GetInt();
	int y = args[3].GetInt();
	
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
			pObj->SetPos(x, y);
	}
}
IMPLEMENT_SCRIPT_API(SetItemPos);

void SetItemSize(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 4 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER ||
		args[3].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetItemSize() parameters error");
		return;
	}
	
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetItemSize() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int w = args[2].GetInt();
	int h = args[3].GetInt();
	
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
			pObj->SetSize(w, h);
	}
}
IMPLEMENT_SCRIPT_API(SetItemSize);

void EnableItem(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_BOOL )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.EnableItem() parameters error");
		return;
	}
	
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.EnableItem() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	bool bShow = args[2].GetBool();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			pObj->Enable(bShow);
		}
	}
}
IMPLEMENT_SCRIPT_API(EnableItem);

void IsItemEnabled(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.IsItemEnabled() parameters error");
		return;
	}
	
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.IsItemEnabled() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			results.push_back(CScriptValue(pObj->IsEnabled()));
		}
	}
}
IMPLEMENT_SCRIPT_API(IsItemEnabled);

void GetItemRect(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetItemRect() parameters error");
		return;
	}
	
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetItemRect() no ui man");
		return;
	}

	bool bLocal = false;
	if( args.size() >= 3 && args[2].m_Type == CScriptValue::SVT_BOOL )
		bLocal = args[2].GetBool();

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			CScriptValue val, ret;
			abase::vector<CScriptValue> veckey;
			abase::vector<CScriptValue> vecvalue;

			SET_SCRIPTVALUE("x", pObj->GetPos(bLocal).x)
			SET_SCRIPTVALUE("y", pObj->GetPos(bLocal).y)
			SET_SCRIPTVALUE("width", pObj->GetSize().cx)
			SET_SCRIPTVALUE("height", pObj->GetSize().cy)

			ret.SetArray(vecvalue, veckey);
			results.push_back(ret);
		}
	}
}
IMPLEMENT_SCRIPT_API(GetItemRect);

void SetItemText(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetItemText() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetItemText() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	RETRIEVE_AWSTRING(item_text, args[2])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			pObj->SetText(item_text);
		}
	}
}
IMPLEMENT_SCRIPT_API(SetItemText);

void GetItemText(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetItemText() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetItemText() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			CScriptValue val;
			ACString strText(pObj->GetText());
			val.SetVal(strText);
			results.push_back(val);
		}
	}
}
IMPLEMENT_SCRIPT_API(GetItemText);

void AppendItemText(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.AppendItemText() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.AppendItemText() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	RETRIEVE_AWSTRING(item_text, args[2])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_TEXTAREA )
			{
				PAUITEXTAREA pText = (PAUITEXTAREA)pObj;
				pText->AppendText(item_text);
			}
			else
				pObj->SetText(pObj->GetText() + item_text);
		}
	}
}
IMPLEMENT_SCRIPT_API(AppendItemText);

void SerializeItemText(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 6 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_NUMBER ||
		args[2].m_Type != CScriptValue::SVT_NUMBER ||
		args[3].m_Type != CScriptValue::SVT_NUMBER ||
		args[4].m_Type != CScriptValue::SVT_NUMBER ||
		args[5].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SerializeItemText() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SerializeItemText() no ui man");
		return;
	}

	RETRIEVE_AWSTRING(item_text, args[0])
	int r = args[1].GetInt();
	int g = args[2].GetInt();
	int b = args[3].GetInt();
	int a = args[4].GetInt();
	RETRIEVE_AWSTRING(item_info, args[5])
	EditBoxItemBase item(enumEICoord);
	item.SetName(item_text);
	item.SetInfo(item_info);
	item.SetColor(A3DCOLORRGBA(r, g, b, a));
	ACString strText = (ACHAR)AUICOMMON_ITEM_CODE_START + item.Serialize();
	CScriptValue val;
	val.SetVal(strText);
	results.push_back(val);
}
IMPLEMENT_SCRIPT_API(SerializeItemText);

void AppendEmotion(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 5 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER ||
		args[3].m_Type != CScriptValue::SVT_NUMBER ||
		args[4].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.AppendEmotion() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.AppendEmotion() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int nSet = args[2].GetInt();
	int nIndex = args[3].GetInt();
	int nMax = args[4].GetInt();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_EDITBOX )
			{
				PAUIEDITBOX pEdit = (PAUIEDITBOX)pObj;
				if (pEdit->GetItemCount(enumEIEmotion) < nMax )
					pEdit->AppendItem(enumEIEmotion, 0, L"W", MarshalEmotionInfo(nSet, nIndex));
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(AppendEmotion);

void FilterEmotionSet(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.FilterEmotionSet() parameters error");
		return;
	}

	RETRIEVE_AWSTRING(text, args[0])
	int nSet = args[1].GetInt();
	ACString res = FilterEmotionSet(text, nSet);
	CScriptValue val;
	val.SetVal(res);
	results.push_back(val);
}
IMPLEMENT_SCRIPT_API(FilterEmotionSet);

void SetItemMask(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetItemMask() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetItemMask() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int iMask = args[2].GetInt();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_TEXTAREA )
			{
				PAUITEXTAREA pText = (PAUITEXTAREA)pObj;
				pText->SetItemMask(iMask);
			}
			else if( pObj->GetType() == AUIOBJECT_TYPE_LABEL )
			{
				PAUILABEL pLabel = (PAUILABEL)pObj;
				pLabel->SetItemMask(iMask);
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(SetItemMask);

void SetOnlyShowText(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_BOOL )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetOnlyShowText() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetOnlyShowText() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	bool bShow = args[2].GetBool();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_TEXTAREA )
			{
				PAUITEXTAREA pText = (PAUITEXTAREA)pObj;
				pText->SetOnlyShowText(bShow);
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(SetOnlyShowText);

void ScrollToTop(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.ScrollToTop() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.ScrollToTop() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_TEXTAREA )
			{
				PAUITEXTAREA pText = (PAUITEXTAREA)pObj;
				pText->ScrollToTop();
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(ScrollToTop);

void ScrollToBottom(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.ScrollToBottom() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.ScrollToBottom() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_TEXTAREA )
			{
				PAUITEXTAREA pText = (PAUITEXTAREA)pObj;
				pText->ScrollToBottom();
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(ScrollToBottom);

void SetReadOnly(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_BOOL )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetReadOnly() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetReadOnly() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	bool bReadOnly = args[2].GetBool();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_EDITBOX )
			{
				PAUIEDITBOX pEdit = (PAUIEDITBOX)pObj;
				pEdit->SetIsReadOnly(bReadOnly);
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(SetReadOnly);

void SetItemColor(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 6 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER ||
		args[3].m_Type != CScriptValue::SVT_NUMBER ||
		args[4].m_Type != CScriptValue::SVT_NUMBER ||
		args[5].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetItemColor() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetItemColor() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			int r = args[2].GetInt();
			int g = args[3].GetInt();
			int b = args[4].GetInt();
			int a = args[5].GetInt();
			pObj->SetColor(A3DCOLORRGBA(r, g, b, a));
		}
	}
}
IMPLEMENT_SCRIPT_API(SetItemColor);

void SetItemAlpha(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetItemAlpha() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetItemAlpha() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			int a = args[2].GetInt();
			pObj->SetAlpha(a);
		}
	}
}
IMPLEMENT_SCRIPT_API(SetItemAlpha);

void SetItemHint(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetItemHint() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetItemHint() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	RETRIEVE_AWSTRING(item_hint, args[2])
	
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			pObj->SetHint(item_hint);
		}
	}
}
IMPLEMENT_SCRIPT_API(SetItemHint);

void FlashItem(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_BOOL )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.FlashItem() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.FlashItem() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	bool bFlash = args[2].GetBool();
	
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			pObj->SetFlash(bFlash);
		}
	}
}
IMPLEMENT_SCRIPT_API(FlashItem);

void SetEmotionSet(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetEmotionSet() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetEmotionSet() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int nSet = args[2].GetInt();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj)
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_LABEL )
				((PAUILABEL)pObj)->SetCurrentEmotion(nSet);
			else if( pObj->GetType() == AUIOBJECT_TYPE_EDITBOX )
				((PAUIEDITBOX)pObj)->SetCurrentEmotion(nSet);
		}
	}
}
IMPLEMENT_SCRIPT_API(SetEmotionSet);

void SetMarquee(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 5 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_BOOL ||
		args[3].m_Type != CScriptValue::SVT_NUMBER ||
		args[4].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetMarquee() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetMarquee() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	bool bMarquee = args[2].GetBool();
	int nTimes = args[3].GetInt();
	int nSpeed = args[4].GetInt();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj && pObj->GetType() == AUIOBJECT_TYPE_LABEL )
		{
			((PAUILABEL)pObj)->SetMarquee(bMarquee, nTimes, nSpeed);
		}
	}
}
IMPLEMENT_SCRIPT_API(SetMarquee);

void IsMarquee(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.IsMarquee() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.IsMarquee() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj && pObj->GetType() == AUIOBJECT_TYPE_LABEL )
		{
			results.push_back(CScriptValue(((PAUILABEL)pObj)->IsMarquee()));
		}
	}
}
IMPLEMENT_SCRIPT_API(IsMarquee);

void SetProgress(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetProgress() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetProgress() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	double pos = args[2].GetDouble();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj && pObj->GetType() == AUIOBJECT_TYPE_PROGRESS )
		{
			PAUIPROGRESS pProgress = (PAUIPROGRESS)pObj;
			pProgress->SetProgress(int(pProgress->GetRangeMax() * pos));
		}
	}
}
IMPLEMENT_SCRIPT_API(SetProgress);

void SetAutoProgress(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 5 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER ||
		args[3].m_Type != CScriptValue::SVT_NUMBER ||
		args[4].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetAutoProgress() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetAutoProgress() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int msecs = args[2].GetInt();
	double start = args[3].GetDouble();
	double end = args[4].GetDouble();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj && pObj->GetType() == AUIOBJECT_TYPE_PROGRESS )
		{
			PAUIPROGRESS pProgress = (PAUIPROGRESS)pObj;
			pProgress->SetAutoProgress(msecs, int(pProgress->GetRangeMax() * start), int(pProgress->GetRangeMax() * end));
		}
	}
}
IMPLEMENT_SCRIPT_API(SetAutoProgress);

void SetActionProgress(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 4 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER ||
		args[3].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetActionProgress() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetActionProgress() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int msecs = args[2].GetInt();
	double pos = args[3].GetDouble();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj && pObj->GetType() == AUIOBJECT_TYPE_PROGRESS )
		{
			PAUIPROGRESS pProgress = (PAUIPROGRESS)pObj;
			pProgress->SetActionProgress(msecs, int(pProgress->GetRangeMax() * pos));
		}
	}
}
IMPLEMENT_SCRIPT_API(SetActionProgress);

void GetEmotionsInfo(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetEmotionsInfo() no ui man");
		return;
	}

	CScriptValue ret, val;
	abase::vector<CScriptValue> vecvalue;
	int i;
	for(i = 0; i < AUIMANAGER_MAX_EMOTIONGROUPS; i++)
		if( pUIMan->GetDefaultEmotion()[i] )
		{
			val.SetVal((int)pUIMan->GetDefaultEmotion()[i]->size());
			vecvalue.push_back(val);
		}
		else
			break;
	val.SetVal(i);
	results.push_back(val);
	ret.SetArray(vecvalue);
	results.push_back(ret);
}
IMPLEMENT_SCRIPT_API(GetEmotionsInfo);

void SetImageEmotion(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 5 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER ||
		args[3].m_Type != CScriptValue::SVT_NUMBER ||
		args[4].m_Type != CScriptValue::SVT_BOOL )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetImageEmotion() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetImageEmotion() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int nSet = args[2].GetInt();
	int nEmotion = args[3].GetInt();
	bool bPlay = args[4].GetBool();
	if( nSet < AUIMANAGER_MAX_EMOTIONGROUPS )
	{
		AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
		if (pDlg)
		{
			AUIObject * pObj = pDlg->GetDlgItem(item_name);
			if (pObj && pObj->GetType() == AUIOBJECT_TYPE_IMAGEPICTURE )
			{
				PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObj;
				A2DSprite* pSprite = pUIMan->GetDefaultSpriteEmotion()[nSet];
				abase::vector<AUITEXTAREA_EMOTION>* pEmotion = pUIMan->GetDefaultEmotion()[nSet];
				if( pSprite && pEmotion && nEmotion < (int)pEmotion->size() )
				{
					int nOffset = 0;
					int nBase = (*pEmotion)[nEmotion].nStartPos;
					if( bPlay )
					{
						int nTick = int(GetTickCount() * 0.06);
						int nFrame = (*pEmotion)[nEmotion].nNumFrames;
						int nThisTick = nTick % (*pEmotion)[nEmotion].nFrameTick[nFrame - 1];
						while( nOffset < nFrame - 1 && nThisTick > (*pEmotion)[nEmotion].nFrameTick[nOffset] )
							nOffset++;
					}
					pImage->SetCover(pSprite, nBase + nOffset);
					pImage->SetHint((*pEmotion)[nEmotion].strHint);
				}
				else
				{
					pImage->ClearCover();
					pImage->SetHint(_AL(""));
				}
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(SetImageEmotion);

void SetImageRenderTop(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_BOOL )

	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetImageRenderTop() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetImageRenderTop() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	bool bRenderTop = args[2].GetBool();

	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj && pObj->GetType() == AUIOBJECT_TYPE_IMAGEPICTURE)
		{
			PAUIIMAGEPICTURE pImage= (PAUIIMAGEPICTURE)pObj;
			pImage->SetRenderTop(bRenderTop);
		}
	}
}
IMPLEMENT_SCRIPT_API(SetImageRenderTop);

void SetImageFile(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 4 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_STRING ||
		args[3].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetImageFile() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetImageFile() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	RETRIEVE_ASTRING(image_name, args[2])
	int image_frames = args[3].GetInt();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj )
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_IMAGEPICTURE )
			{
				PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObj;
				if (image_frames > 0)
					results.push_back(CScriptValue(pImage->InitIndirect(image_name, 0) && pImage->SetFrameNumber(image_frames)));
				else
					results.push_back(CScriptValue(pImage->InitIndirect(NULL, 0)));
			}
			else if( pObj->GetType() == AUIOBJECT_TYPE_STILLIMAGEBUTTON )
			{
				PAUISTILLIMAGEBUTTON pButton = (PAUISTILLIMAGEBUTTON)pObj;
				results.push_back(CScriptValue(pButton->InitIndirect(image_name, image_frames)));
			}
			else if( pObj->GetType() == AUIOBJECT_TYPE_CHECKBOX )
			{
				PAUICHECKBOX pCheck = (PAUICHECKBOX)pObj;
				results.push_back(CScriptValue(pCheck->InitIndirect(image_name, image_frames)));
			}
			else if( pObj->GetType() == AUIOBJECT_TYPE_RADIOBUTTON )
			{
				PAUIRADIOBUTTON pRadio = (PAUIRADIOBUTTON)pObj;
				results.push_back(CScriptValue(pRadio->InitIndirect(image_name, image_frames)));
			}
		}		
	}
}
IMPLEMENT_SCRIPT_API(SetImageFile);

void SelectImageFrame(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SelectImageFrame() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SelectImageFrame() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int frame = args[2].GetInt();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj && pObj->GetType() == AUIOBJECT_TYPE_IMAGEPICTURE )
		{
			PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObj;
			pImage->FixFrame(frame);
		}
	}
}
IMPLEMENT_SCRIPT_API(SelectImageFrame);

void CheckItem(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_BOOL )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.CheckItem() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.CheckItem() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	bool check = args[2].GetBool();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj) 
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_CHECKBOX )
			{
				PAUICHECKBOX pCheck = (PAUICHECKBOX)pObj;
				pCheck->Check(check);
				return;
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(CheckItem);

void IsItemChecked(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.IsItemChecked() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.IsItemChecked() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		AUIObject * pObj = pDlg->GetDlgItem(item_name);
		if (pObj) 
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_CHECKBOX )
			{
				PAUICHECKBOX pCheck = (PAUICHECKBOX)pObj;
				results.push_back(CScriptValue(pCheck->IsChecked()));
				return;
			}
		}
	}
	results.push_back(CScriptValue(false));
}
IMPLEMENT_SCRIPT_API(IsItemChecked);

void CheckRadioButton(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_NUMBER ||
		args[2].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.CheckRadioButton() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.CheckRadioButton() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	int groupid = args[1].GetInt();
	int buttonid = args[2].GetInt();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		pDlg->CheckRadioButton(groupid, buttonid);
	}
}
IMPLEMENT_SCRIPT_API(CheckRadioButton);

void GetCheckedRadioButton(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetCheckedRadioButton() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetCheckedRadioButton() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	int groupid = args[1].GetInt();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		results.push_back(CScriptValue((double)pDlg->GetCheckedRadioButton(groupid)));
	}
}
IMPLEMENT_SCRIPT_API(GetCheckedRadioButton);

void IsButtonClicked(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.IsButtonClicked() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.IsButtonClicked() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj && pObj->GetType() == AUIOBJECT_TYPE_STILLIMAGEBUTTON )
		{
			PAUISTILLIMAGEBUTTON pButton = (PAUISTILLIMAGEBUTTON)pObj;
			results.push_back(CScriptValue(AUISTILLIMAGEBUTTON_STATE_CLICK == pButton->GetState()));
		}
	}
}
IMPLEMENT_SCRIPT_API(IsButtonClicked);

void DeleteListAllItems(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.DeleteListAllItems() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.DeleteListAllItems() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_LISTBOX )
			{
				PAUILISTBOX pList = (PAUILISTBOX)pObj;
				pList->ResetContent();
			}
			else if( pObj->GetType() == AUIOBJECT_TYPE_COMBOBOX )
			{
				PAUICOMBOBOX pCombo = (PAUICOMBOBOX)pObj;
				pCombo->ResetContent();
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(DeleteListAllItems);

void SetListText(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_ARRAY )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetListText() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetListText() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_LISTBOX )
			{
				PAUILISTBOX pList = (PAUILISTBOX)pObj;
				pList->ResetContent();
				for(int i = 0; i < (int)args[2].m_ArrVal.size(); i++)
					if( args[2].m_ArrVal[i].m_Type == CScriptValue::SVT_STRING )
					{
						RETRIEVE_AWSTRING(strText, args[2].m_ArrVal[i])
						pList->AddString(strText);
					}
			}
			else if( pObj->GetType() == AUIOBJECT_TYPE_COMBOBOX )
			{
				PAUICOMBOBOX pCombo = (PAUICOMBOBOX)pObj;
				pCombo->ResetContent();
				for(int i = 0; i < (int)args[2].m_ArrVal.size(); i++)
					if( args[2].m_ArrVal[i].m_Type == CScriptValue::SVT_STRING )
					{
						RETRIEVE_AWSTRING(strText, args[2].m_ArrVal[i])
						pCombo->AddString(strText);
					}
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(SetListText);

void GetListText(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetListText() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetListText() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
		{
			CScriptValue val, ret;
			abase::vector<CScriptValue> vecvalue, veckey;

			if( pObj->GetType() == AUIOBJECT_TYPE_LISTBOX )
			{
				PAUILISTBOX pList = (PAUILISTBOX)pObj;
				for(int i = 0; i < pList->GetCount(); i++)
				{
					SET_SCRIPTVALUE("", pList->GetText(i))
				}
			}
			else if( pObj->GetType() == AUIOBJECT_TYPE_COMBOBOX )
			{
				PAUICOMBOBOX pCombo = (PAUICOMBOBOX)pObj;
				for(int i = 0; i < pCombo->GetCount(); i++)
				{
					SET_SCRIPTVALUE("", pCombo->GetText(i))
				}
			}
			ret.SetArray(vecvalue);
			results.push_back(ret);
		}
	}
}
IMPLEMENT_SCRIPT_API(GetListText);

void InsertListItem(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 4 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER ||
		args[3].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.InsertListItem() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.InsertListItem() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int nIndex = args[2].GetInt();
	RETRIEVE_AWSTRING(text, args[3])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_LISTBOX )
			{
				PAUILISTBOX pList = (PAUILISTBOX)pObj;
				pList->InsertString(nIndex, text);
			}
			else if( pObj->GetType() == AUIOBJECT_TYPE_COMBOBOX )
			{
				PAUICOMBOBOX pCombo = (PAUICOMBOBOX)pObj;
				pCombo->InsertString(nIndex, text);
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(InsertListItem);

void DeleteListItem(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.DeleteListItem() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.DeleteListItem() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int nIndex = args[2].GetInt();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_LISTBOX )
			{
				PAUILISTBOX pList = (PAUILISTBOX)pObj;
				pList->DeleteString(nIndex);
			}
			else if( pObj->GetType() == AUIOBJECT_TYPE_COMBOBOX )
			{
				PAUICOMBOBOX pCombo = (PAUICOMBOBOX)pObj;
				pCombo->DeleteString(nIndex);
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(DeleteListItem);

void SetListItemText(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 4 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER ||
		args[3].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetListItemText() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetListItemText() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int nIndex = args[2].GetInt();
	RETRIEVE_AWSTRING(text, args[3])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_LISTBOX )
			{
				PAUILISTBOX pList = (PAUILISTBOX)pObj;
				pList->SetText(nIndex, text);
			}
			else if( pObj->GetType() == AUIOBJECT_TYPE_COMBOBOX )
			{
				PAUICOMBOBOX pCombo = (PAUICOMBOBOX)pObj;
				pCombo->SetText(nIndex, text);
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(SetListItemText);

void GetListItemText(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER)
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetListItemText() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetListItemText() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int nIndex = args[2].GetInt();

	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
		{
			CScriptValue val;
			if( pObj->GetType() == AUIOBJECT_TYPE_LISTBOX )
			{
				PAUILISTBOX pList = (PAUILISTBOX)pObj;
				val.SetVal(pList->GetText(nIndex));
			}
			else if( pObj->GetType() == AUIOBJECT_TYPE_COMBOBOX )
			{
				PAUICOMBOBOX pCombo = (PAUICOMBOBOX)pObj;
				val.SetVal(pCombo->GetText(nIndex));
			}
			results.push_back(val);
		}
	}
}
IMPLEMENT_SCRIPT_API(GetListItemText);

void SetListItemHint(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 4 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER ||
		args[3].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetListItemHint() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetListItemHint() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int nIndex = args[2].GetInt();
	RETRIEVE_AWSTRING(hint, args[3])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_LISTBOX )
			{
				PAUILISTBOX pList = (PAUILISTBOX)pObj;
				pList->SetItemHint(nIndex, hint);
			}
			else if( pObj->GetType() == AUIOBJECT_TYPE_COMBOBOX )
			{
				PAUICOMBOBOX pCombo = (PAUICOMBOBOX)pObj;
				pCombo->SetItemHint(nIndex, hint);
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(SetListItemHint);

void SetComboDefaultText(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetComboDefaultText() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetComboDefaultText() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	RETRIEVE_AWSTRING(text, args[2])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj && pObj->GetType() == AUIOBJECT_TYPE_COMBOBOX )
		{
			PAUICOMBOBOX pCombo = (PAUICOMBOBOX)pObj;
			pCombo->SetText(-1, text);
		}
	}
}
IMPLEMENT_SCRIPT_API(SetComboDefaultText);

void GetListCurLine(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetListCurLine() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetListCurLine() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_LISTBOX )
			{
				PAUILISTBOX pList = (PAUILISTBOX)pObj;
				results.push_back(CScriptValue((double)pList->GetCurSel()));
			}
			else if( pObj->GetType() == AUIOBJECT_TYPE_COMBOBOX )
			{
				PAUICOMBOBOX pCombo = (PAUICOMBOBOX)pObj;
				results.push_back(CScriptValue((double)pCombo->GetCurSel()));
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(GetListCurLine);

void SetListCurLine(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetListCurLine() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetListCurLine() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int nSel = args[2].GetInt();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_LISTBOX )
			{
				PAUILISTBOX pList = (PAUILISTBOX)pObj;
				pList->SetCurSel(nSel);
			}
			else if( pObj->GetType() == AUIOBJECT_TYPE_COMBOBOX )
			{
				PAUICOMBOBOX pCombo = (PAUICOMBOBOX)pObj;
				pCombo->SetCurSel(nSel);
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(SetListCurLine);

void SetSliderRange(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 4 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER ||
		args[3].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetSliderRange() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetSliderRange() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int minlevel = args[2].GetInt();
	int maxlevel = args[3].GetInt();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj && pObj->GetType() == AUIOBJECT_TYPE_SLIDER )
		{
			PAUISLIDER pSlider = (PAUISLIDER)pObj;
			pSlider->SetTotal(maxlevel, minlevel);
		}
	}
}
IMPLEMENT_SCRIPT_API(SetSliderRange);

void SetSliderPos(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetSliderPos() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetSliderPos() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int pos = args[2].GetInt();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj && pObj->GetType() == AUIOBJECT_TYPE_SLIDER )
		{
			PAUISLIDER pSlider = (PAUISLIDER)pObj;
			pSlider->SetLevel(pos);
		}
	}
}
IMPLEMENT_SCRIPT_API(SetSliderPos);

void GetSliderPos(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetSliderPos() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetSliderPos() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj && pObj->GetType() == AUIOBJECT_TYPE_SLIDER )
		{
			PAUISLIDER pSlider = (PAUISLIDER)pObj;
			results.push_back((double)pSlider->GetLevel());
		}
	}
}
IMPLEMENT_SCRIPT_API(GetSliderPos);

void SetScrollProp(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_ARRAY )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetScrollProp() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetScrollProp() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj && pObj->GetType() == AUIOBJECT_TYPE_SCROLL )
		{
			PAUISCROLL pScroll = (PAUISCROLL)pObj;
			for(int i = 0; i < (int)args[2].m_ArrKey.size(); i++)
				if( args[2].m_ArrKey[i].m_Type == CScriptValue::SVT_STRING )
				{
					RETRIEVE_ASTRING(strText, args[2].m_ArrKey[i])
					if( strText == "max" )
						pScroll->SetScrollRange(0, args[2].m_ArrVal[i].GetInt());
					else if( strText == "pagelevel" )
						pScroll->SetBarLength(args[2].m_ArrVal[i].GetInt());
					else if( strText == "steplevel" )
						pScroll->SetScrollStep(args[2].m_ArrVal[i].GetInt());
					else if( strText == "zoomscroll" )
						pScroll->SetZoomScroll(args[2].m_ArrVal[i].GetBool());
				}
		}
	}
}
IMPLEMENT_SCRIPT_API(SetScrollProp);

void SetScrollPos(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetScrollPos() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetScrollPos() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	int pos = args[2].GetInt();
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if(pObj)
		{
			if(pObj->GetType() == AUIOBJECT_TYPE_SCROLL)
			{
				PAUISCROLL pScroll = (PAUISCROLL)pObj;
				pScroll->SetBarLevel(pos);
			}else if(pObj->GetType() == AUIOBJECT_TYPE_SUBDIALOG)
			{
				AUISubDialog* pSubDialog = (AUISubDialog*)pObj;
				pSubDialog->SetVBarPos(pos);
			}else if(pObj->GetType() == AUIOBJECT_TYPE_LISTBOX)
			{
				AUIListBox* pListBox = (AUIListBox*)pObj;
				if(pos>pListBox->GetScrollLines())
				{
					pListBox->SetFirstItem(pListBox->GetScrollLines());
				}else
				{
					pListBox->SetFirstItem(pos);
				}
			}else if(pObj->GetType() == AUIOBJECT_TYPE_TREEVIEW)
			{
				AUITreeView* pTreeView = (AUITreeView*)pObj;
				pTreeView->SetFirstItem(pos);
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(SetScrollPos);

void GetScrollPos(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetScrollPos() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetScrollPos() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if(pObj)
		{
			int  iScrollPos = -1;
			bool bFind = true;
			if(pObj->GetType() == AUIOBJECT_TYPE_SCROLL)
			{
				PAUISCROLL pScroll = (PAUISCROLL)pObj;
				iScrollPos = pScroll->GetBarLevel();
			}else if(pObj->GetType() == AUIOBJECT_TYPE_SUBDIALOG)
			{
				AUISubDialog* pSubDialog = (AUISubDialog*)pObj;
				iScrollPos = pSubDialog->GetVBarPos();
			}else if(pObj->GetType() == AUIOBJECT_TYPE_LISTBOX)
			{
				AUIListBox* pListBox = (AUIListBox*)pObj;
				iScrollPos = pListBox->GetFirstItem();
			}else if(pObj->GetType() == AUIOBJECT_TYPE_TREEVIEW)
			{
				AUITreeView* pTreeView = (AUITreeView*)pObj;
				iScrollPos = pTreeView->GetFirstItem();
			}else
			{
				bFind = false;
			}
			if(bFind)
			{
				results.push_back((double)iScrollPos);
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(GetScrollPos);

void DeleteTreeAllItems(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.DeleteTreeAllItems() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.DeleteTreeAllItems() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_TREEVIEW )
			{
				PAUITREEVIEW pTree = (PAUITREEVIEW)pObj;
				pTree->DeleteAllItems();
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(DeleteTreeAllItems);

void CreateTreeItem(CScriptValue& val, PAUITREEVIEW pTree, P_AUITREEVIEW_ITEM pItem)
{
	if( val.m_Type == CScriptValue::SVT_ARRAY )
	{
		for(DWORD i = 0; i < val.m_ArrVal.size(); i++)
			if( val.m_ArrVal[i].m_Type == CScriptValue::SVT_STRING )
			{
				RETRIEVE_AWSTRING(text, val.m_ArrVal[i])
				pTree->SetItemText(pItem, text);
			}
			else if( val.m_ArrVal[i].m_Type == CScriptValue::SVT_ARRAY )
			{
				P_AUITREEVIEW_ITEM pChildItem = pTree->InsertItem(_AL(""), pItem);
				CreateTreeItem(val.m_ArrVal[i], pTree, pChildItem);
			}
	}
}

void SetTreeText(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_ARRAY )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetTreeText() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetTreeText() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_TREEVIEW )
			{
				PAUITREEVIEW pTree = (PAUITREEVIEW)pObj;
				pTree->DeleteAllItems();
				CreateTreeItem(args[2], pTree, pTree->GetRootItem());
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(SetTreeText);

void InsertTreeItem(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 4 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_ARRAY ||
		args[3].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.InsertTreeItem() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.InsertTreeItem() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	RETRIEVE_AWSTRING(text, args[3])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_TREEVIEW )
			{
				PAUITREEVIEW pTree = (PAUITREEVIEW)pObj;
				int nDepth = (int)args[2].m_ArrVal.size();
				if( nDepth <= 0 )
					return;

				int *pIndexArray = new int[nDepth];
				for(int i = 0; i < nDepth; i++)
					if( args[2].m_ArrVal[i].m_Type == CScriptValue::SVT_NUMBER)
						pIndexArray[i] = args[2].m_ArrVal[i].GetInt();

				P_AUITREEVIEW_ITEM pParentItem = pTree->GetItemByIndexArray(nDepth - 1, pIndexArray);
				if( !pParentItem )
				{
					delete []pIndexArray;
					return;
				}

				P_AUITREEVIEW_ITEM pItemAfter = pTree->GetItemByIndexArray(nDepth, pIndexArray);
				if( pItemAfter )
					pTree->InsertItem(text, pParentItem, pItemAfter);
				else
					pTree->InsertItem(text, pParentItem);

				delete []pIndexArray;
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(InsertTreeItem);

void DeleteTreeItem(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_ARRAY )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.DeleteTreeItem() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.DeleteTreeItem() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_TREEVIEW )
			{
				PAUITREEVIEW pTree = (PAUITREEVIEW)pObj;
				int nDepth = (int)args[2].m_ArrVal.size();
				if( nDepth <= 0 )
					return;

				int *pIndexArray = new int[nDepth];
				for(int i = 0; i < nDepth; i++)
					if( args[2].m_ArrVal[i].m_Type == CScriptValue::SVT_NUMBER)
						pIndexArray[i] = args[2].m_ArrVal[i].GetInt();

				P_AUITREEVIEW_ITEM pItem = pTree->GetItemByIndexArray(nDepth, pIndexArray);
				if( pItem )
					pTree->DeleteItem(pItem);
				delete []pIndexArray;
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(DeleteTreeItem);

void SetTreeItemText(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 4 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_ARRAY ||
		args[3].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetTreeItemText() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetTreeItemText() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	RETRIEVE_AWSTRING(text, args[3])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_TREEVIEW )
			{
				PAUITREEVIEW pTree = (PAUITREEVIEW)pObj;
				int nDepth = (int)args[2].m_ArrVal.size();
				if( nDepth <= 0 )
					return;

				int *pIndexArray = new int[nDepth];
				for(int i = 0; i < nDepth; i++)
					if( args[2].m_ArrVal[i].m_Type == CScriptValue::SVT_NUMBER)
						pIndexArray[i] = args[2].m_ArrVal[i].GetInt();

				P_AUITREEVIEW_ITEM pItem = pTree->GetItemByIndexArray(nDepth, pIndexArray);
				if( !pItem )
				{
					delete []pIndexArray;
					return;
				}

				pTree->SetItemText(pItem, text);
				delete []pIndexArray;
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(SetTreeItemText);

void GetTreeItemText(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_ARRAY )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetTreeItemText() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetTreeItemText() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_TREEVIEW )
			{
				PAUITREEVIEW pTree = (PAUITREEVIEW)pObj;
				int nDepth = (int)args[2].m_ArrVal.size();
				if( nDepth <= 0 )
					return;

				int *pIndexArray = new int[nDepth];
				for(int i = 0; i < nDepth; i++)
					if( args[2].m_ArrVal[i].m_Type == CScriptValue::SVT_NUMBER)
						pIndexArray[i] = args[2].m_ArrVal[i].GetInt();

				P_AUITREEVIEW_ITEM pItem = pTree->GetItemByIndexArray(nDepth, pIndexArray);
				if( !pItem )
				{
					delete []pIndexArray;
					return;
				}

				CScriptValue val;
				val.SetVal(pTree->GetItemText(pItem));
				results.push_back(val);
				delete []pIndexArray;
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(GetTreeItemText);

void SetTreeItemHint(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 4 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_ARRAY ||
		args[3].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetTreeItemHint() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetTreeItemHint() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	RETRIEVE_AWSTRING(hint, args[3])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_TREEVIEW )
			{
				PAUITREEVIEW pTree = (PAUITREEVIEW)pObj;
				int nDepth = (int)args[2].m_ArrVal.size();
				if( nDepth <= 0 )
					return;

				int *pIndexArray = new int[nDepth];
				for(int i = 0; i < nDepth; i++)
					if( args[2].m_ArrVal[i].m_Type == CScriptValue::SVT_NUMBER)
						pIndexArray[i] = args[2].m_ArrVal[i].GetInt();

				P_AUITREEVIEW_ITEM pItem = pTree->GetItemByIndexArray(nDepth, pIndexArray);
				if( !pItem )
				{
					delete []pIndexArray;
					return;
				}

				pTree->SetItemHint(pItem, hint);
				delete []pIndexArray;
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(SetTreeItemHint);

void GetTreeSelectItem(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetTreeSelectItem() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetTreeSelectItem() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj )
		{
			if( pObj->GetType() == AUIOBJECT_TYPE_TREEVIEW )
			{
				PAUITREEVIEW pTree = (PAUITREEVIEW)pObj;
				CScriptValue val, ret;
				abase::vector<CScriptValue> vecvalue, veckey;

				P_AUITREEVIEW_ITEM pItem = pTree->GetSelectedItem();
				if( !pItem )
					return;
	
				val.SetVal(pTree->GetItemText(pItem));
				results.push_back(val);
				P_AUITREEVIEW_ITEM pParentItem = pTree->GetParentItem(pItem);
				while( pParentItem )
				{
					P_AUITREEVIEW_ITEM pChildItem = pTree->GetFirstChildItem(pParentItem);
					int i = 0;
					while( pChildItem && pChildItem != pItem )
					{
						pChildItem = pTree->GetNextSiblingItem(pChildItem);
						i++;
					}
					val.SetVal(i);
					vecvalue.insert(vecvalue.begin(), val);
					pItem = pParentItem;
					pParentItem = pTree->GetParentItem(pItem);
				}

				val.SetArray(vecvalue);
				results.push_back(val);
			}
		}
	}
}
IMPLEMENT_SCRIPT_API(GetTreeSelectItem);


void IsKeyPress(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 1 ||
		args[0].m_Type != CScriptValue::SVT_NUMBER )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.IsKeyPress() parameters error");
		return;
	}
	
	int vk = args[0].GetInt();
	results.push_back(CScriptValue(AUI_PRESS(vk) != 0));
}
IMPLEMENT_SCRIPT_API(IsKeyPress);

void AddConsoleLine(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 4 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_STRING ||
		args[3].m_Type != CScriptValue::SVT_NUMBER)
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.AddConsoleLine() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.AddConsoleLine() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	RETRIEVE_AWSTRING(line, args[2])
	A3DCOLOR  col = (A3DCOLOR)args[3].GetDouble();

	AUIDialog * pDlg = (AUIDialog * )pUIMan->GetDialog(dlg_name);
	if (pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if( pObj && pObj->GetType() == AUIOBJECT_TYPE_CONSOLE )
		{
			PAUICONSOLE pConsole = (PAUICONSOLE)pObj;
			pConsole->AddLine(line, col);
		}
	}
}
IMPLEMENT_SCRIPT_API(AddConsoleLine);

void CreateControl(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING||
		args[1].m_Type != CScriptValue::SVT_STRING||
		args[2].m_Type != CScriptValue::SVT_STRING)
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.CreateControl() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.CreateControl() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])
	RETRIEVE_ASTRING(new_name, args[2])

	AUIDialog * pDlg = (AUIDialog* )pUIMan->GetDialog(dlg_name);
	if(pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if(pObj)
		{
			PAUIOBJECT pNewObj = pDlg->CloneControl(pObj);
			pNewObj->SetName(const_cast<char *>((const char*)(new_name)));
		}
	}
}
IMPLEMENT_SCRIPT_API(CreateControl);

void DeleteControl(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if(	args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING||
		args[1].m_Type != CScriptValue::SVT_STRING)
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.DeleteControl() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.DeleteControl() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])

	AUIDialog * pDlg = (AUIDialog* )pUIMan->GetDialog(dlg_name);
	if(pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		if(pObj)
		{
			pDlg->DeleteControl(pObj);
		}
	}
}
IMPLEMENT_SCRIPT_API(DeleteControl);

void GetFixFrame(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if(	args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING||
		args[1].m_Type != CScriptValue::SVT_STRING)
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetFixFrame() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetFixFrame() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	RETRIEVE_ASTRING(item_name, args[1])

	AUIDialog * pDlg = (AUIDialog* )pUIMan->GetDialog(dlg_name);
	if(pDlg)
	{
		PAUIOBJECT pObj = pDlg->GetDlgItem(item_name);
		PAUIIMAGEPICTURE pImg = NULL;
		if(pObj && pObj->GetType() == AUIOBJECT_TYPE_IMAGEPICTURE && NULL != (pImg=dynamic_cast<PAUIIMAGEPICTURE>(pObj)))
		{
			CScriptValue val;
			val.SetVal(pImg->GetFixFrame());
			results.push_back(val);
		}
	}
}
IMPLEMENT_SCRIPT_API(GetFixFrame);


void SetTickCB(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if(	args.size() < 2 ||
		args[0].m_Type != CScriptValue::SVT_STRING||
		args[1].m_Type != CScriptValue::SVT_BOOL)
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetTickCB() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetTickCB() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	const bool bTickCB = args[1].GetBool();

	AUILuaDialog* pDlg = dynamic_cast<AUILuaDialog*>(pUIMan->GetDialog(dlg_name));
	if(pDlg)
	{
		pDlg->SetTickCB(bTickCB);
	}else
	{
		if(AUI_IsLogEnabled())
		{		
			a_LogOutput(1, "SetTickCB dialog name error: %s", dlg_name);
		}
	}
}
IMPLEMENT_SCRIPT_API(SetTickCB);

void SetObjProperty(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 4 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetObjProperty() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetObjProperty() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	AUILuaDialog* pDlg = dynamic_cast<AUILuaDialog*>(pUIMan->GetDialog(dlg_name));
	if(!pDlg)
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetObjProperty() dialog name error");
		return;
	}
	
	RETRIEVE_ASTRING(obj_name, args[1])
	AUIObject* pObj = pDlg->GetDlgItem(obj_name);
	if (!pObj)
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetObjProperty() object not found");
		return;
	}
		
	RETRIEVE_ASTRING(property_name, args[2])
	AUIOBJECT_PROPERTY a_Property[AUIOBJECT_MAXPROPERTYNUM];
	int nNumProperties = AUI_RetrieveControlPropertyList(pObj->GetType(), a_Property);
	bool bFindProperty = false;
	int nPropertyType = 0;
	for (int i=0; i<nNumProperties; i++)
	{
		if (0 == _strcmpi(property_name, a_Property[i].szName))
		{
			bFindProperty = true;
			nPropertyType = a_Property[i].nType;
			break;
		}
	}
	if (!bFindProperty)
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetObjProperty() property not found in object");
		return;
	}

	AUIOBJECT_SETPROPERTY setProp;
	ZeroMemory(&setProp, sizeof(AUIOBJECT_SETPROPERTY));
	switch (nPropertyType)
	{
	case AUIOBJECT_PROPERTY_ALIGN:
	case AUIOBJECT_PROPERTY_ALIGNY:
	case AUIOBJECT_PROPERTY_FRAMEMODE:
	case AUIOBJECT_PROPERTY_UNDERLINEMODE:
	case AUIOBJECT_PROPERTY_INT:
		if (args[3].m_Type != CScriptValue::SVT_NUMBER)
		{
			AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetObjProperty() property's value should be a number");
			return;
		}
		setProp.i = args[3].GetInt();
		break;

	case AUIOBJECT_PROPERTY_COLOR:
		if (args[3].m_Type != CScriptValue::SVT_NUMBER)
		{
			AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetObjProperty() property's value should be a number");
			return;
		}
		setProp.dw = args[3].GetInt();
		break;

	case AUIOBJECT_PROPERTY_BOOL:
		if (args[3].m_Type != CScriptValue::SVT_BOOL)
		{
			AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetObjProperty() property's value should be a boolean");
			return;
		}
		setProp.dw = args[3].GetBool();
		break;

	case AUIOBJECT_PROPERTY_FLOAT:
		if (args[3].m_Type != CScriptValue::SVT_NUMBER)
		{
			AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetObjProperty() property's value should be a number");
			return;
		}
		setProp.f = (float)args[3].GetDouble();
		break;

	case AUIOBJECT_PROPERTY_FONT:
		{
			if (args.size() < 7 ||
				args[3].m_Type != CScriptValue::SVT_STRING ||
				args[4].m_Type != CScriptValue::SVT_NUMBER ||
				args[5].m_Type != CScriptValue::SVT_BOOL ||
				args[6].m_Type != CScriptValue::SVT_BOOL)
			{
				AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetObjProperty() property font value is invalid");
				return;
			}
#ifdef UNICODE
			RETRIEVE_AWSTRING(font_name, args[3]);
#else
			RETRIEVE_ASTRING(font_name, args[3]);
#endif
			a_strncpy(setProp.font.szName, font_name.GetBuffer(-1), AFILE_LINEMAXLEN);
			font_name.ReleaseBuffer();
			setProp.font.nHeight = args[4].GetInt();
			setProp.font.bBold = args[5].GetBool();
			setProp.font.bItalic = args[6].GetBool();
			break;
		}
	
	case AUIOBJECT_PROPERTY_STRING:
		{
#ifdef UNICODE
			RETRIEVE_AWSTRING(strText, args[3]);
#else
			RETRIEVE_ASTRING(strText, args[3]);
#endif
			a_strncpy(setProp.s, strText.GetBuffer(-1), AFILE_LINEMAXLEN);
			strText.ReleaseBuffer();
		}		
		break;

	case AUIOBJECT_PROPERTY_CHAR:
	case AUIOBJECT_PROPERTY_COMMAND:
	case AUIOBJECT_PROPERTY_FILENAME:
	case AUIOBJECT_PROPERTY_INTERFACE_FILENAME:
	case AUIOBJECT_PROPERTY_SURFACE_FILENAME:
	case AUIOBJECT_PROPERTY_GFX_FILENAME:
	case AUIOBJECT_PROPERTY_SFX_FILENAME:
		{
			if (args[3].m_Type != CScriptValue::SVT_STRING)
			{
				AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetObjProperty() property's value should be a string");
				return;
			}
			RETRIEVE_ASTRING(property_ch, args[3])
				strncpy(setProp.c, property_ch, AFILE_LINEMAXLEN);
		}
		
		break;
	}

	if (!pObj->SetProperty(property_name, &setProp))
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.SetObjProperty() Internal Error");
		return;
	}
}
IMPLEMENT_SCRIPT_API(SetObjProperty);

void GetObjProperty(vector<CScriptValue>& args, vector<CScriptValue>& results)
{
	if( args.size() < 3 ||
		args[0].m_Type != CScriptValue::SVT_STRING ||
		args[1].m_Type != CScriptValue::SVT_STRING ||
		args[2].m_Type != CScriptValue::SVT_STRING )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetObjProperty() parameters error");
		return;
	}

	PAUIMANAGER pUIMan = GetAuiManager();
	if( !pUIMan )
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetObjProperty() no ui man");
		return;
	}

	RETRIEVE_ASTRING(dlg_name, args[0])
	AUILuaDialog* pDlg = dynamic_cast<AUILuaDialog*>(pUIMan->GetDialog(dlg_name));
	if(!pDlg)
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetObjProperty() dialog name error");
		return;
	}
	
	RETRIEVE_ASTRING(obj_name, args[1])
	AUIObject* pObj = pDlg->GetDlgItem(obj_name);
	if (!pObj)
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetObjProperty() object not found");
		return;
	}
		
	RETRIEVE_ASTRING(property_name, args[2])
	AUIOBJECT_PROPERTY a_Property[AUIOBJECT_MAXPROPERTYNUM];
	int nNumProperties = AUI_RetrieveControlPropertyList(pObj->GetType(), a_Property);
	bool bFindProperty = false;
	int nPropertyType = 0;
	for (int i=0; i<nNumProperties; i++)
	{
		if (0 == _strcmpi(property_name, a_Property[i].szName))
		{
			bFindProperty = true;
			nPropertyType = a_Property[i].nType;
			break;
		}
	}
	if (!bFindProperty)
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetObjProperty() property not found in object");
		return;
	}

	AUIOBJECT_SETPROPERTY setProp;
	ZeroMemory(&setProp, sizeof(AUIOBJECT_SETPROPERTY));
	if (!pObj->GetProperty(property_name, &setProp))
	{
		AUI_REPORT_ERR_LUA_FUNC("LuaDlgApi.GetObjProperty() Internal error");
		return;
	}

	CScriptValue val;
	switch (nPropertyType)
	{
	case AUIOBJECT_PROPERTY_ALIGN:
	case AUIOBJECT_PROPERTY_ALIGNY:
	case AUIOBJECT_PROPERTY_FRAMEMODE:
	case AUIOBJECT_PROPERTY_UNDERLINEMODE:
	case AUIOBJECT_PROPERTY_INT:
		val.SetVal(setProp.i);
		results.push_back(val);
		break;

	case AUIOBJECT_PROPERTY_COLOR:
		val.SetVal((int)setProp.dw);
		results.push_back(val);
		break;

	case AUIOBJECT_PROPERTY_BOOL:
		val.SetVal(setProp.b);
		results.push_back(val);
		break;

	case AUIOBJECT_PROPERTY_FLOAT:
		val.SetVal((double)setProp.f);
		results.push_back(val);
		break;

	case AUIOBJECT_PROPERTY_FONT:
		val.SetVal(setProp.font.szName);
		results.push_back(val);
		val.SetVal(setProp.font.nHeight);
		results.push_back(val);
		val.SetVal(setProp.font.bBold);
		results.push_back(val);
		val.SetVal(setProp.font.bItalic);
		results.push_back(val);
		break;

	case AUIOBJECT_PROPERTY_STRING:
		val.SetVal(setProp.s);
		results.push_back(val);
		break;

	case AUIOBJECT_PROPERTY_CHAR:
	case AUIOBJECT_PROPERTY_COMMAND:
	case AUIOBJECT_PROPERTY_FILENAME:
	case AUIOBJECT_PROPERTY_INTERFACE_FILENAME:
	case AUIOBJECT_PROPERTY_SURFACE_FILENAME:
	case AUIOBJECT_PROPERTY_GFX_FILENAME:
	case AUIOBJECT_PROPERTY_SFX_FILENAME:
		val.SetVal((char*)setProp.c);
		results.push_back(val);
		break;
	}
}
IMPLEMENT_SCRIPT_API(GetObjProperty);

void InitDlgApi(CLuaState * pState)
{
	REGISTER_SCRIPT_API(DlgApi, PopupMessageBox);
	REGISTER_SCRIPT_API(DlgApi, RespawnMessage);
	REGISTER_SCRIPT_API(DlgApi, ConvertChatString);
	REGISTER_SCRIPT_API(DlgApi, GetWindowScale);
	REGISTER_SCRIPT_API(DlgApi, GetWindowRect);
	REGISTER_SCRIPT_API(DlgApi, GetCurrentIMEName);
	REGISTER_SCRIPT_API(DlgApi, ShowDialog);
	REGISTER_SCRIPT_API(DlgApi, IsDialogShow);
	REGISTER_SCRIPT_API(DlgApi, IsDialogActive);
	REGISTER_SCRIPT_API(DlgApi, EnableDialog);
	REGISTER_SCRIPT_API(DlgApi, SetDialogPosition);
	REGISTER_SCRIPT_API(DlgApi, SetDialogSize);
	REGISTER_SCRIPT_API(DlgApi, SetDialogScale);
	REGISTER_SCRIPT_API(DlgApi, SetDialogAlpha);
	REGISTER_SCRIPT_API(DlgApi, SetDialogWholeAlpha);
	REGISTER_SCRIPT_API(DlgApi, SetDialogFrame);
	REGISTER_SCRIPT_API(DlgApi, SetDialogTimer);
	REGISTER_SCRIPT_API(DlgApi, SetDialogProp);
	REGISTER_SCRIPT_API(DlgApi, GetDialogProp);

	REGISTER_SCRIPT_API(DlgApi, GetHitItem);
	REGISTER_SCRIPT_API(DlgApi, GetNameLink);
	REGISTER_SCRIPT_API(DlgApi, GetTextAreaLineInfo);
	REGISTER_SCRIPT_API(DlgApi, ChangeFocus);
	REGISTER_SCRIPT_API(DlgApi, ShowItem);
	REGISTER_SCRIPT_API(DlgApi, IsItemShow);
	REGISTER_SCRIPT_API(DlgApi, SetItemPos);
	REGISTER_SCRIPT_API(DlgApi, SetItemSize);
	REGISTER_SCRIPT_API(DlgApi, GetItemRect);
	REGISTER_SCRIPT_API(DlgApi, EnableItem);
	REGISTER_SCRIPT_API(DlgApi, IsItemEnabled);
	REGISTER_SCRIPT_API(DlgApi, SetItemText);
	REGISTER_SCRIPT_API(DlgApi, GetItemText);
	REGISTER_SCRIPT_API(DlgApi, AppendItemText);
	REGISTER_SCRIPT_API(DlgApi, AppendEmotion);
	REGISTER_SCRIPT_API(DlgApi, SerializeItemText);
	REGISTER_SCRIPT_API(DlgApi, FilterEmotionSet);
	REGISTER_SCRIPT_API(DlgApi, SetItemMask);
	REGISTER_SCRIPT_API(DlgApi, SetOnlyShowText);
	REGISTER_SCRIPT_API(DlgApi, ScrollToTop);
	REGISTER_SCRIPT_API(DlgApi, ScrollToBottom);
	REGISTER_SCRIPT_API(DlgApi, SetReadOnly);
	REGISTER_SCRIPT_API(DlgApi, SetItemColor);
	REGISTER_SCRIPT_API(DlgApi, SetItemAlpha);
	REGISTER_SCRIPT_API(DlgApi, SetItemHint);
	REGISTER_SCRIPT_API(DlgApi, FlashItem);
	REGISTER_SCRIPT_API(DlgApi, SetEmotionSet);
	REGISTER_SCRIPT_API(DlgApi, SetMarquee);
	REGISTER_SCRIPT_API(DlgApi, IsMarquee);
	REGISTER_SCRIPT_API(DlgApi, SetProgress);
	REGISTER_SCRIPT_API(DlgApi, SetAutoProgress);
	REGISTER_SCRIPT_API(DlgApi, SetActionProgress);
	REGISTER_SCRIPT_API(DlgApi, GetEmotionsInfo);
	REGISTER_SCRIPT_API(DlgApi, SetImageEmotion);
	REGISTER_SCRIPT_API(DlgApi, SetImageRenderTop);
	REGISTER_SCRIPT_API(DlgApi, SetImageFile);
	REGISTER_SCRIPT_API(DlgApi, SelectImageFrame);
	REGISTER_SCRIPT_API(DlgApi, CheckItem);
	REGISTER_SCRIPT_API(DlgApi, IsItemChecked);
	REGISTER_SCRIPT_API(DlgApi, CheckRadioButton);
	REGISTER_SCRIPT_API(DlgApi, GetCheckedRadioButton);
	REGISTER_SCRIPT_API(DlgApi, IsButtonClicked);
	REGISTER_SCRIPT_API(DlgApi, DeleteListAllItems);
	REGISTER_SCRIPT_API(DlgApi, SetListText);
	REGISTER_SCRIPT_API(DlgApi, GetListText);
	REGISTER_SCRIPT_API(DlgApi, InsertListItem);
	REGISTER_SCRIPT_API(DlgApi, DeleteListItem);
	REGISTER_SCRIPT_API(DlgApi, SetListItemText);
	REGISTER_SCRIPT_API(DlgApi, GetListItemText);
	REGISTER_SCRIPT_API(DlgApi, SetListItemHint);
	REGISTER_SCRIPT_API(DlgApi, GetListCurLine);
	REGISTER_SCRIPT_API(DlgApi, SetListCurLine);
	REGISTER_SCRIPT_API(DlgApi, SetComboDefaultText);
	REGISTER_SCRIPT_API(DlgApi, SetSliderRange);
	REGISTER_SCRIPT_API(DlgApi, SetSliderPos);
	REGISTER_SCRIPT_API(DlgApi, GetSliderPos);
	REGISTER_SCRIPT_API(DlgApi, SetScrollProp);
	REGISTER_SCRIPT_API(DlgApi, SetScrollPos);
	REGISTER_SCRIPT_API(DlgApi, GetScrollPos);
	REGISTER_SCRIPT_API(DlgApi, DeleteTreeAllItems);
	REGISTER_SCRIPT_API(DlgApi, SetTreeText);
	REGISTER_SCRIPT_API(DlgApi, InsertTreeItem);
	REGISTER_SCRIPT_API(DlgApi, DeleteTreeItem);
	REGISTER_SCRIPT_API(DlgApi, SetTreeItemText);
	REGISTER_SCRIPT_API(DlgApi, GetTreeItemText);
	REGISTER_SCRIPT_API(DlgApi, SetTreeItemHint);
	REGISTER_SCRIPT_API(DlgApi, GetTreeSelectItem);
	REGISTER_SCRIPT_API(DlgApi, GetNameLink);
	REGISTER_SCRIPT_API(DlgApi, GetItemLink);
	REGISTER_SCRIPT_API(DlgApi, GetTextAreaLineInfo);
	REGISTER_SCRIPT_API(DlgApi, OnCommand);
	
	REGISTER_SCRIPT_API(DlgApi, IsKeyPress);
	REGISTER_SCRIPT_API(DlgApi, AddConsoleLine);

	REGISTER_SCRIPT_API(DlgApi, CreateControl);
	REGISTER_SCRIPT_API(DlgApi, DeleteControl);

	REGISTER_SCRIPT_API(DlgApi, GetFixFrame);
	REGISTER_SCRIPT_API(DlgApi, SetTickCB);

	REGISTER_SCRIPT_API(DlgApi, GetObjProperty);
	REGISTER_SCRIPT_API(DlgApi, SetObjProperty);

	pState->RegisterLibApi("DlgApi");
}


