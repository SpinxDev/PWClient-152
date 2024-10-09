// PolicyDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "Global.h"
#include "ElementData.h"
#include "PolicyDlg.h"
#include "PolicyTriggerDlg.h"
#include "BaseDataIDMan.h"
#include "PolicyCreateDlg.h"
#include "Policy.h"
#include "VssOperation.h"

#include <set>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString strOperationList[];
extern CString GetVarTypeName(int type);
extern CString GetOperatorName(int op);
extern CString GetPatrolType(int type);
extern CString GetPatrolSpeedType(int type);

//////////////////////////////////////////////////////////////////////////
//	复制和粘贴触发器
class CTriggerDataPaster{
	CString			m_strPolicyPath;	//	待复制的策略文件路径（当在不同策略间复制触发器时、需要据此加载原策略）
	unsigned int	m_uPolicyID;		//	待复制触发器的策略ID（供验证）
	unsigned int	m_uTriggerID;		//	待复制的触发器ID（复制来源）
	CString			m_strTriggerName;	//	待复制的触发器名称（供验证）（仅 m_uTriggerID 不够，因为触发器可能删除、被删除触发器ID可以重用）
public:
	CTriggerDataPaster()
		: m_uPolicyID(-1)
		, m_uTriggerID(-1)
	{
	}
	void SetSource(const CString &strPolicyPath, unsigned int uPolicyID, unsigned int uTriggerID, const CString &strTriggerName){
		m_strPolicyPath = strPolicyPath;
		m_uPolicyID = uPolicyID;
		m_uTriggerID = uTriggerID;
		m_strTriggerName = strTriggerName;
	}
	bool CanPaste()const{
		return	!m_strPolicyPath.IsEmpty() && !m_strTriggerName.IsEmpty() && m_uPolicyID != -1 && m_uTriggerID != -1;
	}
	CTriggerData * PasteTo(CPolicyData * pTargetPolicy)const{
		if (!CanPaste() || !pTargetPolicy){
			return NULL;
		}
		if (pTargetPolicy->GetID() == m_uPolicyID){
			return PasteInPlace(pTargetPolicy);
		}else{
			return PasteToOtherPolicy(pTargetPolicy);
		}
	}
private:
	static unsigned int GetOperationTriggerID(const CTriggerData::_s_operation *pOperation){
		unsigned int result(0);
		switch (pOperation->iType){
		case CTriggerData::o_run_trigger:
			result = ((O_RUN_TRIGGER *)pOperation->pParam)->uID;
			break;
		case CTriggerData::o_stop_trigger:
			result = ((O_STOP_TRIGGER *)pOperation->pParam)->uID;
			break;
		case CTriggerData::o_active_trigger:
			result = ((O_ACTIVE_TRIGGER *)pOperation->pParam)->uID;
			break;
		default:
			ASSERT(false);
			break;
		}
		return result;
	}
	static void SetOperationTriggerID(CTriggerData::_s_operation *pOperation, unsigned int uNewTriggerID){
		switch (pOperation->iType){
		case CTriggerData::o_run_trigger:
			((O_RUN_TRIGGER *)pOperation->pParam)->uID = uNewTriggerID;
			break;
		case CTriggerData::o_stop_trigger:
			((O_STOP_TRIGGER *)pOperation->pParam)->uID = uNewTriggerID;
			break;
		case CTriggerData::o_active_trigger:
			((O_ACTIVE_TRIGGER *)pOperation->pParam)->uID = uNewTriggerID;
			break;
		default:
			ASSERT(false);
			break;
		}
	}
	CTriggerData * PasteInPlace(CPolicyData *pTargetPolicy)const{
		//	在同一策略内部粘贴触发器
		CPolicyData *pSourcePolicy = pTargetPolicy;
		if (!IsSourceValid(pSourcePolicy)){
			return NULL;
		}
		CTriggerData *pSourceTrigger = pSourcePolicy->GetTriggerPtrByID(m_uTriggerID);

		//	拷贝触发器本身
		CTriggerData *pNewTrigger = CopyTrigger(pSourceTrigger, pTargetPolicy);
		RenameTrigger(pNewTrigger);

		//	拷贝嵌套引用的触发器
		TriggerPtrArray targetTriggersToDeepCopy;
		targetTriggersToDeepCopy.push_back(pNewTrigger);

		while (!targetTriggersToDeepCopy.empty()){
			CTriggerData *pDeepCopyTrigger = targetTriggersToDeepCopy.front();
			targetTriggersToDeepCopy.erase(targetTriggersToDeepCopy.begin());
			
			for (int i(0); i < pDeepCopyTrigger->GetOperaionNum(); ++ i){
				CTriggerData::_s_operation *pOperation = pDeepCopyTrigger->GetOperaion(i);				
				if (CTriggerData::o_run_trigger != pOperation->iType){
					continue;
				}
				CTriggerData *pSourceDeepTrigger = pSourcePolicy->GetTriggerPtrByID(GetOperationTriggerID(pOperation));
				CTriggerData *pNewDeepTrigger = CopyTrigger(pSourceDeepTrigger, pTargetPolicy);
				SetOperationTriggerID(pOperation, pNewDeepTrigger->GetID());
				targetTriggersToDeepCopy.push_back(pNewDeepTrigger);
			}
		}
		return pNewTrigger;
	}
	CTriggerData * PasteToOtherPolicy(CPolicyData *pTargetPolicy)const{
		//	在不同策略之间粘贴触发器
		CPolicyData *pSourcePolicy = LoadSourcePolicyFromFile();
		if (!pSourcePolicy){
			return NULL;
		}
		typedef std::map<CTriggerData *, CTriggerData *> Source2TargetTriggerPtrMap;
		Source2TargetTriggerPtrMap source2TargetTriggerPtrMap;	//	从SourcePolicy中Trigger到拷贝的TargetPolicy中Trigger的查找表
		TriggerPtrArray	sourceTriggersToDeepCopy;				//	需检查 Operation 并进行深度策略拷贝的数组

		CTriggerData *pTrigger = pSourcePolicy->GetTriggerPtrByID(m_uTriggerID);
		CTriggerData *pNewTrigger = CopyTrigger(pTrigger, pTargetPolicy);
		RenameTrigger(pNewTrigger);
		source2TargetTriggerPtrMap[pTrigger] = pNewTrigger;
		sourceTriggersToDeepCopy.push_back(pTrigger);

		while (!sourceTriggersToDeepCopy.empty()){
			CTriggerData *pSourceTrigger = sourceTriggersToDeepCopy.front();
			sourceTriggersToDeepCopy.erase(sourceTriggersToDeepCopy.begin());
			for (int i(0); i < pSourceTrigger->GetOperaionNum(); ++ i){
				CTriggerData::_s_operation *pSourceOperation = pSourceTrigger->GetOperaion(i);
				switch (pSourceOperation->iType){
				case CTriggerData::o_run_trigger:
				case CTriggerData::o_stop_trigger:
				case CTriggerData::o_active_trigger:
					{
						unsigned int uSourceOperationTriggerID = GetOperationTriggerID(pSourceOperation);
						CTriggerData *pSourceTrigger2 = pSourcePolicy->GetTriggerPtrByID(uSourceOperationTriggerID);
						CTriggerData *pTargetTrigger2 = source2TargetTriggerPtrMap[pSourceTrigger2];
						if (!pTargetTrigger2){
							pTargetTrigger2 = CopyTrigger(pSourceTrigger2, pTargetPolicy);
							switch (pSourceOperation->iType){
							case CTriggerData::o_stop_trigger:
							case CTriggerData::o_active_trigger:
								RenameTrigger(pTargetTrigger2);
								break;
							}
							source2TargetTriggerPtrMap[pSourceTrigger2] = pTargetTrigger2;
							sourceTriggersToDeepCopy.push_back(pSourceTrigger2);
						}
						CTriggerData *pTargetTrigger = source2TargetTriggerPtrMap[pSourceTrigger];
						CTriggerData::_s_operation *pTargetOperation = pTargetTrigger->GetOperaion(i);
						ASSERT(pTargetOperation->iType == pSourceOperation->iType);
						SetOperationTriggerID(pTargetOperation, pTargetTrigger2->GetID());
					}
					break;
				}
			}
		}

		pSourcePolicy->Release();
		delete pSourcePolicy;
		return pNewTrigger;
	}
	CPolicyData * LoadSourcePolicyFromFile()const{
		CPolicyData *result = NULL;
		if (!m_strPolicyPath.IsEmpty()){
			result = new CPolicyData;
			if (!result->Load(m_strPolicyPath) ||
				result->GetID() != m_uPolicyID ||
				!IsSourceValid(result)){
				result->Release();
				delete result;
				result = NULL;
			}
		}
		return result;
	}
	typedef abase::vector<CTriggerData *> TriggerPtrArray;
	TriggerPtrArray CollectReferencedSourceTriggers(CPolicyData *pSourcePolicy)const{
		TriggerPtrArray	result;
		TriggerPtrArray	triggersToDeepCollect;
		triggersToDeepCollect.push_back(pSourcePolicy->GetTriggerPtrByID(m_uTriggerID));
		while (!triggersToDeepCollect.empty()){
			CTriggerData *pDeepCollectTrigger = triggersToDeepCollect.front();
			triggersToDeepCollect.erase(triggersToDeepCollect.begin());
			if (std::find(result.begin(), result.end(), pDeepCollectTrigger) != result.end()){
				continue;
			}
			result.push_back(pDeepCollectTrigger);
			for (int i(0); i < pDeepCollectTrigger->GetOperaionNum(); ++ i){
				CTriggerData::_s_operation *pOperation = pDeepCollectTrigger->GetOperaion(i);
				switch (pOperation->iType){
				case CTriggerData::o_stop_trigger:
				case CTriggerData::o_active_trigger:
					{
						unsigned int uDeepTriggerID = GetOperationTriggerID(pOperation);
						triggersToDeepCollect.push_back(pSourcePolicy->GetTriggerPtrByID(uDeepTriggerID));
					}
					break;
				}
			}
		}
		return result;
	}
	bool IsSourceValid(CPolicyData *pSourcePolicy)const{
		CTriggerData *pSourceTrigger = GetSourceTrigger(pSourcePolicy);
		if (!pSourceTrigger){
			return false;
		}
		std::set<unsigned int> checkedTriggers;	//	已检查的 Trigger ID 集合，避免触发器内调用自身时死循环
		TriggerPtrArray triggersToDeepCheck;	//	需检查的 Trigger 指针列表
		triggersToDeepCheck.push_back(pSourceTrigger);
		while (!triggersToDeepCheck.empty()){
			CTriggerData *pDeepCheckTrigger = triggersToDeepCheck.front();
			triggersToDeepCheck.erase(triggersToDeepCheck.begin());
			checkedTriggers.insert(pDeepCheckTrigger->GetID());
			for (int i(0); i < pDeepCheckTrigger->GetOperaionNum(); ++ i){
				CTriggerData::_s_operation *pOperation = pDeepCheckTrigger->GetOperaion(i);
				switch (pOperation->iType){
				case CTriggerData::o_run_trigger:
				case CTriggerData::o_stop_trigger:
				case CTriggerData::o_active_trigger:
					{
						unsigned int uDeepTriggerID = GetOperationTriggerID(pOperation);
						CTriggerData *pDeepTrigger = pSourcePolicy->GetTriggerPtrByID(uDeepTriggerID);
						if (!pDeepTrigger){
							return false;
						}
						if (checkedTriggers.find(uDeepTriggerID) == checkedTriggers.end()){
							triggersToDeepCheck.push_back(pDeepTrigger);
						}
					}
					break;
				}
			}
		}
		return true;
	}
	CTriggerData * GetSourceTrigger(CPolicyData *pSourcePolicy)const{
		CTriggerData *result = NULL;
		if (pSourcePolicy != NULL && pSourcePolicy->GetID() == m_uPolicyID){
			CTriggerData *pSourceTrigger = pSourcePolicy->GetTriggerPtrByID(m_uTriggerID);
			if (pSourceTrigger != NULL && m_strTriggerName == pSourceTrigger->GetName()){
				result = pSourceTrigger;
			}
		}
		return result;
	}
	static CTriggerData * CopyTrigger(CTriggerData *pSourceTrigger, CPolicyData *pTargetPolicy){
		CTriggerData *pNewTrigger = pSourceTrigger->CopyObject();
		pNewTrigger->SetID(pTargetPolicy->GetNextTriggerID());
		pTargetPolicy->AddTriggerPtr(pNewTrigger);
		return pNewTrigger;
	}
	static void RenameTrigger(CTriggerData *pNewTrigger){
		pNewTrigger->SetName(CString(pNewTrigger->GetName())+" Copy");
	}
};

static CTriggerDataPaster s_triggerPaster;
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPolicyDlg dialog


CPolicyDlg::CPolicyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPolicyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPolicyDlg)
	m_fHp = 0.0f;
	m_bSkipKillPlayer = FALSE;
	m_bSkipRandom = FALSE;
	m_bSkipStartAttack = FALSE;
	m_bSkipDied = FALSE;
	m_bTimePoint = FALSE;
	m_bSkipReachEnd = false;
	m_bSkipReachEnd2 = false;
	m_iHistoryStage = -1;
	m_bSkipStopFight = false;
	m_bHasFilter = false;
	//}}AFX_DATA_INIT
	m_pCurrentPolicy = NULL;
	m_bReadOnly = true;
	m_bStartTest =  false;
}


void CPolicyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPolicyDlg)
	DDX_Control(pDX, IDC_EDIT_RUN_LOG, m_editRunLog);
	DDX_Control(pDX, IDC_LIST_TRIGGER, m_listTrigger);
	DDX_Control(pDX, IDC_TREE_POLICY, m_Tree);
	DDX_Text(pDX, IDC_EDIT_HP, m_fHp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPolicyDlg, CDialog)
	//{{AFX_MSG_MAP(CPolicyDlg)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_POLICY, OnRclickTreePolicy)
	ON_COMMAND(ID_CREATE_POLICY, OnCreatePolicy)
	ON_COMMAND(ID_POLICY_CHECK_IN, OnPolicyCheckIn)
	ON_COMMAND(ID_POLICY_CHECK_OUT, OnPolicyCheckOut)
	ON_COMMAND(ID_POLICY_UNDO_CHECK_OUT, OnPolicyUndoCheckOut)
	ON_BN_CLICKED(IDC_BUTTON_DEL_TRIGGER, OnButtonDelTrigger)
	ON_BN_CLICKED(IDC_BUTTON_ADD_TRIGGER, OnButtonAddTrigger)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_UP, OnButtonMoveUp)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_DOWN, OnButtonMoveDown)
	ON_BN_CLICKED(IDC_BUTTON_POLICY_SAVE, OnButtonPolicySave)
	ON_BN_CLICKED(IDC_BUTTON_COPY_TRIGGER, OnButtonCopyTrigger)
	ON_BN_CLICKED(IDC_BUTTON_PASTE_TRIGGER, OnButtonPasteTrigger)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_POLICY, OnDblclkTreePolicy)
	ON_LBN_DBLCLK(IDC_LIST_TRIGGER, OnDblclkListTrigger)
	ON_BN_CLICKED(IDC_BUTTON_RUN_TRIGGER, OnButtonRunTrigger)
	ON_BN_CLICKED(IDC_BUTTON_STOP_TRIGGER, OnButtonStopTrigger)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_EVENT_RANDOM, OnButtonEventRandom)
	ON_BN_CLICKED(IDC_BUTTON_EVENT_KILLPLAYER, OnButtonEventKillplayer)
	ON_BN_CLICKED(IDC_BUTTON_EVENT_ATTACKSTART, OnButtonEventAttackstart)
	ON_BN_CLICKED(IDC_BUTTON_EVENT_DIED, OnButtonEventDied)
	ON_BN_CLICKED(IDC_BUTTON_EVENT_TIME_POINT, OnButtonEventTimePoint)
	ON_BN_CLICKED(ID_BTN_FIND_POLICY, OnBtnFindPolicy)
	ON_BN_CLICKED(IDC_BUTTON_EVENT_REACHEND, OnButtonEventReachend)
	ON_BN_CLICKED(IDC_BUTTON_EVENT_AT_HISTORY_STAGE, OnButtonEventAtHistoryStage)
	ON_BN_CLICKED(IDC_BUTTON_EVENT_STOPFIGHT, OnButtonEventStopFight)
	ON_BN_CLICKED(IDC_BUTTON_EVENT_REACHEND2, OnButtonEventReachend2)
	ON_BN_CLICKED(IDC_BUTTON_EVENT_HASFILTER, OnButtonEventHasFilter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPolicyDlg message handlers
bool CPolicyDlg::LoadIDTemplate()
{
	g_PolicyIDMan.Release();
	if (g_PolicyIDMan.Load("PolicyData\\PolicyID.dat") != 0)
	{
		AfxMessageBox("打开TPolicyData\\PolicyID.dat文件失败", MB_ICONSTOP);
		return false;
	}
	return true;
}

void CPolicyDlg::EnumFiles(CString strPathName, HTREEITEM hTreeItemp)
{
	CTreeCtrl* pCtrl = NULL;
	pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_POLICY);
	ASSERT(pCtrl != NULL);
	
	CFileFind finder;
	CString strWildcard;
	strWildcard = strPathName + "\\*.ply";
	
	BOOL bWorking = finder.FindFile(strWildcard);
	int n = 0;
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		
		//是"."或是".." 跳过.
		if (finder.IsDots())
			continue;
		
		// 是文件夹，继续搜寻.
		if (finder.IsDirectory())
		{
			continue;
		}
		// 是文件.
		CString ext = finder.GetFileName();
		CString path = finder.GetFilePath();
		ext.Replace(g_szWorkDir,"");
		
		HTREEITEM insertAfter = TVI_FIRST;
		HTREEITEM hNewItem;
		
		hNewItem = pCtrl->InsertItem(ext,hTreeItemp,insertAfter);		
		
		if(FileIsReadOnly(AString(path)))
		{
			pCtrl->SetItemImage(hNewItem,4,4);
		}else
		{
			pCtrl->SetItemImage(hNewItem,5,5);
		}
	}
	finder.Close();
}

BOOL CPolicyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// TODO: Add extra initialization here
	m_ImageList.Create(IDB_LISTTAG, 16, 3, RGB(255, 0, 255));
    m_Tree.SetImageList(&m_ImageList,TVSIL_NORMAL);
	
	HTREEITEM insertAfter = TVI_FIRST;
	HTREEITEM hNewItem;
	hNewItem = m_Tree.InsertItem("怪物策略",NULL,insertAfter);
	m_Tree.SetItemImage(hNewItem,1,0);
	CString strPath(g_szWorkDir);
	strPath += "\\PolicyData";
	EnumFiles(strPath, hNewItem);
	LoadIDTemplate();
	m_editRunLog.SetLimitText(1024*100);

	SetTimer(0,1000,NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPolicyDlg::OnRclickTreePolicy(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint point;
	GetCursorPos(&point);
	POINT pt = {point.x, point.y};
	m_Tree.ScreenToClient(&pt);
	CPoint client(pt.x,pt.y);
	pt.x = point.x;
	pt.y = point.y;
	
	CMenu Menu;
	Menu.LoadMenu(IDR_MENU_POLICY);
	
	CMenu* pPopupMenu = Menu.GetSubMenu(0);
	ASSERT(pPopupMenu);
	
	
	UpdateMenu(pPopupMenu);
	pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);
	*pResult = 0;
}

void CPolicyDlg::UpdateMenu(CMenu *pMenu)
{
	/*
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_SORT_LIST);
	HTREEITEM itemp = pCtrl->GetSelectedItem();
	HTREEITEM oldItem = itemp;
	CString itemText = pCtrl->GetItemText(itemp);
	CString path = itemText;
	if(stricmp("附加属性",path)==0)
	{
		pMenu->EnableMenuItem(ID_CREATE_OBJECT_EXT,MF_GRAYED);
		pMenu->EnableMenuItem(ID_CREATE_OBJECT,MF_GRAYED);
		pMenu->EnableMenuItem(ID_VSS_CHECK_IN,MF_GRAYED);
		pMenu->EnableMenuItem(ID_VSS_CHECK_OUT,MF_GRAYED);
		pMenu->EnableMenuItem(ID_EDIT_TEMPLATE_PROPERTY,MF_GRAYED);
		pMenu->EnableMenuItem(ID_MODIFIED_OBJECT_EXT,MF_GRAYED);
		return;
	}
	
	while(itemp = pCtrl->GetParentItem(itemp))
	{
		CString pathsub = pCtrl->GetItemText(itemp);
		path = pathsub + "\\" + path;	
	}
	path = "BaseData\\" + path;

	if(path.Find(".tmpl")!=-1 || path.Find(".ext")!=-1)
	{
		pMenu->EnableMenuItem(ID_CREATE_OBJECT,MF_GRAYED);
		
		if(FileIsReadOnly(AString(path)))
		{
			pMenu->EnableMenuItem(ID_VSS_CHECK_OUT,MF_ENABLED);
			pMenu->EnableMenuItem(ID_VSS_CHECK_IN,MF_GRAYED);	
		}else 
		{
			pMenu->EnableMenuItem(ID_VSS_CHECK_OUT,MF_GRAYED);
			pMenu->EnableMenuItem(ID_VSS_CHECK_IN,MF_ENABLED);
		}
		pMenu->EnableMenuItem(ID_EDIT_TEMPLATE_PROPERTY,MF_ENABLED);
	}else 
	{
		if(FileIsExist(AString(path + "\\" + itemText + ".tmpl")))
		{
			if(FileIsReadOnly(AString(path + "\\" + itemText + ".tmpl")))
			{
				pMenu->EnableMenuItem(ID_VSS_CHECK_OUT,MF_ENABLED);
				pMenu->EnableMenuItem(ID_VSS_CHECK_IN,MF_GRAYED);	
			}else 
			{
				pMenu->EnableMenuItem(ID_VSS_CHECK_OUT,MF_GRAYED);
				pMenu->EnableMenuItem(ID_VSS_CHECK_IN,MF_ENABLED);
			}
			pMenu->EnableMenuItem(ID_EDIT_TEMPLATE_PROPERTY,MF_ENABLED);
		}else
		{
			pMenu->EnableMenuItem(ID_VSS_CHECK_OUT,MF_GRAYED);
			pMenu->EnableMenuItem(ID_VSS_CHECK_IN,MF_GRAYED);
			pMenu->EnableMenuItem(ID_EDIT_TEMPLATE_PROPERTY,MF_GRAYED);
		}
		
		if(!IsHideFiles(path))
		{
			pMenu->EnableMenuItem(ID_CREATE_OBJECT,MF_GRAYED);
		}else pMenu->EnableMenuItem(ID_CREATE_OBJECT,MF_ENABLED);
	}

	bool bPass = false;
	for (ORG_PATHS::iterator n = _org_paths.begin(); n != _org_paths.end(); ++n)
	{
		AString strOrgPath = n->first;
		
		if(stricmp(path,strOrgPath) == 0)
		{
			bPass = true;
		}
	}
	
	if(bPass)
	{
		bPass = false;
		itemp = oldItem;
		HTREEITEM child = pCtrl->GetChildItem(itemp);
		CString txt = pCtrl->GetItemText(child);
		if(stricmp(txt,"附加属性")==0) bPass = true;
		while(child && !bPass)
		{
			child = pCtrl->GetNextItem(child,TVGN_NEXT);
			if(child)
			{
				txt = pCtrl->GetItemText(child);
				if(stricmp(txt,"附加属性")==0)
				{
					bPass = true;
					break;
				}
			}
		}
	}
	if(!bPass) pMenu->EnableMenuItem(ID_CREATE_OBJECT_EXT,MF_GRAYED);
	else pMenu->EnableMenuItem(ID_CREATE_OBJECT_EXT,MF_ENABLED);

	if(itemText.Find(".ext")!=-1)
	{	if(!FileIsReadOnly(AString(path)))
			pMenu->EnableMenuItem(ID_MODIFIED_OBJECT_EXT,MF_ENABLED);
		else pMenu->EnableMenuItem(ID_MODIFIED_OBJECT_EXT,MF_GRAYED);
	}
	else pMenu->EnableMenuItem(ID_MODIFIED_OBJECT_EXT,MF_GRAYED);
	*/
}

void CPolicyDlg::OnCreatePolicy() 
{
	// TODO: Add your command handler code here
	CPolicyCreateDlg dlg;
	CPolicyData mPolicyData;
	if(IDOK == dlg.DoModal())
	{
		CString pathName;
		int k = 0;
		pathName.Format("PolicyData\\%s.ply",dlg.m_szPolicyName);
		m_nPolicyID = g_PolicyIDMan.CreateID(AString(pathName));
		if(m_nPolicyID==0) 
		{
			MessageBox("创建失败，可能已经有相同的策略名字存在!");
			return;
		}
		//如果要复制
		CString& strSelPolicyPath = dlg.m_szPolicyCopyPath;
		if(strSelPolicyPath.GetLength() > 0)
		{
			if(!mPolicyData.Load(strSelPolicyPath))
			{
				CString strMSG;
				strMSG.Format("加载复制的策略文件: %s 失败!",strSelPolicyPath);
				MessageBox(strMSG);
				return;
			}
		}
		mPolicyData.SetID(m_nPolicyID);
		//保存
		if(!mPolicyData.Save(pathName.GetBuffer(0)))
		{
			MessageBox("CPolicyDlg::OnCreatePolicy(),保存文件失败!");
			return;
		}
		pathName.ReleaseBuffer();
		g_VSS.SetProjectPath("PolicyData");
		g_VSS.AddFile(pathName);
		g_VSS.CheckInFile(pathName);
		pathName.Format("%s.ply",dlg.m_szPolicyName);
		HTREEITEM hNewItem;
		hNewItem = m_Tree.InsertItem(pathName,m_Tree.GetRootItem(),TVI_FIRST);
		pathName.Format("%sPolicyData\\%s.ply",g_szWorkDir,dlg.m_szPolicyName);
		if(FileIsReadOnly(AString(pathName)))
		{
			m_Tree.SetItemImage(hNewItem,4,4);
		}else
		{
			m_Tree.SetItemImage(hNewItem,5,5);
		}
		FreshTriggerList();
	}
	mPolicyData.Release();
}

void CPolicyDlg::OnPolicyCheckIn() 
{
	// TODO: Add your command handler code here
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_POLICY);
	HTREEITEM itemp = pCtrl->GetSelectedItem();
	CString fileName = pCtrl->GetItemText(itemp);
	CString pathName = "PolicyData\\" + fileName;
	if(-1==fileName.Find(".ply")) return;
	
	if(m_strCurrentPathName==pathName && m_pCurrentPolicy)
	{
		if(!m_bReadOnly && g_bPolicyModifyed) 
		{
			if(IDYES==AfxMessageBox("策略数据已经改变，你需要存盘吗?",MB_YESNO|MB_ICONQUESTION))
			{
				m_pCurrentPolicy->Save(m_strCurrentPathName.GetBuffer(0));
				g_bPolicyModifyed = false;
			}
		}
	}
	
	g_VSS.SetProjectPath("PolicyData");
	g_VSS.CheckInFile(fileName);
	fileName = "PolicyData\\" + fileName;
	if(FileIsReadOnly(AString(fileName)))
	{
		m_bReadOnly = true;
		pCtrl->SetItemImage(itemp,4,4);
	}else
	{
		pCtrl->SetItemImage(itemp,5,5);
	}	
	
}

void CPolicyDlg::OnPolicyCheckOut() 
{
	// TODO: Add your command handler code here
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_POLICY);
	HTREEITEM itemp = pCtrl->GetSelectedItem();
	CString fileName = pCtrl->GetItemText(itemp);
	if(-1==fileName.Find(".ply")) return;
	
	fileName = "PolicyData\\" + fileName;
	g_VSS.SetProjectPath("PolicyData");
	g_VSS.CheckOutFile(fileName);
	if(FileIsReadOnly(AString(fileName)))
	{
		pCtrl->SetItemImage(itemp,4,4);
	}else
	{
		pCtrl->SetItemImage(itemp,5,5);
	}
	
	if(FileIsReadOnly(AString(fileName)))
	{
		m_bReadOnly = true;
		pCtrl->SetItemImage(itemp,4,4);
	}else
	{
		m_bReadOnly = false;
		pCtrl->SetItemImage(itemp,5,5);
	}
	
	if(m_strCurrentPathName==fileName && m_pCurrentPolicy)
	{
		g_bPolicyModifyed = false;
		if(FileIsReadOnly(AString(fileName)))
		{
			MessageBox("Check Out 失败!");
			return;
		}
		m_pCurrentPolicy->Release();
		if(!m_pCurrentPolicy->Load(fileName.GetBuffer(0)))
		{
			delete m_pCurrentPolicy;
			m_pCurrentPolicy = NULL;
			MessageBox("打开文件失败!");
			return;
		}
		FreshTriggerList();
	}
}

void CPolicyDlg::OnPolicyUndoCheckOut(){
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_POLICY);
	HTREEITEM itemp = pCtrl->GetSelectedItem();
	CString fileName = pCtrl->GetItemText(itemp);
	if(-1==fileName.Find(".ply")) return;
	
	fileName = "PolicyData\\" + fileName;
	g_VSS.SetProjectPath("PolicyData");
	g_VSS.UndoCheckOut(fileName);	
	if(FileIsReadOnly(AString(fileName)))
	{
		m_bReadOnly = true;
		pCtrl->SetItemImage(itemp,4,4);
	}else
	{
		m_bReadOnly = false;
		pCtrl->SetItemImage(itemp,5,5);
	}
	
	if(m_strCurrentPathName==fileName && m_pCurrentPolicy)
	{
		g_bPolicyModifyed = false;
		if(!FileIsReadOnly(AString(fileName)))
		{
			MessageBox("Undo Check Out 失败!");
			return;
		}
		m_pCurrentPolicy->Release();
		if(!m_pCurrentPolicy->Load(fileName.GetBuffer(0)))
		{
			delete m_pCurrentPolicy;
			m_pCurrentPolicy = NULL;
			MessageBox("打开文件失败!");
			return;
		}
		FreshTriggerList();
	}
}

void CPolicyDlg::OnButtonDelTrigger() 
{
	// TODO: Add your control notification handler code here
	OnButtonStopTrigger();
	if(m_pCurrentPolicy==NULL || m_bStartTest ) return;
	int sel = m_listTrigger.GetCurSel();
	if(sel == -1) return;
	DWORD id = m_listTrigger.GetItemData(sel);
	for( int i = 0; i < m_pCurrentPolicy->GetTriggerPtrNum(); ++i)
	{
		CTriggerData *pData = m_pCurrentPolicy->GetTriggerPtr(i);
		if(id==pData->GetID())
		{
			if(IDYES==AfxMessageBox("你确定要删除当前触发器吗?",MB_YESNO|MB_ICONQUESTION))
			{
				m_pCurrentPolicy->DelTriggerPtr(i);
				//这个本来是个比较费时的操作，不过对于数据量很少的策略来说是可以不计较的
				//关联的触发器也是需要删除的
				DelRedundancy();
			}
			break;
		}
	}
	FreshTriggerList();
	g_bPolicyModifyed =  true;
}

void CPolicyDlg::OnButtonCopyTrigger() 
{
	OnButtonStopTrigger();
	if(m_pCurrentPolicy==NULL || m_bStartTest) return;	
	int sel = m_listTrigger.GetCurSel();
	if (sel == -1) return;
	DWORD uSourceTriggerID = m_listTrigger.GetItemData(sel);
	CTriggerData *pSourceTrigger = m_pCurrentPolicy->GetTriggerPtrByID(uSourceTriggerID);
	s_triggerPaster.SetSource(m_strCurrentPathName, m_pCurrentPolicy->GetID(), uSourceTriggerID, pSourceTrigger->GetName());
}

void CPolicyDlg::OnButtonPasteTrigger() 
{
	OnButtonStopTrigger();
	if(m_pCurrentPolicy==NULL || m_bStartTest) return;
	if (!s_triggerPaster.CanPaste()){
		return;
	}
	if (CTriggerData *pNewTrigger = s_triggerPaster.PasteTo(m_pCurrentPolicy)){
		FreshTriggerList();
		SelectTrigger(pNewTrigger->GetID());
		g_bPolicyModifyed = true;
	}
}

void CPolicyDlg::OnButtonAddTrigger() 
{
	// TODO: Add your control notification handler code here
	OnButtonStopTrigger();
	if(m_pCurrentPolicy==NULL || m_bStartTest) return;

	CPolicyTriggerDlg dlg;
	dlg.m_pCurrentPolicy = m_pCurrentPolicy;
	if(IDOK==dlg.DoModal())
	{
		if(dlg.m_pTriggerData)
		{
			dlg.m_pTriggerData->SetRunStatus(false);
			m_pCurrentPolicy->AddTriggerPtr(dlg.m_pTriggerData);
		}
		FreshTriggerList();
		if (dlg.m_pTriggerData){
			SelectTrigger(dlg.m_pTriggerData->GetID());
		}
		g_bPolicyModifyed = true;
	}
	//上面操作可能产生多余的触发器，所以必须做清理操作
	DelRedundancy();
}

void CPolicyDlg::OnButtonMoveUp() 
{
	OnButtonStopTrigger();
	if(m_pCurrentPolicy==NULL || m_bStartTest) return;
	// TODO: Add your control notification handler code here
	int sel = m_listTrigger.GetCurSel();
	if(sel == -1 || sel == 0) return;
	
	void *pData1, *pData2;
	DWORD id1 = m_listTrigger.GetItemData(sel);
	DWORD id2 = m_listTrigger.GetItemData(sel-1);
	
	int idx1,idx2;
	int i(0);
	for( i = 0; i < m_pCurrentPolicy->GetTriggerPtrNum(); ++i)
	{
		CTriggerData *pData = m_pCurrentPolicy->GetTriggerPtr(i);
		if(id1==pData->GetID())
		{
			pData1 = m_pCurrentPolicy->GetTriggerPtr(i);
			idx1 = i;
			break;
		}
	}
	for( i = 0; i < m_pCurrentPolicy->GetTriggerPtrNum(); ++i)
	{
		CTriggerData *pData = m_pCurrentPolicy->GetTriggerPtr(i);
		if(id2==pData->GetID())
		{
			pData2 = m_pCurrentPolicy->GetTriggerPtr(i);
			idx2 = i;
			break;
		}
	}

	m_pCurrentPolicy->SetTriggerPtr(idx1,(CTriggerData*)pData2);
	m_pCurrentPolicy->SetTriggerPtr(idx2,(CTriggerData*)pData1);
	FreshTriggerList();
	m_listTrigger.SetCurSel(sel-1);
	g_bPolicyModifyed=  true;
}

void CPolicyDlg::OnButtonMoveDown() 
{
	OnButtonStopTrigger();
	if(m_pCurrentPolicy==NULL || m_bStartTest) return;
	// TODO: Add your control notification handler code here
	int sel = m_listTrigger.GetCurSel();
	if(sel == -1 || sel > m_listTrigger.GetCount()-2) return;
	void *pData1, *pData2;
	DWORD id1 = m_listTrigger.GetItemData(sel);
	DWORD id2 = m_listTrigger.GetItemData(sel+1);
	
	int idx1,idx2;
	int i(0);
	for( i = 0; i < m_pCurrentPolicy->GetTriggerPtrNum(); ++i)
	{
		CTriggerData *pData = m_pCurrentPolicy->GetTriggerPtr(i);
		if(id1==pData->GetID())
		{
			pData1 = m_pCurrentPolicy->GetTriggerPtr(i);
			idx1 = i;
			break;
		}
	}
	for( i = 0; m_pCurrentPolicy->GetTriggerPtrNum(); ++i)
	{
		CTriggerData *pData = m_pCurrentPolicy->GetTriggerPtr(i);
		if(id2==pData->GetID())
		{
			pData2 = m_pCurrentPolicy->GetTriggerPtr(i);
			idx2 = i;
			break;
		}
	}

	m_pCurrentPolicy->SetTriggerPtr(idx1,(CTriggerData*)pData2);
	m_pCurrentPolicy->SetTriggerPtr(idx2,(CTriggerData*)pData1);
	FreshTriggerList();
	m_listTrigger.SetCurSel(sel+1);
	g_bPolicyModifyed =  true;
}

void CPolicyDlg::FreshTriggerList()
{
	if(m_pCurrentPolicy==NULL) return;
	int count = m_listTrigger.GetCount();
	int sel =m_listTrigger.GetCurSel();
	int i(0);
	for( i = 0; i < count; i++ )
		m_listTrigger.DeleteString(0);

	int n = m_pCurrentPolicy->GetTriggerPtrNum();
	for( i = 0; i < n; ++i)
	{
		CTriggerData *pData = m_pCurrentPolicy->GetTriggerPtr(i);
		if(!pData->IsRun()) 
		{
			int index = m_listTrigger.AddString(pData->GetName());
			m_listTrigger.SetItemData(index,pData->GetID());
		}
	}

	if(sel!=-1 && sel < m_listTrigger.GetCount()) m_listTrigger.SetCurSel(sel);
}

void CPolicyDlg::SelectTrigger(unsigned int uTriggerID){	
	for (int i(0); i < m_listTrigger.GetCount(); ++ i){
		if (m_listTrigger.GetItemData(i) == uTriggerID){
			m_listTrigger.SetCurSel(i);
			break;
		}
	}
}

void CPolicyDlg::OnButtonPolicySave() 
{
	// TODO: Add your control notification handler code here
	if(m_pCurrentPolicy) 
	{
		if(!m_bReadOnly && g_bPolicyModifyed) 
		{
			m_pCurrentPolicy->Save(m_strCurrentPathName.GetBuffer(0));
			m_strCurrentPathName.ReleaseBuffer();
		}
	}
	g_bPolicyModifyed = false;
}

void CPolicyDlg::OnDblclkTreePolicy(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	OnButtonStopTrigger();
	
	CWnd *pWnd = GetDlgItem(IDC_STATIC_POLICYNAME);
	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE_POLICY);
	HTREEITEM itemp = pCtrl->GetSelectedItem();
	CString fileName = pCtrl->GetItemText(itemp);
	if(-1==fileName.Find(".ply")) return;
	fileName = "PolicyData\\"+fileName;
	if(m_pCurrentPolicy) 
	{
		if(m_strCurrentPathName==fileName) return;
		
		if(!m_bReadOnly && g_bPolicyModifyed) 
		{
			if(IDYES==AfxMessageBox("策略数据已经改变，你需要存盘吗?",MB_YESNO|MB_ICONQUESTION))
			{
				m_pCurrentPolicy->Save(m_strCurrentPathName.GetBuffer(0));
				m_strCurrentPathName.ReleaseBuffer();
			}
		}
		m_pCurrentPolicy->Release();
		delete m_pCurrentPolicy;
	}
	
	g_bPolicyModifyed = false;
	if(FileIsReadOnly(AString(fileName)))
	{
		if(AfxMessageBox("文件没有做Check out,修改是无效的!你要继续打开吗",MB_YESNO|MB_ICONQUESTION)==IDNO)
		{
			m_pCurrentPolicy = NULL;
			return;
		}
		m_bReadOnly = true;
	}else
	{
		m_bReadOnly = false;
	}	
	

	m_pCurrentPolicy = new CPolicyData;
	if(!m_pCurrentPolicy->Load(fileName.GetBuffer(0)))
	{
		delete m_pCurrentPolicy;
		m_pCurrentPolicy = NULL;
		pWnd->SetWindowText("空文档");
		MessageBox("打开文件失败!");
		return;
	}

	CString strID;
	if(m_pCurrentPolicy) strID.Format("(ID=%d)",m_pCurrentPolicy->GetID());
	m_strCurrentPathName = fileName;
	pWnd->SetWindowText(m_strCurrentPathName + strID);
	FreshTriggerList();
	
	*pResult = 0;
}

void CPolicyDlg::OnCancel()
{
	OnButtonStopTrigger(); 
	if(m_pCurrentPolicy) 
	{
		if(!m_bReadOnly && g_bPolicyModifyed) 
		{
			if(IDYES==AfxMessageBox("策略数据已经改变，你需要存盘吗?",MB_YESNO|MB_ICONQUESTION))
			{
				m_pCurrentPolicy->Save(m_strCurrentPathName.GetBuffer(0));
				m_strCurrentPathName.ReleaseBuffer();
			}
		}
		m_pCurrentPolicy->Release();
		delete m_pCurrentPolicy;
	}
	CDialog::OnCancel();
}

void CPolicyDlg::OnDblclkListTrigger() 
{
	
	OnButtonStopTrigger();
	if(m_pCurrentPolicy==NULL || m_bStartTest ) return;
	int sel = m_listTrigger.GetCurSel();
	if(sel == -1) return;
	DWORD id = m_listTrigger.GetItemData(sel);
	for( int i = 0; i < m_pCurrentPolicy->GetTriggerPtrNum(); ++i)
	{
		CTriggerData *pData = m_pCurrentPolicy->GetTriggerPtr(i);
		if(id==pData->GetID())
		{
			CPolicyTriggerDlg dlg;
			dlg.m_pCurrentPolicy = m_pCurrentPolicy;
			dlg.m_bModifyed = true;
			dlg.m_pTriggerData = pData;
			if(IDOK==dlg.DoModal())
			{
				g_bPolicyModifyed |= dlg.m_bIsChanged;
				FreshTriggerList();
			}
			break;
		}
	}
	
	//上面操作可能产生多余的触发器，所以必须做清理操作
	DelRedundancy();
}

bool CPolicyDlg::TraverselTree( void *pTree , CTriggerData *pTrigger)
{
	CTriggerData::_s_tree_item * temp = (CTriggerData::_s_tree_item*)pTree;
	bool bRes = false;
	bool bRes2 = false;
	switch(temp->mConditon.iType) 
	{
	case CTriggerData::c_time_come:
		bRes = IsTimeCome(((C_TIME_COME*)temp->mConditon.pParam)->uID);
		break;
//	case CTriggerData::c_time_point:
//		bRes = IsTimePoint();
//		break;
	case CTriggerData::c_hp_less:
		bRes = HpLess(((C_HP_LESS*)temp->mConditon.pParam)->fPercent);
		break;
	case CTriggerData::c_random:
		bRes = Random(((C_RANDOM*)temp->mConditon.pParam)->fProbability);
		break;
	case CTriggerData::c_and:
		if(temp->pLeft) bRes = TraverselTree(temp->pLeft,pTrigger);
		if(temp->pRight) bRes2= TraverselTree(temp->pRight,pTrigger);
		return bRes && bRes2;
	case CTriggerData::c_not:
		if(temp->pRight) bRes = TraverselTree(temp->pRight,pTrigger);
		return !bRes;
	case CTriggerData::c_or:
		if(temp->pLeft) bRes = TraverselTree(temp->pLeft,pTrigger);
		if(temp->pRight) bRes2= TraverselTree(temp->pRight,pTrigger);
		return bRes || bRes2;
		break;
	case CTriggerData::c_kill_player:
		return KillPlayer();
	case CTriggerData::c_died:
		return Died();
	case CTriggerData::c_start_attack:
		return StartAttack();
		break;
	case CTriggerData::c_reach_end:
		return ReachEnd();
	case CTriggerData::c_at_history_stage:
		return AtHistoryStage();
	case CTriggerData::c_stop_fight:
		return StopFight();
	case CTriggerData::c_reach_end_2:
		return ReachEnd2();
	case CTriggerData::c_has_filter:
		return HasFilter();
	default:
		return bRes;
	}
	return bRes;
}

//模拟函数
bool CPolicyDlg::IsTimeCome(unsigned int iId)
{
	for( size_t i = 0; i < listTimer.size(); ++i)
	{
		if(iId == listTimer[i].id)
		{
			if(listTimer[i].counter!=0)
			{
				if(listTimer[i].skipCounter>=listTimer[i].counter)
				{
					m_bTriggerDisable = true;
					return false;
				}
				if(listTimer[i].last_time > listTimer[i].period)
				{
					listTimer[i].skipCounter++;
					listTimer[i].last_time = 0;
					return true;
				}
			}else
			{
				if(listTimer[i].last_time > listTimer[i].period)
				{
					listTimer[i].last_time = 0;
					return true;
				}
			}
		}
	}
	
	return false;
}

bool CPolicyDlg::IsTimePoint()
{
	return (bool)m_bTimePoint;
}	

bool CPolicyDlg::HpLess( float fLess)
{
	UpdateData(true);
	if(m_fHp < fLess) 
	{
		m_bTriggerDisable = true;
		return true;
	}
	else return false;
}

bool CPolicyDlg::KillPlayer()
{
	UpdateData(true);
	return (bool)m_bSkipKillPlayer;
}

bool CPolicyDlg::Died()
{
	UpdateData(true);
	return (bool)m_bSkipDied;
}

bool CPolicyDlg::Random(float fProbability)
{
	UpdateData(true);
	return (bool)m_bSkipRandom;
}

bool CPolicyDlg::StartAttack()
{
	UpdateData(true);
	return (bool)m_bSkipStartAttack;
}

bool CPolicyDlg::ReachEnd()
{
	UpdateData(true);
	return m_bSkipReachEnd;
}

bool CPolicyDlg::ReachEnd2()
{
	UpdateData(true);
	return m_bSkipReachEnd2;
}

bool CPolicyDlg::AtHistoryStage()
{
	UpdateData(true);
	return m_iHistoryStage == 1;
}

bool CPolicyDlg::StopFight()
{
	UpdateData(true);
	return (bool)m_bSkipStopFight;
}

bool CPolicyDlg::HasFilter()
{
	UpdateData(true);
	return m_bHasFilter;
}

void CPolicyDlg::OnButtonRunTrigger() 
{
	// TODO: Add your control notification handler code here
	CWnd *pWnd = NULL;
	if(m_pCurrentPolicy && !m_bStartTest)
	{
		m_bStartTest = true;
		m_editRunLog.SetSel(0,-1);
		m_editRunLog.ReplaceSel("");
		AddLogMsg("测试运行当前策略已经开始 ... ");
		CWnd *pWnd = GetDlgItem(IDC_BUTTON_RUN_TRIGGER);
		if(pWnd) pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_BUTTON_STOP_TRIGGER);
		if(pWnd) pWnd->EnableWindow(true);
		listGriggerStatus.clear();
		//取得出发触发器的状态保存

		for( int i = 0; i < m_pCurrentPolicy->GetTriggerPtrNum(); ++i)
		{
			CTriggerData *pTrigger = m_pCurrentPolicy->GetTriggerPtr(i);
			TRIGGER_STATUS status;
			status.bActive = pTrigger->IsActive();
			status.id = pTrigger->GetID();
			status.bRun = false;
			listGriggerStatus.push_back(status);
		}
	}
}

void CPolicyDlg::OnButtonStopTrigger() 
{
	// TODO: Add your control notification handler code here
	if(m_pCurrentPolicy && m_bStartTest)
	{
		m_bStartTest = false;
		listTimer.clear();
		CWnd *pWnd = GetDlgItem(IDC_BUTTON_RUN_TRIGGER);
		if(pWnd) pWnd->EnableWindow(true);
		pWnd = GetDlgItem(IDC_BUTTON_STOP_TRIGGER);
		if(pWnd) pWnd->EnableWindow(false);
	}
}

bool CPolicyDlg::RunOperaion(void *pTriggerPtr, void *pOperation)
{
	CString msg;
	CTriggerData *pTrigger = (CTriggerData *)pTriggerPtr;
	CTriggerData::_s_operation *pTempOperation = (CTriggerData::_s_operation *)pOperation;
	void *pData = 0;
	switch(pTempOperation->iType) 
	{
	case CTriggerData::o_attact:
		pData = (O_ATTACK_TYPE *)pTempOperation->pParam;
		msg.Format("**操作[%s] [ID(%d)]",strOperationList[pTempOperation->iType],((O_ATTACK_TYPE*)pData)->uType);
		msg += GetTargetStr(&pTempOperation->mTarget);
		break;
	case CTriggerData::o_use_skill:
		pData = (O_USE_SKILL *)pTempOperation->pParam;
		msg.Format("**操作[%s] [技能(%d),等级(%d)]",strOperationList[pTempOperation->iType],((O_USE_SKILL*)pData)->uSkill,((O_USE_SKILL*)pData)->uLevel);
		msg += GetTargetStr(&pTempOperation->mTarget);
		break;
	case CTriggerData::o_talk:
		pData = (O_TALK_TEXT *)pTempOperation->pParam;
		{
			const O_TALK_TEXT *p = (const O_TALK_TEXT *)pData;
			msg.Format("**操作[%s] [内容(%s),附加数据(0x%x)]",strOperationList[pTempOperation->iType], glb_WideCharToMultiByte(p->szData,p->uSize/2), p->uAppendDataMask);
		}
		//msg += GetTargetStr(&pTempOperation->mTarget);
		break;
	case CTriggerData::o_run_trigger:
		pData = (O_RUN_TRIGGER *)pTempOperation->pParam;
		msg.Format("**操作[%s] [%s]",strOperationList[pTempOperation->iType],GetTriggerName(((O_RUN_TRIGGER*)pData)->uID));
		SetTriggerRunStatus(((O_ACTIVE_TRIGGER*)pData)->uID,true);
		//msg += GetTargetStr(&pTempOperation->mTarget);
		break;
	case CTriggerData::o_reset_hate_list:
		msg.Format("**操作[%s]",strOperationList[pTempOperation->iType]);
		break;
	case CTriggerData::o_stop_trigger:
		pData = (O_STOP_TRIGGER *)pTempOperation->pParam;
		msg.Format("**操作[%s] [%s]",strOperationList[pTempOperation->iType],GetTriggerName(((O_STOP_TRIGGER*)pData)->uID));
		SetTriggerActiveStatus(((O_ACTIVE_TRIGGER*)pData)->uID,false);
		break;
	case CTriggerData::o_active_trigger:
		pData = (O_ACTIVE_TRIGGER *)pTempOperation->pParam;
		msg.Format("**操作[%s] [%s]",strOperationList[pTempOperation->iType],GetTriggerName(((O_ACTIVE_TRIGGER*)pData)->uID));
		SetTriggerActiveStatus(((O_ACTIVE_TRIGGER*)pData)->uID,true);
		break;
	case CTriggerData::o_create_timer:
		pData = (O_CREATE_TIMER *)pTempOperation->pParam;
		msg.Format("**操作[%s] [ID(%d) 间隔(%d), 次数(%d)]",strOperationList[pTempOperation->iType],((O_CREATE_TIMER*)pData)->uID,((O_CREATE_TIMER*)pData)->uPeriod,((O_CREATE_TIMER*)pData)->uCounter);
		TIMER_TEST test;
		test.counter = 0;
		test.id = ((O_CREATE_TIMER*)pData)->uID;
		test.last_time = 0;
		test.skipCounter = 0;
		test.period = ((O_CREATE_TIMER*)pData)->uPeriod;
		test.counter = ((O_CREATE_TIMER*)pData)->uCounter;
		listTimer.push_back(test);
		//msg += GetTargetStr(&pTempOperation->mTarget);
		break;
	case CTriggerData::o_kill_timer:
		pData = (O_KILL_TIMER *)pTempOperation->pParam;
		msg.Format("**操作[%s] [ID(%d)]",strOperationList[pTempOperation->iType],((O_KILL_TIMER*)pData)->uID);
		//msg += GetTargetStr(&pTempOperation->mTarget);
		break;
	case CTriggerData::o_active_controller:
		pData = (O_ACTIVE_CONTROLLER *)pTempOperation->pParam;
		msg.Format("**操作[%s] [ID(%d)] [停止激活(%d)]",strOperationList[pTempOperation->iType],((O_ACTIVE_CONTROLLER*)pData)->uID,((O_ACTIVE_CONTROLLER*)pData)->bStop);
		//msg += GetTargetStr(&pTempOperation->mTarget);
		break;
	case CTriggerData::o_flee:
		msg.Format("**操作[%s]",strOperationList[pTempOperation->iType]);
		break;
	case CTriggerData::o_set_hate_to_first:
		msg.Format("**操作[%s]",strOperationList[pTempOperation->iType]);
		msg += GetTargetStr(&pTempOperation->mTarget);
		break;
	case CTriggerData::o_set_hate_to_last:
		msg.Format("**操作[%s]",strOperationList[pTempOperation->iType]);
		msg += GetTargetStr(&pTempOperation->mTarget);
		break;
	case CTriggerData::o_set_hate_fifty_percent:
		msg.Format("**操作[%s]",strOperationList[pTempOperation->iType]);
		msg += GetTargetStr(&pTempOperation->mTarget);
		break;
	case CTriggerData::o_set_global:
		pData = (O_SET_GLOBAL *)pTempOperation->pParam;
		msg.Format("**操作[%s] [id=%d, value=%d, Is value=%d]",strOperationList[pTempOperation->iType],((O_SET_GLOBAL *)pData)->iID,((O_SET_GLOBAL *)pData)->iValue,((O_SET_GLOBAL *)pData)->bIsValue);
		break;
	case CTriggerData::o_revise_global:
		pData = (O_REVISE_GLOBAL *)pTempOperation->pParam;
		msg.Format("**操作[%s] [id=%d, value=%d]",strOperationList[pTempOperation->iType],((O_REVISE_GLOBAL *)pData)->iID,((O_REVISE_GLOBAL *)pData)->iValue);
		break;
	case CTriggerData::o_summon_monster:
		pData = (O_SUMMON_MONSTER *)pTempOperation->pParam;
		msg.Format("**操作[%s] [%d,%d,%d,%d,%d,%d]",strOperationList[pTempOperation->iType],((O_SUMMON_MONSTER *)pData)->iDispearCondition,((O_SUMMON_MONSTER *)pData)->iMonsterID,((O_SUMMON_MONSTER *)pData)->iRange,((O_SUMMON_MONSTER *)pData)->iLife,((O_SUMMON_MONSTER *)pData)->iPathID,((O_SUMMON_MONSTER *)pData)->iMonsterNum);
		break;
	case CTriggerData::o_walk_along:
		pData = (O_WALK_ALONG *)pTempOperation->pParam;
		msg.Format("**操作[%s] [%d, %d, %d, %d]",strOperationList[pTempOperation->iType],((O_WALK_ALONG *)pData)->iWorldID,((O_WALK_ALONG *)pData)->iPathID,((O_WALK_ALONG *)pData)->iPatrolType,((O_WALK_ALONG *)pData)->iSpeedType);
		break;	
	case CTriggerData::o_play_action:
		pData = (O_PLAY_ACTION *)pTempOperation->pParam;
		msg.Format("**操作[%s] [%s, %d, %d, %d]",strOperationList[pTempOperation->iType],((O_PLAY_ACTION *)pData)->szActionName,((O_PLAY_ACTION *)pData)->iLoopCount,((O_PLAY_ACTION *)pData)->iInterval,((O_PLAY_ACTION *)pData)->iPlayTime);
		break;	
	case CTriggerData::o_revise_history:
		pData = (O_REVISE_HISTORY *)pTempOperation->pParam;
		msg.Format("**操作[%s] [id=%d, value=%d]",strOperationList[pTempOperation->iType],((O_REVISE_HISTORY *)pData)->iID,((O_REVISE_HISTORY *)pData)->iValue);
		break;
	case CTriggerData::o_set_history:
		pData = (O_SET_HISTORY *)pTempOperation->pParam;
		msg.Format("**操作[%s] [id=%d, value=%d, Is value=%d]",strOperationList[pTempOperation->iType],((O_SET_HISTORY *)pData)->iID,((O_SET_HISTORY *)pData)->iValue,((O_SET_HISTORY *)pData)->bIsHistoryValue);
		break;
	case CTriggerData::o_deliver_faction_pvp_points:
		pData = (O_DELIVER_FACTION_PVP_POINTS *)pTempOperation->pParam;
		msg.Format("**操作[%s] [ID(%d)]",strOperationList[pTempOperation->iType],((O_DELIVER_FACTION_PVP_POINTS*)pData)->uType);
		msg += GetTargetStr(&pTempOperation->mTarget);
		break;
	case CTriggerData::o_calc_var:
		pData = (O_CALC_VAR *)pTempOperation->pParam;
		{
			const O_CALC_VAR *p = (const O_CALC_VAR *)pData;
			msg.Format("**操作[%s] [%s(%d)=%s(%d)%s%s(%d)]",strOperationList[pTempOperation->iType],
				GetVarTypeName(p->iDstType), p->iDst,
				GetVarTypeName(p->iSrc1Type), p->iSrc1,
				GetOperatorName(p->iOp),
				GetVarTypeName(p->iSrc2Type), p->iSrc2);
		}
		break;
	case CTriggerData::o_summon_monster_2:
		pData = (O_SUMMON_MONSTER_2 *)pTempOperation->pParam;
		{
			const O_SUMMON_MONSTER_2 *p = (const O_SUMMON_MONSTER_2 *)pData;
			msg.Format("**操作[%s] [%d,%s(%d),%d,%d,%s(%d),%s(%d)]",strOperationList[pTempOperation->iType],
				p->iDispearCondition,
				GetVarTypeName(p->iMonsterIDType), p->iMonsterID,
				p->iRange,p->iLife,
				GetVarTypeName(p->iPathIDType), p->iPathID,
				GetVarTypeName(p->iMonsterNumType), p->iMonsterNum);
		}
		break;
	case CTriggerData::o_walk_along_2:
		pData = (O_WALK_ALONG_2 *)pTempOperation->pParam;
		{
			const O_WALK_ALONG_2 *p = (const O_WALK_ALONG_2 *)pData;
			msg.Format("**操作[%s] [%d, %s(%d), %s, %s]",strOperationList[pTempOperation->iType],
				p->iWorldID,
				GetVarTypeName(p->iPathIDType), p->iPathID,
				GetPatrolType(p->iPatrolType),
				GetPatrolSpeedType(p->iSpeedType));
		}
		break;
	case CTriggerData::o_use_skill_2:
		pData = (O_USE_SKILL_2 *)pTempOperation->pParam;
		{
			const O_USE_SKILL_2 *p = (const O_USE_SKILL_2 *)pData;
			msg.Format("**操作[%s] [技能 %s(%d),等级 %s(%d)]",strOperationList[pTempOperation->iType],
				GetVarTypeName(p->uSkillType), p->uSkill,
				GetVarTypeName(p->uLevelType), p->uLevel);
		}
		msg += GetTargetStr(&pTempOperation->mTarget);
		break;
	case CTriggerData::o_active_controller_2:
		pData = (O_ACTIVE_CONTROLLER_2 *)pTempOperation->pParam;
		{
			const O_ACTIVE_CONTROLLER_2 *p = (const O_ACTIVE_CONTROLLER_2 *)pData;
			msg.Format("**操作[%s] [ID %s(%d)] [停止激活(%d)]",strOperationList[pTempOperation->iType],
				GetVarTypeName(p->uIDType), p->uID,
				p->bStop);
		}
		break;
	case CTriggerData::o_deliver_task:
		{
			const O_DELIVER_TASK *p = (const O_DELIVER_TASK *)pTempOperation->pParam;
			msg.Format("**操作[%s] [发放任务 %s(%d)]",strOperationList[pTempOperation->iType],
				GetVarTypeName(p->uIDType), p->uID);
		}
		break;
	case CTriggerData::o_summon_mine:
		{
			const O_SUMMON_MINE *p = (const O_SUMMON_MINE *)pTempOperation->pParam;
			msg.Format("**操作[%s] [%s(%d),%d,%s(%d),%s(%d)]",
				strOperationList[pTempOperation->iType],
				GetVarTypeName(p->iMineIDType), p->iMineID,
				p->iRange,
				GetVarTypeName(p->iLifeType), p->iLife,
				GetVarTypeName(p->iMineNumType), p->iMineNum);
		}
		break;
	case CTriggerData::o_summon_npc:
		{
			const O_SUMMON_NPC *p = (const O_SUMMON_NPC *)pTempOperation->pParam;
			msg.Format("**操作[%s] [%s(%d),%d,%s(%d),%s(%d),%s(%d)]",
				strOperationList[pTempOperation->iType],
				GetVarTypeName(p->iNPCIDType), p->iNPCID,
				p->iRange,
				GetVarTypeName(p->iLifeType), p->iLife,
				GetVarTypeName(p->iPathIDType), p->iPathID,
				GetVarTypeName(p->iNPCNumType), p->iNPCNum);
		}
		break;
	case CTriggerData::o_deliver_random_task_in_region:
		{
			const O_DELIVER_RANDOM_TASK_IN_REGION *p = (const O_DELIVER_RANDOM_TASK_IN_REGION *)pTempOperation->pParam;
			msg.Format("**操作[%s] [(配置表%d) (最小点[%f,%f,%f]) (最大点[%f,%f,%f])]",
				strOperationList[pTempOperation->iType],
				p->uID,
				p->zvMin.x, p->zvMin.y, p->zvMin.z,
				p->zvMax.x, p->zvMax.y, p->zvMax.z
				);
		}
		break;
	case CTriggerData::o_deliver_task_in_hate_list:
		{
			const O_DELIVER_TASK_IN_HATE_LIST *p = (const O_DELIVER_TASK_IN_HATE_LIST *)pTempOperation->pParam;
			msg.Format("**操作[%s] [发放任务 %s(%d), 距离%d米,玩家%d个]",strOperationList[pTempOperation->iType],
				GetVarTypeName(p->uIDType), p->uID, p->iRange, p->iPlayerNum);
		}
		break;
	case CTriggerData::o_skip_operation:
		msg.Format("**操作[%s]",strOperationList[pTempOperation->iType]);
		AddLogMsg((LPCSTR)msg);
		return false;
	default:
		break;
	}
	AddLogMsg((LPCSTR)msg);
	return true;
}

void CPolicyDlg::RunTrigger(unsigned int id)
{
	for( int i = 0; i < m_pCurrentPolicy->GetTriggerPtrNum(); ++i)
	{
		CTriggerData *pTrigger = m_pCurrentPolicy->GetTriggerPtr(i);
		if(id==pTrigger->GetID())
		{
			if(pTrigger->IsRun())
			{
				if(TraverselTree(pTrigger->GetConditonRoot(),pTrigger))
				{//已经触发
					CString msg;
					msg.Format("==运行触发器 [%s], 执行下列操作:", pTrigger->GetName() );
					AddLogMsg((LPCTSTR)msg);
					for( int k = 0; k < pTrigger->GetOperaionNum(); k++)
					{//执行操作
						RunOperaion(pTrigger,pTrigger->GetOperaion(k));
					}
				}
				break;
			}
		}
	}
}

void CPolicyDlg::AddLogMsg(const char *szMsg)
{
	CString strBuf;
	strBuf.Format(_T("%s\n"),szMsg);
	//	Select all text at first then to cancel selection. This is the way to
	//	ensure caret is at the end of text when new text is added !!
	m_editRunLog.SetSel(0, -1);
	m_editRunLog.SetSel(-1, -1);
	m_editRunLog.ReplaceSel(strBuf);
    SetFocus();
}

void CPolicyDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	int m = listTimer.size();
	for( int h = 0; h < m; ++h)
	{
		if(listTimer[h].skipCounter <= listTimer[h].counter)
			listTimer[h].last_time += 1;
	}

	CString msg;
	if(m_pCurrentPolicy && m_bStartTest)
	{
		for( int i = 0; i < m_pCurrentPolicy->GetTriggerPtrNum(); ++i)
		{
			m_bTriggerDisable = false;
			CTriggerData *pTrigger = m_pCurrentPolicy->GetTriggerPtr(i);
			if(!pTrigger->IsRun())
			{//模拟事件触发表
				if(GetTriggerActiveStatus(pTrigger->GetID()))
				{
					if(TraverselTree(pTrigger->GetConditonRoot(),pTrigger))
					{//已经触发
						if(m_bTriggerDisable) SetTriggerActiveStatus(pTrigger->GetID(),false);
						msg.Format("==运行触发器 [%s], 执行下列操作:", pTrigger->GetName() );
						AddLogMsg((LPCTSTR)msg);
						for( int k = 0; k < pTrigger->GetOperaionNum(); k++)
						{//执行操作
							if(!RunOperaion(pTrigger,pTrigger->GetOperaion(k)))
								break;
						}
					}
				}
			}else
			{//模拟执行触发表、
				if(GetTriggerActiveStatus(pTrigger->GetID()) && GetTriggerRunStatus(pTrigger->GetID()))
				{
					if(TraverselTree(pTrigger->GetConditonRoot(),pTrigger))
					{//已经触发
						if(m_bTriggerDisable) SetTriggerActiveStatus(pTrigger->GetID(),false);
						msg.Format("==运行触发器 [%s], 执行下列操作:", pTrigger->GetName() );
						AddLogMsg((LPCTSTR)msg);
						for( int k = 0; k < pTrigger->GetOperaionNum(); k++)
						{//执行操作
							if(!RunOperaion(pTrigger,pTrigger->GetOperaion(k)))
								break;
						}
					}
				}
			}
		}
	}

	//重新设置事件标志
	m_bSkipKillPlayer = FALSE;
	m_bSkipRandom = FALSE;
	m_bSkipStartAttack = FALSE;
	m_bSkipDied = FALSE;
	m_bTimePoint = FALSE;
	m_bSkipReachEnd = false;
	m_bSkipReachEnd2 = false;
	m_iHistoryStage = -1;
	m_bSkipStopFight = false;
	m_bHasFilter = false;
	CDialog::OnTimer(nIDEvent);
}

void CPolicyDlg::SetTriggerActiveStatus( unsigned int id, bool bActive)
{
	int n = listGriggerStatus.size();
	for( int i = 0; i < n; ++i)
	{
		if(listGriggerStatus[i].id == id)
			listGriggerStatus[i].bActive = bActive;
	}
}

void CPolicyDlg::SetTriggerRunStatus( unsigned int id, bool bRun)
{
	int n = listGriggerStatus.size();
	for( int i = 0; i < n; ++i)
	{
		if(listGriggerStatus[i].id == id)
			listGriggerStatus[i].bRun = bRun;
	}
}


bool CPolicyDlg::GetTriggerActiveStatus( unsigned int id)
{
	int n = listGriggerStatus.size();
	for( int i = 0; i < n; ++i)
	{
		if(listGriggerStatus[i].id == id)
			return listGriggerStatus[i].bActive;
	}
	return false;
}

bool CPolicyDlg::GetTriggerRunStatus( unsigned int id)
{
	int n = listGriggerStatus.size();
	for( int i = 0; i < n; ++i)
	{
		if(listGriggerStatus[i].id == id)
			return listGriggerStatus[i].bRun;
	}
	return false;
}

CString CPolicyDlg::GetTriggerName( unsigned int id)
{
	if(m_pCurrentPolicy)
	{
		int n = m_pCurrentPolicy->GetTriggerPtrNum();
		for( int i = 0; i < n; ++i)
		{
			CTriggerData *pData = m_pCurrentPolicy->GetTriggerPtr(i);
			if(pData->GetID() == id) return pData->GetName();
		}
	}
	return "错误的触发器ID";
}

void CPolicyDlg::OnButtonEventRandom() 
{
	// TODO: Add your control notification handler code here
	m_bSkipRandom = TRUE;
}

void CPolicyDlg::OnButtonEventKillplayer() 
{
	// TODO: Add your control notification handler code here
	m_bSkipKillPlayer = TRUE;
}

void CPolicyDlg::OnButtonEventAttackstart() 
{
	// TODO: Add your control notification handler code here
	m_bSkipStartAttack = TRUE;
	
}

//下面三个函数删除冗余的执行触发器
void CPolicyDlg::DelRedundancy()
{
	abase::vector<unsigned> listId;
	int n = m_pCurrentPolicy->GetTriggerPtrNum();
	int i(0);
	for( i = 0; i < n; ++i)
	{
		CTriggerData *pData = m_pCurrentPolicy->GetTriggerPtr(i);
		if(pData->IsRun())
		{
			if(!TriggerIsUse(pData->GetID()))
			listId.push_back(pData->GetID());
		}
	}

	n = listId.size();
	for ( i = 0; i < n; ++i)
	{
		int idx = m_pCurrentPolicy->GetIndex(listId[i]);
		if(idx!=-1) m_pCurrentPolicy->DelTriggerPtr(idx);
	}
	listId.clear();
}

bool CPolicyDlg::TriggerIsUse( unsigned int id)
{
	int n = m_pCurrentPolicy->GetTriggerPtrNum();
	for( int i = 0; i < n; ++i)
	{
		CTriggerData *pData = m_pCurrentPolicy->GetTriggerPtr(i);
		if(!pData->IsRun())
		{
			if(TraceTrigger(pData,id)) return true;
		}
	}
	return false;
}

bool CPolicyDlg::TraceTrigger( CTriggerData *pTrigger,unsigned int id)
{
	if(pTrigger->GetID()==id) return true;
	int num = pTrigger->GetOperaionNum();
	for( int j = 0; j < num; ++j)
	{
		CTriggerData::_s_operation *pOperation = pTrigger->GetOperaion(j);				
		if(pOperation->iType==CTriggerData::o_run_trigger)
		{
			int idx = m_pCurrentPolicy->GetIndex(((O_RUN_TRIGGER*)pOperation->pParam)->uID);
			if(idx!=-1)
			{
				CTriggerData *pNext = m_pCurrentPolicy->GetTriggerPtr(idx);
				if(pNext) if(TraceTrigger(pNext,id)) return true;
			}
		}
	}
	return false;
}

void CPolicyDlg::OnButtonEventDied() 
{
	// TODO: Add your control notification handler code here
	m_bSkipDied = TRUE;
}

void CPolicyDlg::OnButtonEventTimePoint() 
{
	// TODO: Add your control notification handler code here
	m_bTimePoint = TRUE;
}

void CPolicyDlg::OnBtnFindPolicy() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CString strText;
	GetDlgItem(ID_EDIT_FIND_POLICY)->GetWindowText(strText);

	int nID = atoi(strText);
	if(nID <= 0)
	{
		AfxMessageBox("请输入查找ID！");
		return;
	}
	CString strName = g_PolicyIDMan.GetPathByID((unsigned long)nID);
	if(strName.IsEmpty())
	{
		CString strMSG;
		strMSG.Format("查找不到ID: %d 的策略!",nID);
		AfxMessageBox(strMSG);
		return;
	}
	int nIndex = strName.Find("\\");
	if(nIndex==-1)
		return;
	int nLen = strName.GetLength(); 
	strName = strName.Right(nLen-nIndex-1);

	CTreeCtrl* pCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREE_POLICY);
	HTREEITEM itemRoot  = pCtrl->GetRootItem();
	HTREEITEM itemChild = pCtrl->GetChildItem(itemRoot); 
	while (itemChild != NULL)
	{
		CString strTemp = pCtrl->GetItemText(itemChild);
		if(strName==strTemp)
		{
			pCtrl->SelectItem(itemChild);
			return;
		}

		itemChild = pCtrl->GetNextItem(itemChild, TVGN_NEXT);
	}

	CString strMSG;
	strMSG.Format("查找不到ID: %d 的策略!",nID);
	AfxMessageBox(strMSG);
}

void CPolicyDlg::OnButtonEventReachend() 
{
	// TODO: Add your control notification handler code here
	m_bSkipReachEnd = true;
}

void CPolicyDlg::OnButtonEventAtHistoryStage() 
{
	m_iHistoryStage = 1;	
}

void CPolicyDlg::OnButtonEventStopFight() 
{
	m_bSkipStopFight = true;	
}

void CPolicyDlg::OnButtonEventReachend2() 
{
	// TODO: Add your control notification handler code here
	m_bSkipReachEnd2 = true;
}

void CPolicyDlg::OnButtonEventHasFilter() 
{
	m_bHasFilter = true;	
}
