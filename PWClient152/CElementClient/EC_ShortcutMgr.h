// Filename	: EC_ShortcutMgr.h
// Creator	: Feng Ning
// Date		: 2011/07/11

#pragma once

#include "AString.h"
#include "hashmap.h"
#include "vector.h"

//------------------------------------------------------------------------
// Class Declaration
//------------------------------------------------------------------------
class CDlgBase;
class AUIDialog;
class AUIObject;
class CECIvtrItem;

//------------------------------------------------------------------------
// Manager for all Right-Click Shortcuts
//------------------------------------------------------------------------
class CECShortcutMgr
{
public:
	CECShortcutMgr();
	~CECShortcutMgr();
		
	//------------------------------------------------------------------------
	// Right-Click Shortcut Used in Dialogs
	//------------------------------------------------------------------------
	class ClickShortcut
	{
	public:
		virtual ~ClickShortcut() {};
		
		// which group this shortcut will be registered
		virtual const char* GetGroupName() { return NULL; }
		
		// whether to trigger this shortcut
		// true means this shortcut will be triggered and other shortcuts will be ignored
		virtual bool CanTrigger(AUIObject* pSrcObj) = 0;

		// whether invoke the number input dialog
		// return 1 means no further process ( by DEFAULT )
		// return >1 means number input dialog will be invoked
		// return 0 means number limitation is reached, triggered but do nothing
		virtual int CheckNumber(AUIObject* pSrcObj) { return 1; }
		
		// trigger this shortcut 
		virtual void Trigger(AUIObject* pSrcObj, int num) = 0;
	};
	
	//------------------------------------------------------------------------
	// Right-Click Shortcut for Simple Usage
	//------------------------------------------------------------------------
	class SimpleClickShortcutBase : public ClickShortcut
	{
	public:
		// default name constrain is used in Win_Inventory
		virtual const char* GetItemName();
		virtual const char* GetItemPrefix();
		virtual const char* GetGroupName();

		// for get back the dialog pointer
		virtual CDlgBase* GetDialog();

		// from base class
		virtual int CheckNumber(AUIObject* pSrcObj);
		virtual bool CanTrigger(AUIObject* pSrcObj);
		virtual void Trigger(AUIObject* pSrcObj, int num);

	protected:
		SimpleClickShortcutBase(){};
		SimpleClickShortcutBase(const SimpleClickShortcutBase&){};
	};

	//------------------------------------------------------------------------
	// Template Right-Click Shortcut for Simple Usage
	//------------------------------------------------------------------------
	template<class DlgType> class SimpleClickShortcut : public SimpleClickShortcutBase
	{
	protected:
		DlgType* m_pDlg;
	public:
		SimpleClickShortcut(DlgType* pDlg) :m_pDlg(pDlg) {}
		virtual CDlgBase* GetDialog() { return m_pDlg; }
	};

public:
	
	// helper function, find a target for drag-drop or right-click
	static bool CheckDragClickTarget(AUIDialog* pDlg, AUIObject*& pObjOver, const char* pName);

	bool RegisterShortCut(ClickShortcut* pCS);
	bool TriggerShortcuts(const char* pGroupName, AUIObject* pSrcObj);

protected:
	typedef abase::vector<ClickShortcut*> ShortCuts;
	typedef abase::hash_map<AString, ShortCuts*> ShortCutsMap;

	ShortCutsMap m_Shortcuts;
};