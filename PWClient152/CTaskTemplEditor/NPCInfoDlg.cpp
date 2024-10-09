// NPCInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "NPCInfoDlg.h"
#include "SetNPCInfoDlg.h"
#include "VSSOperation.h"
#include "shlwapi.h"
#include "AMemory.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNPCInfoDlg dialog

#define TASK_NPC_INFO_VERSION	2
#define TASK_NPC_INFO_LINE_LEN	1024

const char* _npc_info_path	= "BaseData\\TaskNPCInfo.dat";

static const char* _npc_info_ver	= "Version = %d";
static const char* _npc_info_count	= "Count = %d";
static const char* _npc_info		= "id = %d, x = %d, y = %d, z = %d";
static const char* _npc_pos			= "x=%d, y=%d, z=%d";

extern CString GetBaseDataNameByID(unsigned long ulID);

CNPCInfoDlg::CNPCInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNPCInfoDlg::IDD, pParent), m_bReadOnly(true)
{
	//{{AFX_DATA_INIT(CNPCInfoDlg)
	m_nUnMatch = 0;
	m_strNPCToFind = _T("");
	//}}AFX_DATA_INIT
}

CNPCInfoDlg::~CNPCInfoDlg()
{
	for (size_t i = 0; i < npc_arr.size(); i++)
		delete npc_arr[i];
}

void CNPCInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNPCInfoDlg)
	DDX_Control(pDX, IDC_NPC, m_NPCList);
	DDX_Text(pDX, IDC_UNMATCH, m_nUnMatch);
	DDX_Text(pDX, IDC_NPC_TO_FIND, m_strNPCToFind);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNPCInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CNPCInfoDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_NPC, OnDblclkNpc)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_FIND_NPC, OnFindNpc)
	ON_BN_CLICKED(ID_CHECK_IN, OnCheckIn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNPCInfoDlg message handlers

static bool _compare(NPC_INFO_EX* arg1, NPC_INFO_EX* arg2)
{
   return _stricmp(arg1->name, arg2->name) < 0;
}

BOOL CNPCInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_NPCList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_NPCList.InsertColumn(0, _T("NPC"), LVCFMT_LEFT, 150);
	m_NPCList.InsertColumn(1, _T("位置"), LVCFMT_LEFT, 150);

	LoadAndUpdateNPCInfo(true);
	std::sort(npc_arr.begin(), npc_arr.end(), _compare);

	for (size_t i = 0; i < npc_arr.size(); i++)
	{
		NPC_INFO_EX* pInfo = (NPC_INFO_EX*)npc_arr[i];
		CString strPos;
		strPos.Format(_npc_pos, pInfo->x, pInfo->y, pInfo->z);

		m_NPCList.InsertItem(i, pInfo->name);
		m_NPCList.SetItemData(i, (DWORD)pInfo);
		m_NPCList.SetItemText(i, 1, strPos);
	}

	if (m_bReadOnly) GetDlgItem(ID_CHECK_IN)->EnableWindow(FALSE);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool CNPCInfoDlg::LoadAndUpdateNPCInfo(bool bGetName)
{
	TaskNPCInfoMap npc_infos;

	if (!LoadNPCInfo(npc_infos))
	{
		AfxMessageBox("读取NPC信息文件失败");
		return false;
	}
	else
	{
		TaskNPCInfoMap& man_map = GetTaskTemplMan()->GetNPCInfoMap();
		npc_arr.reserve(man_map.size());

		TaskNPCInfoMap::iterator it = man_map.begin();
		for (; it != man_map.end(); ++it)
		{
			NPC_INFO_EX* pInfoEx = new NPC_INFO_EX;
			pInfoEx->id = it->first;
			if (bGetName) pInfoEx->name = GetBaseDataNameByID(it->first);
			TaskNPCInfoMap::iterator it2 = npc_infos.find(it->first);

			if (it2 != npc_infos.end())
			{
				pInfoEx->x = it2->second.x;
				pInfoEx->y = it2->second.y;
				pInfoEx->z = it2->second.z;

				it->second.x = it2->second.x;
				it->second.y = it2->second.y;
				it->second.z = it2->second.z;

				if (pInfoEx->x == 0 && pInfoEx->y == 0 && pInfoEx->z == 0)
					m_nUnMatch++;
			}
			else
			{
				pInfoEx->x = 0;
				pInfoEx->y = 0;
				pInfoEx->z = 0;
				m_nUnMatch++;
			}

			npc_arr.push_back(pInfoEx);
		}

		return true;
	}
}

inline bool ReadLine(FILE* fp, char* szLine)
{
	if (fgets(szLine, TASK_NPC_INFO_LINE_LEN, fp) == NULL) return false;
	szLine[strcspn(szLine, "\r\n")] = '\0';
	return true;
}

bool CNPCInfoDlg::LoadNPCInfo(TaskNPCInfoMap& npc_infos)
{
	FILE* fp = fopen(_npc_info_path, "rb");
	if (!fp) return false;

	char line[TASK_NPC_INFO_LINE_LEN];
	long ver = 0;
	long count = 0;
	long i;

	if (!ReadLine(fp, line)) goto fail_ret;
	sscanf(line, _npc_info_ver, &ver);

	if (!ReadLine(fp, line)) goto fail_ret;
	sscanf(line, _npc_info_count, &count);

	for (i = 0; i < count; i++)
	{
		unsigned long npc = 0;
		long x, y, z;

		if (!ReadLine(fp, line)) goto fail_ret;
		sscanf(line, _npc_info, &npc, &x, &y, &z);

		if (npc == 0) goto fail_ret;

		TaskNPCInfoMap::iterator it = npc_infos.find(npc);
		if (it != npc_infos.end()) goto fail_ret;

		NPC_INFO info;
		info.id = npc;
		info.x = (short)x;
		info.y = (short)y;
		info.z = (short)z;
		npc_infos[npc] = info;
	}

	fclose(fp);
	return true;

fail_ret:
	fclose(fp);
	npc_infos.clear();

	return false;
}

bool CNPCInfoDlg::SaveNPCInfo()
{
	FILE* fp = fopen(_npc_info_path, "wb");
	if (!fp) return false;

	fprintf(fp, _npc_info_ver, TASK_NPC_INFO_VERSION);
	fprintf(fp, "\r\n");

	fprintf(fp, _npc_info_count, npc_arr.size());
	fprintf(fp, "\r\n");

	for (size_t i = 0; i < npc_arr.size(); i++)
	{
		const NPC_INFO_EX* pInfo = (const NPC_INFO_EX*)npc_arr[i];

		fprintf(fp, _npc_info, pInfo->id, pInfo->x, pInfo->y, pInfo->z);
		fprintf(fp, "\r\n");
	}

	fclose(fp);
	return true;
}

void CNPCInfoDlg::OnDblclkNpc(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	if (m_bReadOnly) return;

	LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)pNMHDR;
	NPC_INFO_EX* pInfo = (NPC_INFO_EX*)m_NPCList.GetItemData(lpnmitem->iItem);
	CSetNPCInfoDlg dlg;
	dlg.m_strName = pInfo->name;
	dlg.m_x = pInfo->x;
	dlg.m_y = pInfo->y;
	dlg.m_z = pInfo->z;

	if (dlg.DoModal() == IDOK)
	{
		pInfo->x = dlg.m_x;
		pInfo->y = dlg.m_y;
		pInfo->z = dlg.m_z;

		CString strPos;
		strPos.Format(_npc_pos, pInfo->x, pInfo->y, pInfo->z);
		m_NPCList.SetItemText(lpnmitem->iItem, 1, strPos);
	}
}

void CNPCInfoDlg::OnOK()
{
	if (m_bReadOnly) EndDialog(IDOK);
}

void CNPCInfoDlg::OnCancel()
{
	if (m_bReadOnly) EndDialog(IDCANCEL);
}

void CNPCInfoDlg::OnDestroy()
{
	CDialog::OnDestroy();
}

void CNPCInfoDlg::OnFindNpc()
{
	size_t i;

	CString strOld = m_strNPCToFind;
	UpdateData();

	if (strOld != m_strNPCToFind)
		i = 0;
	else
	{
		POSITION pos = m_NPCList.GetFirstSelectedItemPosition();
		if (pos) i = m_NPCList.GetNextSelectedItem(pos) + 1;
		else i = 0;
	}

	for (; i < npc_arr.size(); i++)
	{
		if (npc_arr[i]->name.Find(m_strNPCToFind) >= 0)
		{
			m_NPCList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			m_NPCList.EnsureVisible(i, FALSE);
			break;
		}
	}
}

void CNPCInfoDlg::OnCheckIn()
{
	if (!SaveNPCInfo())
		AfxMessageBox("保存失败");
	else
	{
		g_VSS.SetProjectPath("BaseData");
		g_VSS.CheckInFile(::PathFindFileName(_npc_info_path));
	}

	EndDialog(IDOK);
}
