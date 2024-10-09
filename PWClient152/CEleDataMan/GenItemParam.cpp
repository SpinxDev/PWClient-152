// GenItemParam.cpp : implementation file
//

#include "stdafx.h"
#include "eledataman.h"
#include "GenItemParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGenItemParam dialog


CGenItemParam::CGenItemParam(CWnd* pParent /*=NULL*/)
	: CDialog(CGenItemParam::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGenItemParam)
	memset(m_indexList,0,sizeof(m_indexList));
	memset(m_addonList,0,sizeof(m_addonList));
	memset(m_randList,0,sizeof(m_randList));	
	//}}AFX_DATA_INIT
}


void CGenItemParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenItemParam)
	DDX_Text(pDX, IDC_INDEX_1, m_indexList[0]);
	DDV_MinMaxInt(pDX, m_indexList[0], 0, 1000);
	DDX_Text(pDX, IDC_INDEX_2, m_indexList[1]);
	DDV_MinMaxInt(pDX, m_indexList[1], 0, 1000);
	DDX_Text(pDX, IDC_INDEX_3, m_indexList[2]);
	DDV_MinMaxInt(pDX, m_indexList[2], 0, 1000);
	DDX_Text(pDX, IDC_INDEX_4, m_indexList[3]);
	DDV_MinMaxInt(pDX, m_indexList[3], 0, 1000);
	DDX_Text(pDX, IDC_INDEX_5, m_indexList[4]);
	DDV_MinMaxInt(pDX, m_indexList[4], 0, 1000);
	DDX_Text(pDX, IDC_INDEX_6, m_indexList[5]);
	DDV_MinMaxInt(pDX, m_indexList[5], 0, 1000);
	DDX_Text(pDX, IDC_INDEX_7, m_indexList[6]);
	DDV_MinMaxInt(pDX, m_indexList[6], 0, 1000);
	DDX_Text(pDX, IDC_INDEX_8, m_indexList[7]);
	DDV_MinMaxInt(pDX, m_indexList[7], 0, 1000);
	DDX_Text(pDX, IDC_INDEX_9, m_indexList[8]);
	DDV_MinMaxInt(pDX, m_indexList[8], 0, 1000);
	DDX_Text(pDX, IDC_INDEX_10, m_indexList[9]);
	DDV_MinMaxInt(pDX, m_indexList[9], 0, 1000);
	DDX_Text(pDX, IDC_INDEX_11, m_indexList[10]);
	DDV_MinMaxInt(pDX, m_indexList[10], 0, 1000);
	DDX_Text(pDX, IDC_INDEX_12, m_indexList[11]);
	DDV_MinMaxInt(pDX, m_indexList[11], 0, 1000);
	DDX_Text(pDX, IDC_INDEX_13, m_indexList[12]);
	DDV_MinMaxInt(pDX, m_indexList[12], 0, 1000);
	DDX_Text(pDX, IDC_INDEX_14, m_indexList[13]);
	DDV_MinMaxInt(pDX, m_indexList[13], 0, 1000);
	DDX_Text(pDX, IDC_INDEX_15, m_indexList[14]);
	DDV_MinMaxInt(pDX, m_indexList[14], 0, 1000);
	DDX_Text(pDX, IDC_INDEX_16, m_indexList[15]);
	DDV_MinMaxInt(pDX, m_indexList[15], 0, 1000);

	DDX_Text(pDX, IDC_ADDON_1, m_addonList[0]);
	DDV_MinMaxInt(pDX, m_addonList[0], 0, 1000);
	DDX_Text(pDX, IDC_ADDON_2, m_addonList[1]);
	DDV_MinMaxInt(pDX, m_addonList[1], 0, 1000);
	DDX_Text(pDX, IDC_ADDON_3, m_addonList[2]);
	DDV_MinMaxInt(pDX, m_addonList[2], 0, 1000);
	DDX_Text(pDX, IDC_ADDON_4, m_addonList[3]);
	DDV_MinMaxInt(pDX, m_addonList[3], 0, 1000);
	DDX_Text(pDX, IDC_ADDON_5, m_addonList[4]);
	DDV_MinMaxInt(pDX, m_addonList[4], 0, 1000);
	DDX_Text(pDX, IDC_ADDON_6, m_addonList[5]);
	DDV_MinMaxInt(pDX, m_addonList[5], 0, 1000);
	DDX_Text(pDX, IDC_ADDON_7, m_addonList[6]);
	DDV_MinMaxInt(pDX, m_addonList[6], 0, 1000);
	DDX_Text(pDX, IDC_ADDON_8, m_addonList[7]);
	DDV_MinMaxInt(pDX, m_addonList[7], 0, 1000);
	DDX_Text(pDX, IDC_ADDON_9, m_addonList[8]);
	DDV_MinMaxInt(pDX, m_addonList[8], 0, 1000);
	DDX_Text(pDX, IDC_ADDON_10, m_addonList[9]);
	DDV_MinMaxInt(pDX, m_addonList[9], 0, 1000);
	DDX_Text(pDX, IDC_ADDON_11, m_addonList[10]);
	DDV_MinMaxInt(pDX, m_addonList[10], 0, 1000);
	DDX_Text(pDX, IDC_ADDON_12, m_addonList[11]);
	DDV_MinMaxInt(pDX, m_addonList[11], 0, 1000);
	DDX_Text(pDX, IDC_ADDON_13, m_addonList[12]);
	DDV_MinMaxInt(pDX, m_addonList[12], 0, 1000);
	DDX_Text(pDX, IDC_ADDON_14, m_addonList[13]);
	DDV_MinMaxInt(pDX, m_addonList[13], 0, 1000);
	DDX_Text(pDX, IDC_ADDON_15, m_addonList[14]);
	DDV_MinMaxInt(pDX, m_addonList[14], 0, 1000);
	DDX_Text(pDX, IDC_ADDON_16, m_addonList[15]);
	DDV_MinMaxInt(pDX, m_addonList[15], 0, 1000);

	DDX_Text(pDX, IDC_RANGE_1, m_randList[0]);
	DDV_MinMaxInt(pDX, m_randList[0], 0.f, 1.0f);
	DDX_Text(pDX, IDC_RANGE_2, m_randList[1]);
	DDV_MinMaxInt(pDX, m_randList[1], 0.f, 1.0f);
	DDX_Text(pDX, IDC_RANGE_3, m_randList[2]);
	DDV_MinMaxInt(pDX, m_randList[2], 0.f, 1.0f);
	DDX_Text(pDX, IDC_RANGE_4, m_randList[3]);
	DDV_MinMaxInt(pDX, m_randList[3], 0.f, 1.0f);
	DDX_Text(pDX, IDC_RANGE_5, m_randList[4]);
	DDV_MinMaxInt(pDX, m_randList[4], 0.f, 1.0f);
	DDX_Text(pDX, IDC_RANGE_6, m_randList[5]);
	DDV_MinMaxInt(pDX, m_randList[5], 0.f, 1.0f);
	DDX_Text(pDX, IDC_RANGE_7, m_randList[6]);
	DDV_MinMaxInt(pDX, m_randList[6], 0.f, 1.0f);
	DDX_Text(pDX, IDC_RANGE_8, m_randList[7]);
	DDV_MinMaxInt(pDX, m_randList[7], 0.f, 1.0f);
	DDX_Text(pDX, IDC_RANGE_9, m_randList[8]);
	DDV_MinMaxInt(pDX, m_randList[8], 0.f, 1.0f);
	DDX_Text(pDX, IDC_RANGE_10, m_randList[9]);
	DDV_MinMaxInt(pDX, m_randList[9], 0.f, 1.0f);
	DDX_Text(pDX, IDC_RANGE_11, m_randList[10]);
	DDV_MinMaxInt(pDX, m_randList[10], 0.f, 1.0f);
	DDX_Text(pDX, IDC_RANGE_12, m_randList[11]);
	DDV_MinMaxInt(pDX, m_randList[11], 0.f, 1.0f);
	DDX_Text(pDX, IDC_RANGE_13, m_randList[12]);
	DDV_MinMaxInt(pDX, m_randList[12], 0.f, 1.0f);
	DDX_Text(pDX, IDC_RANGE_14, m_randList[13]);
	DDV_MinMaxInt(pDX, m_randList[13], 0.f, 1.0f);
	DDX_Text(pDX, IDC_RANGE_15, m_randList[14]);
	DDV_MinMaxInt(pDX, m_randList[14], 0.f, 1.0f);
	DDX_Text(pDX, IDC_RANGE_16, m_randList[15]);
	DDV_MinMaxInt(pDX, m_randList[15], 0.f, 1.0f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGenItemParam, CDialog)
	//{{AFX_MSG_MAP(CGenItemParam)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenItemParam message handlers
