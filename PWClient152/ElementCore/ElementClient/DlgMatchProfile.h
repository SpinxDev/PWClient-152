/*
 * FILE: DlgMatchProfile.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: WYD
 *
 * HISTORY: 
 *
 * Copyright (c) 2013, All Rights Reserved.
 */
 
#pragma once

#include "DlgBase.h"
#include <AUIDialog.h>
#include <AUICheckBox.h>

static const char* s_pCheckName[4][9] = {
	{"Chk_Time1","Chk_Time2","Chk_Time3","Chk_Time4",NULL,NULL,NULL,NULL,NULL},
	{"Chk_Like1","Chk_Like2","Chk_Like3","Chk_Like4","Chk_Like5",NULL,NULL,NULL,NULL},
	{"Chk_hobby1","Chk_hobby2","Chk_hobby3","Chk_hobby4","Chk_hobby5","Chk_hobby6","Chk_hobby7","Chk_hobby8","Chk_hobby9"},
	{"Chk_1","Chk_2","Chk_3",NULL,NULL,NULL,NULL,NULL,NULL},
};

enum
{
	MATCH_TYPE_TIMESPANE = 0,
	MATCH_TYPE_GAMEPLAY,
	MATCH_TYPE_HOBBY,
	MATCH_TYPE_SETTING,
};

enum
{
	MATCH_TYPE_TIMESPANE_NUM = 4,
	MATCH_TYPE_GAMEPLAY_NUM = 5,
	MATCH_TYPE_HOBBY_NUM = 9,
	MATCH_TYPE_SETTING_NUM = 3,
};

template< int type,int num>
class CMatchChecker
{
	enum
	{
		m_type = type,
		m_count = num,
	};
public:
	CMatchChecker(){ memset(m_pChk,0,sizeof(m_pChk));};
	void SetMask(int m) { m_mask = m;}
	int GetMask() const { return m_mask;}
	void UpdateData(bool b)
	{
		if (b)
		{
			m_mask = 0;
			for(int i=0;i<m_count;i++)
			{
				if(m_pChk[i] && m_pChk[i]->IsChecked())
					m_mask |= (1<<i);
			}
		}
		else
		{
			for(int i=0;i<m_count;i++)
			{
				if(m_pChk[i])
					m_pChk[i]->Check(((1<<i) & m_mask) !=0);
			}
		}
	}

	void OnInitControl(PAUIDIALOG pDlg)
	{
		if(!pDlg) return;

		m_mask = 0;
		for (int i=0;i<m_count;i++)
		{
			m_pChk[i] = dynamic_cast<PAUICHECKBOX>(pDlg->GetDlgItem(s_pCheckName[m_type][i]));
		}
	}
	
	bool IsCompletedInfo() const
	{ 
		for (int i=0;i<m_count;i++)
		{
			if(((1<<i) & m_mask) != 0)
				return true;
		}
		return false;
	}

protected:
	PAUICHECKBOX m_pChk[m_count];
	int m_mask;
};

class CDlgMatchProfileHabit : public CDlgBase
{	
	AUI_DECLARE_COMMAND_MAP()

public:
	void OnCommandSave(const char *szCommand);

	void OnCommandHobby(const char* szCommand);
	void OnCommandAge(const char* szCommand);
	void OnCommandSetting(const char* szCommand);

	void SetMaskTimeSpan(int m) { m_checker1.SetMask(m);}
	void SetMaskGamePlay(int m) { m_checker2.SetMask(m);}

	int GetMaskTimeSpane() const { return m_checker1.GetMask();}
	int GetMaskGamePlay() const { return m_checker2.GetMask();}
	
	bool IsCompletedProfile() const { return m_checker1.IsCompletedInfo() && m_checker2.IsCompletedInfo();}
	static void ToggleDlg(PAUIDIALOG pOld,PAUIDIALOG pNew);

protected:
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual bool OnInitDialog();

protected:
	CMatchChecker<MATCH_TYPE_TIMESPANE,MATCH_TYPE_TIMESPANE_NUM> m_checker1;
	CMatchChecker<MATCH_TYPE_GAMEPLAY,MATCH_TYPE_GAMEPLAY_NUM> m_checker2;
};

//////////////////////////////////////////////////////////////////////////
class CDlgMatchProfileHobby : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()

public:
	void OnCommandSave(const char *szCommand);
	
	void OnCommandHabit(const char* szCommand);
	void OnCommandAge(const char* szCommand);
	void OnCommandSetting(const char* szCommand);

	void SetMaskHobby(int m) { m_checker.SetMask(m);}	
	int GetMaskHobby() const { return m_checker.GetMask();}

	bool IsCompletedProfile() const { return m_checker.IsCompletedInfo();}

protected:
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual bool OnInitDialog();	

protected:
	CMatchChecker<MATCH_TYPE_HOBBY,MATCH_TYPE_HOBBY_NUM> m_checker;
};
//////////////////////////////////////////////////////////////////////////
class CDlgMatchProfileAge : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
public:
	
	void OnCommandSave(const char *szCommand);

	void OnCommandHabit(const char* szCommand);
	void OnCommandHobby(const char* szCommand);
	void OnCommandSetting(const char* szCommand);
	
	void SetAgeHoro(int a,int h) { m_age = a; m_horoscope = h;}
	int GetAge() const { return m_age;}
	int GetHoroscope () const { return m_horoscope;}

	bool IsCompletedProfile() const { return m_age !=0 && m_horoscope !=0;}
	
protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnHideDialog();
	virtual void DoDataExchange(bool bSave);

protected:
	int m_age;
	int m_horoscope;
};
//////////////////////////////////////////////////////////////////////////
class CDlgMatchProfileSetting : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
public:		
	void OnCommandSave(const char *szCommand);

	void OnCommandHabit(const char* szCommand);
	void OnCommandAge(const char* szCommand);
	void OnCommandHobby(const char* szCommand);

	void OnCommandCANCEL(const char *szCommand);

	bool GetProfile(bool bShowErrorMessage, bool bShowUIOnReturn);
	bool ShowUIOnGetProfileReturn()const{ return m_bShowUIOnGetProfileReturn; }
	
	void SetMask(int m) { m_checker.SetMask(m);}	
	int  GetMask() const { return m_checker.GetMask();}

	void SetServerMask(unsigned short uMask);
	unsigned short GetServerMask()const{ return m_uServerMask; }
	bool IsServerMaskInitialized()const{ return m_bServerMaskInited; }
	
	bool IsCompletedProfile() const { return m_checker.IsCompletedInfo();}

protected:
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual bool OnInitDialog();

protected:
	CMatchChecker<MATCH_TYPE_SETTING,MATCH_TYPE_SETTING_NUM> m_checker;

	bool	m_bServerMaskInited;
	unsigned short m_uServerMask;

	bool	m_bShowUIOnGetProfileReturn;
};
