// Filename	: DlgBackHelp.cpp
// Creator	: Xu Wenbin
// Date		: 2009/08/27

#include "AFI.h"
#include "DlgBackHelp.h"
#include <AWIniFile.h>

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgBackHelp, CDlgBase)
AUI_END_COMMAND_MAP()

CDlgBackHelp::CDlgBackHelp()
{
	m_pLst_Member = NULL;
}

bool CDlgBackHelp::OnInitDialog()
{
	DDX_Control("List_Member", m_pLst_Member);
	return true;
}

void CDlgBackHelp::OnShowDialog()
{
	m_pLst_Member->ResetContent();

	AWIniFile helpFile;

	if (!helpFile.Open("configs\\backshop.ini"))
		return;

	const ACHAR *szSection = _AL("BackOthers");
	const ACHAR *szKey1Format = _AL("Level%d");
	const ACHAR *szKey2Format = _AL("Gold%d");

	int i(0);
	ACString key1, key2;
	ACString value1, value2;
	ACString strListItem;
	do {
		key1.Format(szKey1Format, i);
		key2.Format(szKey2Format, i);

		value1 = helpFile.GetValueAsString(szSection, key1);
		if (value1.IsEmpty())
			break;

		value2 = helpFile.GetValueAsString(szSection, key2);
		if (value2.IsEmpty())
			break;

		strListItem = value1 + _AL("\t") + value2;
		m_pLst_Member->AddString(strListItem);

		++ i;
	} while(true);

	helpFile.Close();

	CDlgTextHelp::OnShowDialog();
}