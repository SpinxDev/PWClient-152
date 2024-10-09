#include <A3DDevice.h>
#include <A3DEngine.h>
#include <A3DViewport.h>
#include "EC_Global.h"
#include "EC_Game.h"
#include "DlgSettingScriptHelp.h"
#include "AUICheckBox.h"
#include "ECScriptOption.h"
#include "ECScriptContext.h"

AUI_BEGIN_COMMAND_MAP(CDlgSettingScriptHelp, CDlgBase)

AUI_ON_COMMAND("IDCANCEL", OnCommandIDCANCEL)
AUI_ON_COMMAND("confirm", OnCommandConfirm)

AUI_END_COMMAND_MAP()

CDlgSettingScriptHelp::CDlgSettingScriptHelp()
{
	m_pContext = NULL;
}

CDlgSettingScriptHelp::~CDlgSettingScriptHelp()
{
}

void CDlgSettingScriptHelp::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	for(int i = 0; i < CDLGSETTINGSCRIPTHELP_TIPTYPES_PP; i++ )
	{
		char szTemp[20];
		sprintf(szTemp,"Chk_TipType%d",i+1);
		DDX_CheckBox(bSave, szTemp, m_bTipType[i]);
	}
}

void CDlgSettingScriptHelp::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	for(int i = 0; i < CDLGSETTINGSCRIPTHELP_TIPTYPES_PP; i++ )
	{
		m_bTipType[i] = m_pContext->GetOption()->IsTypeActive(i);
	}	

	UpdateData(false);

	SIZE sizeWnd = this->GetSize();
	A3DVIEWPORTPARAM* pParam = g_pGame->GetA3DDevice()->GetA3DEngine()->GetActiveViewport()->GetParam();
	int x = (pParam->Width - sizeWnd.cx) / 2;
	int y = (pParam->Height - sizeWnd.cy) / 2;

	// old : SetPos(x, y);
	SetPosEx(x, y);
}

void CDlgSettingScriptHelp::SetScriptContext(CECScriptContext * pContext)
{
	m_pContext = pContext;
}

void CDlgSettingScriptHelp::OnCommandIDCANCEL(const char * szCommand)
{
	Show(false);
}

void CDlgSettingScriptHelp::OnCommandConfirm(const char * szCommand)
{
	UpdateData(true);

	for(int i = 0; i < CDLGSETTINGSCRIPTHELP_TIPTYPES_PP; i++ )
	{
		m_pContext->GetOption()->SetTypeActive(i, m_bTipType[i]);
	}
	m_pContext->GetOption()->SaveToServer();
	
	Show(false);
}
