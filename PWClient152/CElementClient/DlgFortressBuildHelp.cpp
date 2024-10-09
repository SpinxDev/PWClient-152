// File		: DlgFortressBuildHelp.cpp
// Creator	: Feng Ning
// Date		: 2010/11/24

#include "DlgFortressBuildHelp.h"
#include "AUIStillImageButton.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFortressBuildHelp, CDlgTextHelp)
AUI_ON_COMMAND("BtnTab_*", OnCommand_Switch)
AUI_END_COMMAND_MAP()

CDlgFortressBuildHelp::CDlgFortressBuildHelp()
{
}

CDlgFortressBuildHelp::~CDlgFortressBuildHelp()
{
}

bool CDlgFortressBuildHelp::OnInitDialog()
{
	if(!CDlgTextHelp::OnInitDialog())
	{
		return false;
	}

	ShowTab(1);

	//
	return true;
}

void CDlgFortressBuildHelp::OnShowDialog()
{
	//
	CDlgTextHelp::OnShowDialog();
}

bool CDlgFortressBuildHelp::Release()
{
	//
	return CDlgTextHelp::Release();
}

void CDlgFortressBuildHelp::OnCommand_Switch(const char* szCommand)
{
	size_t prefix = strlen("BtnTab_");
	if(strlen(szCommand) > prefix)
	{
		int index = atoi(szCommand + prefix);
		ShowTab(index);
	}
}

void CDlgFortressBuildHelp::ShowTab(int index)
{
	PAUIOBJECTLISTELEMENT pElement = GetFirstControl();
	while (pElement)
	{
		PAUIOBJECT pObj = pElement->pThis;
		if (strstr(pObj->GetName(), "BtnTab_"))
		{
			((PAUISTILLIMAGEBUTTON)pObj)->SetPushed(false);
		}
		else if (strstr(pObj->GetName(), "ImgTab_"))
		{
			pObj->Show(false);
		}
		pElement = GetNextControl(pElement);
	}

	AString strName;
	PAUISTILLIMAGEBUTTON pBtn = dynamic_cast<PAUISTILLIMAGEBUTTON>
		(GetDlgItem(strName.Format("BtnTab_%02d", index)));
	if(pBtn) pBtn->SetPushed(true);

	PAUIOBJECT pImg = GetDlgItem(strName.Format("ImgTab_%02d", index));
	if(pImg) pImg->Show(true);
}