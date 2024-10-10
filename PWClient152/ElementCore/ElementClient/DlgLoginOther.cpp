// File		: DlgLoginOther.cpp
// Creator	: Xu Wenbin
// Date		: 2012/5/14

#include "DlgLoginOther.h"
#include "EC_LoginUIMan.h"
#include "EC_LoginSwitch.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgLoginOther, CDlgExplorer)
AUI_END_COMMAND_MAP()

CDlgLoginOther::CDlgLoginOther()
{
	m_bLoginSwitch = true;
}

CDlgLoginOther::~CDlgLoginOther()
{
}

void CDlgLoginOther::OnShowDialog()
{
	m_bLoginSwitch = true;

	CDlgExplorer::OnShowDialog();
}

void CDlgLoginOther::OnHideDialog()
{
	CDlgExplorer::OnHideDialog();

	if (m_bLoginSwitch)
	{
		GetLoginUIMan()->GetDialog("Win_Login")->Show(true);
	}
}

void CDlgLoginOther::DocumentComplete(IDispatch* pDisp,VARIANT* URL)
{
	if (!IsShow()) return;

	//	�����¼�ɹ��������� m_bLoginSwitch = false
	while (true)
	{
		CExplorerMan *pExplorerMan = GetExplorerMan();
		if (!pExplorerMan)
			break;

		CPWBrowser *pBrowser = pExplorerMan->GetBrowser();
		if (!pBrowser)
			break;

		if (!pBrowser->IsDocReady())
			break;

		CComPtr<IHTMLDocument2> spDoc2 = pBrowser->GetDocument2();
		if (!spDoc2)
			break;

		//	���� spDoc2 ���Ի�ȡ��½��Ϣ
		HRESULT hr = E_FAIL;
		CComPtr<IHTMLElementCollection> spAll;
		hr = spDoc2->get_all(&spAll);
		if (!spAll)
			break;

		ACString strAgent, strUserId, strTicket;
		ACString strContent, strValue;
		
		BSTR pszOptText = NULL;
		BSTR pszOptValue = NULL;
		IDispatch *ppvdispOption = NULL;
		IHTMLInputElement *ppvOption = NULL;

		long lItems = 0;
		hr = spAll->get_length(&lItems);
		for (long i=0; i<lItems; i++)
		{
			VARIANT index;
			index.vt = VT_I4;
			index.intVal = i;
			hr = spAll->item(index, index, &ppvdispOption);
			if (!ppvdispOption) continue;
			
			hr = ppvdispOption->QueryInterface(IID_IHTMLInputElement, (void **) &ppvOption);
			ppvdispOption->Release();			
			if (!ppvOption) continue;
			
			hr = ppvOption->get_name(&(pszOptText));
			strContent = pszOptText;
			hr = ppvOption->get_value(&(pszOptValue));
			strValue = pszOptValue;			
			ppvOption->Release();
			
			if (strContent == _AL("userId"))
			{
				strUserId = strValue;
			}
			else if (strContent == _AL("ticket"))
			{
				strTicket = strValue;
			}
			else if (strContent == _AL("agent"))
			{
				strAgent = strValue;
			}
		}

		if (!strAgent.IsEmpty() && !strUserId.IsEmpty() && !strTicket.IsEmpty())
		{
			//	�ر�ʱ�����л���ԭ�е�¼����
			m_bLoginSwitch = false;

			//	�رյ�ǰ��ҳ��ʾ
			Show(false);

			//	���õ�¼����
			ACString strUser;
			strUser.Format(_AL("%s@%s@sso"), strUserId, strAgent);
			CECLoginSwitch::Instance().SSOLogin(strUser, strTicket, strAgent);

			//	���õ�¼����
			GetLoginUIMan()->ClickLoginButton();
		}

		break;
	}
}

CECLoginUIMan* CDlgLoginOther::GetLoginUIMan()
{
	return static_cast<CECLoginUIMan*>(m_pAUIManager);
}
