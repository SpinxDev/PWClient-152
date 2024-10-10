// Filename	: DlgNameLink.h
// Creator	: Feng Ning
// Date		: 2010/05/10

#pragma once

#include "DlgBase.h"
#include "AUITextArea.h"
#include "hashmap.h"
#include "EC_Game.h"

struct Task_Region;

// Derived from this class will make all AUI controls named as
// "Txt_Link_*" to handle the namelink event automatically.
class CDlgNameLink : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	class LinkCommand;

	void OnEventLButtonClick_NameLink(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventMouseMove_NameLink(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	
	virtual bool Release();
	LinkCommand* GetLinkCommand(PAUITEXTAREA pArea, const ACString* pName);

protected:
	// set NULL cmd to unbind
	bool BindLinkCommand(PAUITEXTAREA pArea, const ACString* pName, const LinkCommand* pCmdType);
	void ClearCommands();

	virtual void OnEventMouseMove(int x, int y, DWORD keys, const P_AUITEXTAREA_NAME_LINK pLink) {};
	virtual void OnEventLButtonClick(int x, int y, DWORD keys, const P_AUITEXTAREA_NAME_LINK pLink) {};
	virtual bool PtInRect(const AUITEXTAREA_NAME_LINK &name, int x, int y);

private:
	bool GetNameLinkMouseOn(int x, int y, PAUITEXTAREA pText, P_AUITEXTAREA_NAME_LINK pLink, int* pArea );

	typedef abase::hash_map<ACString, LinkCommand*> CommandMap;
	CommandMap m_Commands;

public:
	
	// command pattern for click a hyper link in textarea
	class LinkCommand
	{
		friend CDlgNameLink;

	public:
		virtual bool operator()(P_AUITEXTAREA_NAME_LINK pLink) = 0;
		virtual ~LinkCommand(){};

	protected:
		LinkCommand();

		PAUITEXTAREA GetTxtArea() const {return m_pArea;}
		virtual void AppendText();

		virtual ACString GetLinkText()  const = 0;
		virtual LinkCommand* Clone() const = 0;

	private:
		LinkCommand(const LinkCommand&);
		LinkCommand& operator=(const LinkCommand&);
		PAUITEXTAREA m_pArea;
	};

	// click this textarea will cause player move to the specific target
	class MoveToLinkCommand : public LinkCommand
	{
	public:
		virtual bool operator()(P_AUITEXTAREA_NAME_LINK pLink);
		MoveToLinkCommand(int idTarget, const ACString &targetName, int idTask = 0);
		MoveToLinkCommand(int idTarget, const A3DVECTOR3& targetPos, const ACString &targetName, int idTask = 0);
		MoveToLinkCommand(const abase::vector<CECGame::OBJECT_COORD>& targets, const ACString &targetName);
		MoveToLinkCommand(const MoveToLinkCommand& rhs);

		virtual ACString GetLinkText() const;

	protected:
		virtual LinkCommand* Clone() const;

		ACString m_TargetName;
		A3DVECTOR3 m_TargetPos;
		abase::vector<CECGame::OBJECT_COORD> m_Targets;
		int m_TargetId;
		int m_TaskId;
	};
};