// FWDialogEdit.cpp: implementation of the FWDialogEdit class.
//
//////////////////////////////////////////////////////////////////////

#include "FWDialogEdit.h"
#include "FWCommandDelete.h"
#include "FWDoc.h"
#include "FWCommandList.h"
#include "AUIStillImageButton.h"
#include "FWView.h"
#include "FWStateSelect.h"
#include "FWDefinedMsg.h"
#include "FWAUIManager.h"
#include "FWStateMove.h"
#include "AUIListBox.h"
#include "FWGlyph.h"
#include "FWAlgorithm.h"
#include "FWCommandMove.h"
#include "FWGlyphList.h"
#include "FWCommandCreate.h"
#include "AUIProgress.h"
#include "AUILabel.h"
#include "FWSharedFile.h"

using namespace FWAlgorithm;

#define new A_DEBUG_NEW


FW_IMPLEMENT_DYNCREATE(FWDialogEdit, FWDialogBase)

BEGIN_EVENT_MAP_FW(FWDialogEdit, FWDialogBase)

ON_EVENT_FW("BTN_DELETE", WM_LCLICK, FWDialogEdit::OnClickBtnDelete)
ON_EVENT_FW("BTN_UNDO", WM_LCLICK, FWDialogEdit::OnClickBtnUndo)
ON_EVENT_FW("BTN_REDO", WM_LCLICK, FWDialogEdit::OnClickBtnRedo)
ON_EVENT_FW("BTN_SELECT_ALL", WM_LCLICK, FWDialogEdit::OnClickBtnSelectAll)
ON_EVENT_FW("LIST_GLYPH", WM_LBUTTONDOWN, FWDialogEdit::OnLButtonDownListGlyph)
ON_EVENT_FW("BTN_PROP", WM_LCLICK, FWDialogEdit::OnClickBtnProp)
ON_EVENT_FW("LIST_GLYPH", WM_LBUTTONDBLCLK, FWDialogEdit::OnLButtonDblClickListGlyph)
ON_EVENT_FW("BTN_CENTER", WM_LCLICK, FWDialogEdit::OnClickBtnCenter)
ON_EVENT_FW("BTN_COPY", WM_LCLICK, FWDialogEdit::OnClickBtnCopy)
ON_EVENT_FW("BTN_CUT", WM_LCLICK, FWDialogEdit::OnClickBtnCut)
ON_EVENT_FW("BTN_PASTE", WM_LCLICK, FWDialogEdit::OnClickBtnPaste)
ON_EVENT_FW(NULL, WM_UPDATE_TOOLBAR, FWDialogEdit::OnUpdateToolbar)
ON_EVENT_FW("BTN_INVERT_SELECT", WM_LCLICK, FWDialogEdit::OnClickBtnInvertSelect)


END_EVENT_MAP_FW()



FWDialogEdit::FWDialogEdit()
{

}

FWDialogEdit::~FWDialogEdit()
{

}

bool FWDialogEdit::Init(AUIDialog * pDlg)
{
	FWDialogBase::Init(pDlg);

	m_pPrgParticalQuota = (AUIProgress *) GetDlgItem("PRG_PARTICAL_QUOTA");
	ASSERT(m_pPrgParticalQuota);

	m_pPrgSoundQuota = (AUIProgress *) GetDlgItem("PRG_SOUND_QUOTA");
	ASSERT(m_pPrgSoundQuota);

	return true;
}


void FWDialogEdit::OnClickBtnDelete(WPARAM wParam, LPARAM lParam)
{
	GetView()->ChangeState(new FWStateSelect(GetView()));
	GetDocument()->GetCommandList()->ExecuteCommand(
		new FWCommandDelete(GetDocument()));
}

void FWDialogEdit::OnClickBtnUndo(WPARAM wParam, LPARAM lParam)
{
	GetView()->ChangeState(new FWStateSelect(GetView()));
	GetDocument()->GetCommandList()->UnExecuteCommand();
}

void FWDialogEdit::OnClickBtnRedo(WPARAM wParam, LPARAM lParam)
{
	GetView()->ChangeState(new FWStateSelect(GetView()));
	GetDocument()->GetCommandList()->ReExecuteCommand();
}

void FWDialogEdit::OnClickBtnSelectAll(WPARAM wParam, LPARAM lParam)
{
	GetView()->ChangeState(new FWStateSelect(GetView()));
	GetDocument()->GetGlyphList()->SelAll();
}


void FWDialogEdit::OnLButtonDownListGlyph(WPARAM wParam, LPARAM lParam)
{
	GetView()->ChangeState(new FWStateSelect(GetView()));

	AUIListBox *pListbox = (AUIListBox *) GetDlgItem("LIST_GLYPH");
	ASSERT(pListbox);

	// convert to listbox coordinate
	APointI pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	A3DRECT rect = pListbox->GetRect();
	pt = GetRelativePointToRect(&rect, &pt);

	// update sel in board
	if (pListbox->GetHitArea(pt.x, pt.y) == AUILISTBOX_RECT_TEXT)
	{
		FWGlyphList *pGlyphList = GetDocument()->GetGlyphList();
		pGlyphList->EmptySel();

		for (int i = 0; i < pListbox->GetCount(); i++)
		{
			if (!pListbox->GetSel(i)) continue; 

			FWGlyph *pGlyph = (FWGlyph *)pListbox->GetItemDataPtr(i);
			ALISTPOSITION pos = pGlyphList->GetHeadPosition();
			while (pos)
			{
				if (pGlyphList->GetAt(pos) == pGlyph)
				{
					pGlyphList->AddSel(pos);
					break;
				}
				pGlyphList->GetNext(pos);
			}
		}
	}
		
}


void FWDialogEdit::OnClickBtnProp(WPARAM wParam, LPARAM lParam)
{
	AUIListBox *pListbox = (AUIListBox *) GetDlgItem("LIST_GLYPH");
	int nSelCount = pListbox->GetSelCount();

	FWGlyph *pGlyph = NULL;
	if (nSelCount == 1) 
	{
		FWGlyphList *pList = GetDocument()->GetGlyphList();
		pGlyph = pList->GetSelHead();
	}
	else if (nSelCount > 1)
	{
		for (int i = 0; i < pListbox->GetCount(); i++)
		{
			if (pListbox->GetSel(i))
			{
				pGlyph = (FWGlyph *) pListbox->GetItemDataPtr(i);
				break;
			}
		} 
	}

	if (pGlyph)
	{
		GetView()->ShowPropDlg(pGlyph);
	}

}

void FWDialogEdit::OnLButtonDblClickListGlyph(WPARAM wParam, LPARAM lParam)
{
	OnClickBtnProp(NULL, NULL);
}

void FWDialogEdit::OnClickBtnCenter(WPARAM wParam, LPARAM lParam)
{
	FWGlyphList *pList = GetDocument()->GetGlyphList();
	ALISTPOSITION pos = pList->GetSelHeadPosition();
	if (!pos) return;

	APointI centerSel = pList->GetSelTrueRect().CenterPoint();
	APointI offset(-centerSel.x, -centerSel.y);

	
	FWCommandMove::AGlyphArray aryGlyph;
	FWCommandMove::APointArray aryOldCenter, aryNewCenter;
	while (pos)
	{
		FWGlyph *pGlyph = pList->GetSelNext(pos);
		aryGlyph.Add(pGlyph);

		APointI centerGlyph = pGlyph->GetCentralPoint();
		aryOldCenter.Add(centerGlyph);

		centerGlyph += offset;
		aryNewCenter.Add(centerGlyph);
	}
	GetDocument()->GetCommandList()->ExecuteCommand(
		new FWCommandMove(aryGlyph, aryOldCenter, aryNewCenter));

}

#define GLOBAL_MEM_GROW_SIZE 1024

void FWDialogEdit::OnClickBtnCopy(WPARAM wParam, LPARAM lParam)
{
	if (!GetView()->GetClipboardFormat()) return;

	FWGlyphList *pList = GetDocument()->GetGlyphList();
	ALISTPOSITION pos = pList->GetSelHeadPosition();
	if (!pos) return;

	FWSharedFile file(GLOBAL_MEM_GROW_SIZE);
	FWArchive ar(&file, FWArchive::MODE_STORE);

	ar << pList->GetSelCount();
	while (pos)
	{
		FWGlyph *pGlyph = pList->GetSelNext(pos);
		ar << pGlyph;
	}
	ar.Flush();

	HGLOBAL hData = file.Detach();
	if (::OpenClipboard(GetView()->GetHwnd()))
	{
		::EmptyClipboard();
		::SetClipboardData(GetView()->GetClipboardFormat(), hData);
		::CloseClipboard();
	}
	else 
	{
		// should I delete hData ?
		::CloseHandle(hData);
	}


}

void FWDialogEdit::OnClickBtnCut(WPARAM wParam, LPARAM lParam)
{
	OnClickBtnCopy(wParam, lParam);

	GetDocument()->GetCommandList()->ExecuteCommand(
		new FWCommandDelete(GetDocument()));
}

void FWDialogEdit::OnClickBtnPaste(WPARAM wParam, LPARAM lParam)
{
	HANDLE hData = NULL;

	BEGIN_FAKE_WHILE;

	CHECK_BREAK(GetView()->GetClipboardFormat());

	CHECK_BREAK(::OpenClipboard(GetView()->GetHwnd()));

	hData = ::GetClipboardData(GetView()->GetClipboardFormat());
	CHECK_BREAK(hData);
 
	FWSharedFile file(GLOBAL_MEM_GROW_SIZE);
	file.SetHandle(hData, false);
	FWArchive ar(&file, FWArchive::MODE_LOAD);

	FWCommandCreate::FWGlyphList glyphList;
	int nCount = 0;
	ar >> nCount;
	while (nCount--)
	{
		FWGlyph *pGlyph = NULL;
		ar >> (const FWObject *&)pGlyph;

		pGlyph->GenerateMeshParam();
		
		glyphList.AddTail(pGlyph);
	}
	GetDocument()->GetCommandList()->ExecuteCommand(
		new FWCommandCreate(GetDocument(), glyphList));
	
	file.Detach();

	END_FAKE_WHILE;
}

void FWDialogEdit::OnUpdateToolbar(WPARAM wParam, LPARAM lParam)
{
	UpdateGlyphListbox();

	UpdateParticalQuotaProgress();

	UpdateSoundQuotaProgress();
}

void FWDialogEdit::OnClickBtnInvertSelect(WPARAM wParam, LPARAM lParam)
{
	FWGlyphList *pList = GetDocument()->GetGlyphList();

	ALISTPOSITION pos = pList->GetHeadPosition();
	while (pos)
	{
		pList->InvertSel(pos);
		pList->GetNext(pos);
	}
}

void FWDialogEdit::UpdateGlyphListbox()
{
	// update listbox 
	
	AUIListBox *pListbox = (AUIListBox *)GetDlgItem("LIST_GLYPH");
	ASSERT(pListbox);
	
	FWGlyphList *pGlyphList = GetDocument()->GetGlyphList();
	AIntList * pParticalQuotaList = GetDocument()->GetParticalQuotaList();
	AIntList * pSoundQuotaList = GetDocument()->GetSoundQuotaList();
	
	// rebuild list if glyph number changed
	if (pGlyphList->GetCount() != pListbox->GetCount())
	{
		pListbox->ResetContent();
		ALISTPOSITION pos = pGlyphList->GetHeadPosition();
		while (pos)
		{
			FWGlyph *pGlyph = pGlyphList->GetNext(pos);
			int nIndex = pListbox->AddString(pGlyph->GetName()) - 1;
			pListbox->SetItemDataPtr(nIndex, pGlyph);
		}
	}
	
	// update name if needed
	int nIndex = 0;
	ALISTPOSITION posGlyph = pGlyphList->GetHeadPosition();
	ALISTPOSITION posParticalQuota = pParticalQuotaList->GetHeadPosition();
	ALISTPOSITION posSoundQuota = pSoundQuotaList->GetHeadPosition();
	while (posGlyph && posParticalQuota && posSoundQuota &&
		nIndex < pListbox->GetCount())
	{
		FWGlyph *pGlyph = pGlyphList->GetNext(posGlyph);
		int nParticalQuota = pParticalQuotaList->GetNext(posParticalQuota);
		int nSoundQuota = pSoundQuotaList->GetNext(posSoundQuota);
		ACString strName;
		strName.Format(_AL("%s(%d/%d)"), pGlyph->GetName(), nParticalQuota, nSoundQuota);
		if (strName != pListbox->GetText(nIndex))
			pListbox->SetText(nIndex, strName);
		nIndex++;
	}
	
	// set selelecd flag in listbox
	for (int i = 0; i < pListbox->GetCount(); i++)
	{
		FWGlyph *pGlyph = (FWGlyph *)pListbox->GetItemDataPtr(i);
		
		bool bFound = false;
		ALISTPOSITION pos = pGlyphList->GetSelHeadPosition();
		while (pos)
		{
			if (pGlyphList->GetSelNext(pos) == pGlyph)
			{
				bFound = true;
				break;
			}
		}
		pListbox->SetSel(i, bFound);
	}
}

void FWDialogEdit::UpdateParticalQuotaProgress()
{
	int nCurrent = GetDocument()->GetCurrentParticalQuota();
	int nMax = GetDocument()->GetMaxParticalQuota();
	
	AUILabel *pLabel = (AUILabel *) GetDlgItem("LBL_PARTICAL_QUOTA");
	ASSERT(pLabel);
	ACString tmp;
	tmp.Format(_AL("粒子总数：%d[%d]"), nCurrent, nMax);
	pLabel->SetText(tmp);

	if (nMax != 0)
	{
		m_pPrgParticalQuota->SetProgress(float(nCurrent) / nMax * 100.f);

		AUIOBJECT_SETPROPERTY propOld, propNew;
		if (m_pPrgParticalQuota->GetProperty("Fill Image File", &propOld))
		{
			if (nCurrent > nMax)
			{
				strcpy(propNew.fn, "Progress\\进度条_红.bmp");
			}
			else
			{
				strcpy(propNew.fn, "Progress\\进度条_兰.bmp");
			}
			if (strcmp(propOld.fn, propNew.fn) != 0)
				m_pPrgParticalQuota->SetProperty("Fill Image File", &propNew);
		}

	}


}


void FWDialogEdit::UpdateSoundQuotaProgress()
{
	int nCurrent = GetDocument()->GetCurrentSoundQuota();
	int nMax = GetDocument()->GetMaxSoundQuota();
	
	AUILabel *pLabel = (AUILabel *) GetDlgItem("LBL_SOUND_QUOTA");
	ASSERT(pLabel);
	ACString tmp;
	tmp.Format(_AL("音效总数：%d[%d]"), nCurrent, nMax);
	pLabel->SetText(tmp);
	
	if (nMax != 0)
	{
		m_pPrgSoundQuota->SetProgress(float(nCurrent) / nMax * 100.f);
		
		AUIOBJECT_SETPROPERTY propOld, propNew;
		if (m_pPrgSoundQuota->GetProperty("Fill Image File", &propOld))
		{
			if (nCurrent > nMax)
			{
				strcpy(propNew.fn, "Progress\\进度条_红.bmp");
			}
			else
			{
				strcpy(propNew.fn, "Progress\\进度条_兰.bmp");
			}
			if (strcmp(propOld.fn, propNew.fn) != 0)
				m_pPrgSoundQuota->SetProperty("Fill Image File", &propNew);
		}
		
	}
	
	
}
