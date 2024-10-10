// DlgSettings.cpp : implementation file
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgSettings.h"

#include "PropListCtrl/BCGPPropertyListWrapper.h"
#include "PropListCtrl/BCGPPropertyObject.h"
#include "Global.h"


// CDlgSettings dialog

IMPLEMENT_DYNAMIC(CDlgSettings, CBCGPDialog)

CDlgSettings::CDlgSettings(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgSettings::IDD, pParent)
{
	m_pBCGPPropListWrapper = new CBCGPPropertyListWrapper;
	m_pPropertyListener = new CPropertyUpdateListener(this);
	m_pProperties = new CPropertyArray;
	m_pProperties->AddListener(m_pPropertyListener);
}

CDlgSettings::~CDlgSettings()
{
	delete m_pPropertyListener;
	m_pPropertyListener = 0;
	if (m_pProperties)
		m_pProperties->Clear();
	delete m_pProperties;
	m_pProperties = 0;
	delete m_pBCGPPropListWrapper;
	m_pBCGPPropListWrapper = 0;
}

void CDlgSettings::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSettings, CBCGPDialog)
END_MESSAGE_MAP()


// CDlgSettings message handlers

BOOL CDlgSettings::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle();
	// TODO:  Add extra initialization here

	CRect rc;
	GetDlgItem(IDC_FRM_SETTING_PROP)->GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.DeflateRect(1,1,1,1);

	if (!m_pBCGPPropListWrapper->OnCreate(rc, this))
		return FALSE;
	if(!buildProperty())
		return FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool CDlgSettings::buildProperty()
{
	m_pProperties->Clear();

	CPropertyArray* pClassArray = m_pProperties->CreatePropertyArray();
	for (int i=0; i<MAX_GROUP_CLASS_NUM; ++i)
	{
		char szName[128] = {0};
		sprintf(szName, "%d", i);
		pClassArray->AddProperty(std::string(g_Configs.szClassNames[i]), szName);
	}
	m_pProperties->AddProperty(pClassArray, "事件组类属");

	CPropertyArray* p3DEventPropArray = m_pProperties->CreatePropertyArray();	
	p3DEventPropArray->AddProperty(g_Configs.v3DEventPos, "3D事件位置");
	p3DEventPropArray->AddProperty(g_Configs.v3DEventVel, "3D事件速度");
	p3DEventPropArray->AddProperty(g_Configs.v3DEventOrientation, "3D事件朝向");
	m_pProperties->AddProperty(p3DEventPropArray, "3D事件空间属性");

	CPropertyArray* pListenerPropArray = m_pProperties->CreatePropertyArray();	
	pListenerPropArray->AddProperty(g_Configs.vListenerPos, "听众位置");
	pListenerPropArray->AddProperty(g_Configs.vListenerVel, "听众速度");
	pListenerPropArray->AddProperty(g_Configs.vListenerForward, "听众前方向");
	pListenerPropArray->AddProperty(g_Configs.vListenerUp, "听众上方向");
	m_pProperties->AddProperty(pListenerPropArray, "听众空间属性");

	m_pBCGPPropListWrapper->AttachTo(m_pProperties);
	return true;
}

void CDlgSettings::UpdateProperty(bool bSave, bool bIsCommit /*= false*/)
{
	CPropertyArray* pClassArray = any_cast<CPropertyArray>(&m_pProperties->GetPropVal(0));
	CPropertyArray* p3DEventPropArray = any_cast<CPropertyArray>(&m_pProperties->GetPropVal(1));
	CPropertyArray* pListenerPropArray = any_cast<CPropertyArray>(&m_pProperties->GetPropVal(2));
	if(bSave)
	{
		for (int i=0; i<MAX_GROUP_CLASS_NUM; ++i)
		{
			strcpy(g_Configs.szClassNames[i], any_cast<std::string>(pClassArray->GetPropVal(i)).c_str());
		}
		g_Configs.v3DEventPos = any_cast<A3DVECTOR3>(p3DEventPropArray->GetPropVal(0));
		g_Configs.v3DEventVel = any_cast<A3DVECTOR3>(p3DEventPropArray->GetPropVal(1));
		g_Configs.v3DEventOrientation = any_cast<A3DVECTOR3>(p3DEventPropArray->GetPropVal(2));

		g_Configs.vListenerPos = any_cast<A3DVECTOR3>(pListenerPropArray->GetPropVal(0));
		g_Configs.vListenerVel = any_cast<A3DVECTOR3>(pListenerPropArray->GetPropVal(1));
		g_Configs.vListenerForward = any_cast<A3DVECTOR3>(pListenerPropArray->GetPropVal(2));
		g_Configs.vListenerUp = any_cast<A3DVECTOR3>(pListenerPropArray->GetPropVal(3));
	}
	else
	{
		for (int i=0; i<MAX_GROUP_CLASS_NUM; ++i)
		{
			pClassArray->SetPropVal(i, std::string(g_Configs.szClassNames[i]));
		}
		p3DEventPropArray->SetPropVal(0, g_Configs.v3DEventPos);
		p3DEventPropArray->SetPropVal(1, g_Configs.v3DEventVel);
		p3DEventPropArray->SetPropVal(2, g_Configs.v3DEventOrientation);

		pListenerPropArray->SetPropVal(0, g_Configs.vListenerPos);
		pListenerPropArray->SetPropVal(1, g_Configs.vListenerVel);
		pListenerPropArray->SetPropVal(2, g_Configs.vListenerForward);
		pListenerPropArray->SetPropVal(3, g_Configs.vListenerUp);
	}
}