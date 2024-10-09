// Filename	: DlgTheme.h
// Creator	: Feng Ning
// Date		: October 19, 2010

#pragma once

#include "AUILuaDialog.h"

#include <vector.h>
#include "hashmap.h"

#define AUI_SWAP_RES(res)	lhs->GetProperty(res, &lhsProp);	\
							rhs->GetProperty(res, &rhsProp);	\
							lhs->SetProperty(res, &rhsProp);	\
							rhs->SetProperty(res, &lhsProp);	\
							//

class CECUIAnimation;

class CDlgTheme : public AUILuaDialog  
{
public:
	CDlgTheme();
	virtual ~CDlgTheme();

	virtual bool ChangeLayout(CDlgTheme* pDlg);

	void	RegisterEnableDelay(const char *szObjName, DWORD dwDelayTime = 300);
	void	UnregisterEnableDelay(const char *szObjName);
	// �ڰ�ť�ϲ���һ��gfx��Ч�������ֹͣ����(���ܶ��Ѿ�������PushLike�İ�ťʹ���������)
	void StartButtonGfx(const char* button, const char* gfx);
	void OnGfxButtonClicked(const char *szCommand);
	
	virtual bool Render();

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual void OnTick();

	// Load specific animation
	CECUIAnimation* StartAnimation(const char* suffix);

	//////////////////////////////////////////////////////////////////////////
	// Handle the Object Transfer
	//////////////////////////////////////////////////////////////////////////
	bool RemoveControl(PAUIOBJECT pAUIObject);
	bool TransferControl(PAUIOBJECT pAUIObject, CDlgTheme* pDlg);
	bool AddControlBySequence(PAUIOBJECT pObj, CDlgTheme* pTheirDlg);
	bool AddControlBefore(PAUIOBJECT pAUIObject, const AString& objName);
	AString FindNextControl(const AString& name);
	void Resort();

	//////////////////////////////////////////////////////////////////////////
	// Callbacks for Layout Changing
	//////////////////////////////////////////////////////////////////////////
	// return false to cancel the change action on current object
	virtual bool OnChangeLayout(PAUIOBJECT pMine, PAUIOBJECT pTheir) { return true; }
	// return false to cancel the whole change action
	virtual bool OnChangeLayoutBegin() { return true; }
	// for notify user the layout changing is over
	virtual void OnChangeLayoutEnd(bool bAllDone) { return; }

	// switch the resources on these two object
	bool SwitchResource(PAUIOBJECT lhs, PAUIOBJECT rhs);	
	
	//	�ؼ��ӳ����ã����������
	struct ControlEnableDelay
	{
		AUIObject *	m_pObj;
		DWORD		m_dwDelayTime;
		DWORD		m_dwLeftTime;
		ControlEnableDelay():m_pObj(NULL), m_dwDelayTime(0), m_dwLeftTime(0){}
		ControlEnableDelay(AUIObject *pObj, DWORD dwDelayTime)
			: m_pObj(pObj), m_dwDelayTime(dwDelayTime),	m_dwLeftTime(0){
		}
		void		Restart();
		void		Tick(DWORD dwTickTime);
		bool operator == (const AUIObject *pObj)const{
			return m_pObj == pObj;
		}
	};
	typedef abase::vector<ControlEnableDelay>	ControlEnableDelayArray;
	ControlEnableDelayArray	m_ctrlDelayArray;
	
	struct ButtonGfxBackup 
	{
		AString down_gfx;
		AString down_image;
	};
	typedef abase::hash_map<PAUIOBJECT, ButtonGfxBackup> ButtonBackupMap;
	typedef abase::hash_map<AString, PAUIOBJECT> CommandButtonMap;
	ButtonBackupMap m_BtnBackUp;
	CommandButtonMap m_CmdBtnMap;
};

//	�޸ĶԻ�����أ����ؼ��� A2DSprite ��Դ�����ڴ� Dialog �� A2DSprite �ѱ��ڴ��Ż�ʱ�޸� A2DSprite ��Դ��
//	AUIDialog::SetProperty��AUIObject::SetProperty �ȷ�����ǿ�м�����Դ
//	���� A2DSprite ��Դ���ü������Ż�ʱ����ȷ
//	��ˣ����Ȼָ�Ϊ���Ż�״̬�����޸���ɺ����Ż�
class ScopedDialogSpriteModify
{
	AUIDialog	*	m_p;
	bool			m_bResourceLoaded;
public:
	ScopedDialogSpriteModify(AUIDialog *p);
	virtual ~ScopedDialogSpriteModify();
};

//	�޸�ĳ�� AUI �ؼ��� Sprite ��Դ��ͬ ScopedDialogSpriteModify��
class ScopedAUIControlSpriteModify
{
	AUIObject	*	m_p;
	bool			m_bResourceLoaded;
public:
	ScopedAUIControlSpriteModify(AUIObject *p);
	virtual ~ScopedAUIControlSpriteModify();
};