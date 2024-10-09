// Filename	: DlgWikiTaskDetail.cpp
// Creator	: Feng Ning
// Date		: 2010/05/11

#include "DlgWikiTaskDetail.h"
#include "TaskTempl.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiTaskDetail, CDlgTask)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiTaskDetail, CDlgTask)
AUI_END_EVENT_MAP()

CDlgWikiTaskDetail::CDlgWikiTaskDetail()
{
}

bool CDlgWikiTaskDetail::OnInitDialog()
{
	if (!CDlgTask::OnInitDialog())
		return false;
	
	return true;
}

void CDlgWikiTaskDetail::OnShowDialog()
{
	ATaskTempl* pTempl = (ATaskTempl*)GetDataPtr();
	if(pTempl)
	{
		m_iType = 1;
		for( size_t i = 0; i < m_ImgCount; i++ )
		{
			m_pImg_Item[i]->Show(false);
		}
		m_pTxt_BaseAward->Show(false);

		CDlgTask::SearchForTask(pTempl->GetID());
	}

	CDlgTask::OnShowDialog();
}

bool CDlgWikiTaskDetail::Release(void)
{
	//
	return CDlgTask::Release();
}