#include "stdafx.h"
#include "TaskTemplEditor.h" 
#include "ExcelConverter.h"
#include "TemplIDSelDlg.h"
#include "VssOperation.h"
#include "BaseDataTemplate.h"
#include "shlwapi.h"
#include "TaskTempl.h"

extern BOOL IsReadOnly(LPCTSTR szFile);

static const char* TASK_ID_DAT = "BaseData\\TaskID.dat";
static const char* BASE_DATA = "BaseData";
extern BaseDataIDManBase g_TemplIDMan;

CExcelConverter::~CExcelConverter()
{

}

void CExcelConverter::ConvertExcel(const char* filename)
{
	if (!Init())
	{
		AfxMessageBox("TaskID.dat被其他同学占用，请等会再试");
		return;
	}

	ConvertExcelEnd(Convert(filename, "sheet1"));
}

bool CExcelConverter::Init()
{
	g_VSS.SetProjectPath(BASE_DATA);
	g_VSS.CheckOutFile(TASK_ID_DAT);
	m_sBasedata = BASE_DATA;
	if (IsReadOnly(TASK_ID_DAT)) return false;
	LoadTaskID();
	m_vecFilesToCheckIn.clear();
	m_pLog = NULL;
	if (NULL == (m_pLog = freopen("excel_log.txt", "w", stdout))) return false;
	
	return true;
}

void CExcelConverter::ConvertExcelEnd(bool bSucc)
{
	if (bSucc)
	{
		g_VSS.SetProjectPath(BASE_DATA);
		g_VSS.CheckInFile("TaskID.dat");
		for (size_t i = 0; i < m_vecFilesToCheckIn.size(); ++i)
		{
			g_VSS.SetProjectPath(m_vecFilesToCheckIn[i].path);
			g_VSS.CheckInFile(PathFindFileName(m_vecFilesToCheckIn[i].name));
		}
	}
	else
	{
		g_VSS.SetProjectPath(BASE_DATA);
		g_VSS.UndoCheckOut(TASK_ID_DAT);
		for (size_t i = 0; i < m_vecFilesToCheckIn.size(); ++i)
		{
			g_VSS.SetProjectPath(m_vecFilesToCheckIn[i].path);
			g_VSS.UndoCheckOut(m_vecFilesToCheckIn[i].name);
		}
	}

	fclose(m_pLog);
	ShellExecute(0,"open","excel_log.txt",0,0,SW_SHOWNORMAL);
}

bool CExcelConverter::IsTaskMatter( int tid )
{
	AString strPath = g_TemplIDMan.GetPathByID(tid);
	return strPath.Find("\\任务物品") > 0;
}
bool CExcelConverter::AddNpcTask(bool bDelv, unsigned long ulNPC, unsigned long ulTask)
{
	if (g_BaseIDMan.Load("BaseData\\TemplID.dat") != 0)
		return false;

	CString strPath = g_BaseIDMan.GetPathByID(ulNPC);
	BaseDataTempl tmpl;
	unsigned long ulSvrID = 0;
	bool bRet = false;

	if (strPath.IsEmpty())
	{
		// npc被删除
		PrintMessage(true, "找不到npc %d", ulNPC);
		g_BaseIDMan.Release();
		return false;
	}

	if (tmpl.Load(strPath) == 0)
	{
		int nItem = tmpl.GetItemIndex(bDelv ? "发放任务服务" : "验证完成任务服务");
		if (nItem >= 0) ulSvrID = tmpl.GetItemValue(nItem);
		tmpl.Release();
	}

	if (ulSvrID)
	{
		CString strSvr = g_BaseIDMan.GetPathByID(ulSvrID);
		BaseDataTempl svr;

		bool bCurCheckOut;
		int nFind = strSvr.ReverseFind('\\');
		CString strPath = strSvr.Left(nFind);

		if (IsReadOnly(strSvr))
		{
			g_VSS.SetProjectPath(strPath);
			g_VSS.CheckOutFile(strSvr);
			bCurCheckOut = false;
		}
		else
			bCurCheckOut = true;

		if (!IsReadOnly(strSvr))
		{
			if (svr.Load(strSvr) == 0)
			{
				bool bFind = false;

				for (int i = 0; i < svr.GetItemNum(); i++)
				{
					unsigned long ulId = svr.GetItemValue(i);
					if (ulId == ulTask)
					{
						bFind = true;
						PrintMessage(true, "NPC服务模板：%s 中已经有任务id %d", strSvr, ulTask);
						break;
					}
				}

				if (!bFind)
				{
					int i = 0;
					const char* sTask = "任务";
					while (1)
					{
						AString sName = svr.GetItemName(i);
						if (sName.Find(sTask) == 0)
						{
							break;
						}
						++i;
					}
					
					for (; i < svr.GetItemNum(); i++)
					{
						unsigned long ulId = svr.GetItemValue(i);

						if (ulId == 0)
						{
							svr.SetItemValue(i, static_cast<unsigned long>(ulTask));
							bRet = true;
							break;
						}
					}

					if (!bRet)
					{
						PrintMessage(true, "NPC任务已满");
					}
					else
					{
						svr.Save(strSvr);
						WriteMd5CodeToTemplate(strSvr);
					}
				}

				svr.Release();
			}

			if (!bCurCheckOut)
			{
				STR_FILES paths;
				paths.path = strPath;
				paths.name = strSvr;
				m_vecFilesToCheckIn.push_back(paths);
			}
		}
		else
		{
			PrintMessage(true, "CheckOut服务模板失败 %s", strSvr);
		}
	}
	else
	{
		if (bDelv)
			PrintMessage(true, "找不到npc %d 对应的发放任务服务模板", ulNPC);
		else
			PrintMessage(true, "找不到npc %d 对应的验证完成任务服务模板", ulNPC);
	}
		
	g_BaseIDMan.Release();
	return bRet;
}
bool CExcelConverter::ExtractNPC( ATaskTempl* pTask,const String& str,const String& str2 )
{
	int iDelNpc = atoi(str.c_str());
	int iAwardNpc = atoi(str2.c_str());
	bool ret(false);
	while(true)
	{
		// 发放npc为0的话代表自动触发的任务
		if (0 == iDelNpc)
			pTask->m_bAutoDeliver = true;
		else if (AddNpcTask(true, iDelNpc, pTask->m_ID))
			pTask->m_ulDelvNPC = iDelNpc;
		else break;
		// 回收npc为0的话代表直接完成的任务
		if (0 == iAwardNpc)
			pTask->m_enumFinishType	= enumTFTDirect;
		else
		{
			pTask->m_enumFinishType	= enumTFTNPC;
			if (AddNpcTask(false, iAwardNpc, pTask->m_ID))
				pTask->m_ulAwardNPC = iAwardNpc;
			else break;
		}
		
		ret = true;
		break;
	}
	return ret;
}

ATaskTempl* CExcelConverter::DoConvert( const StringVector& data )
{
	ATaskTempl* pTempl = TaskConverter::DoConvert(data);
	if (pTempl)
		m_vecNewTempls.push_back(pTempl);
	else if ("" != data[0])
	{
		for (int i = 0; i < m_vecNewTempls.size(); ++i)
			delete m_vecNewTempls[i];
		m_vecNewTempls.clear();
	}
	// 表示excel截止
	else pTempl = (ATaskTempl*)1;
	return pTempl;
}