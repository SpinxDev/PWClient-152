#include "StdAfx.h"
#include "Global.h"
#include "MainFrm.h"
#include "AudioEditorView.h"
#include "AudioEditor.h"
#include "AudioEditorDoc.h"
#include "Engine.h"
#include <io.h>
#include <AIniFile.h>

CONFIGS g_Configs;

CString		g_csLog;
HWND		g_hOutputBar = NULL;
static CCriticalSection g_ShowStatusCriticalSection;

void GF_InitConfigs()
{
	strcpy_s(g_Configs.szProjectName, "");
	strcpy_s(g_Configs.szAudioEditorPath, "Audio");
	strcpy_s(g_Configs.szProjectsPath, "Audio\\Projects");
	strcpy_s(g_Configs.szReverbsPath, "Audio\\Reverbs");
	strcpy_s(g_Configs.szPresetsPath, "Presets");
	strcpy_s(g_Configs.szEventPresetPath, "Presets\\Event");
	strcpy_s(g_Configs.szSoundDefPresetPath, "Presets\\SoundDef");
	strcpy_s(g_Configs.szEffectPresetPath, "Presets\\Effect");
	strcpy_s(g_Configs.szSoundInsPresetPath, "Presets\\SoundInstance");

	GetCurrentDirectory(MAX_PATH, g_Configs.szRootPath);
	strcat_s(g_Configs.szRootPath, "\\");
	sprintf_s(g_Configs.szINIFilePath, MAX_PATH, "%s\\AudioEditor.ini", g_Configs.szRootPath);
	strcpy_s(g_Configs.szProjectListFile, g_Configs.szProjectsPath);
	strcat_s(g_Configs.szProjectListFile, "\\ProjectList.txt");
	strcpy_s(g_Configs.szResServerPath, "\\\\wangpaielement\\PerfectWorld2_ElementEditor");

	g_Configs.v3DEventPos.Clear();
	g_Configs.v3DEventVel.Clear();
	g_Configs.v3DEventOrientation = A3DVECTOR3(0, 0, 1);

	g_Configs.vListenerPos.Clear();
	g_Configs.vListenerVel.Clear();
	g_Configs.vListenerForward = A3DVECTOR3(0, 0, 1);
	g_Configs.vListenerUp = A3DVECTOR3(0, 1, 0);

	g_Configs.iClassNum = 0;
	for (int i=0; i<MAX_GROUP_CLASS_NUM; ++i)
	{
		memset(g_Configs.szClassNames[i], 0, MAX_PATH);
	}
}

void GF_LoadConfigs()
{
	AIniFile iniFile;
	if(!iniFile.Open(g_Configs.szINIFilePath))
		return;
	//	Load configs from .ini file
	char szSect[MAX_PATH];

	AString strValue;
	//	General section
	strcpy(szSect, "General");
	strValue = iniFile.GetValueAsString(szSect, "Audio_Res_Path", g_Configs.szResServerPath);
	strcpy(g_Configs.szResServerPath, strValue);

	strcpy(szSect, "3D Event Prop");
	g_Configs.v3DEventPos.x = iniFile.GetValueAsFloat(szSect, "3D_Event_Pos_x", 0);
	g_Configs.v3DEventPos.y = iniFile.GetValueAsFloat(szSect, "3D_Event_Pos_y", 0);
	g_Configs.v3DEventPos.z = iniFile.GetValueAsFloat(szSect, "3D_Event_Pos_z", 0);

	g_Configs.v3DEventVel.x = iniFile.GetValueAsFloat(szSect, "3D_Event_Vel_x", 0);
	g_Configs.v3DEventVel.y = iniFile.GetValueAsFloat(szSect, "3D_Event_Vel_y", 0);
	g_Configs.v3DEventVel.z = iniFile.GetValueAsFloat(szSect, "3D_Event_Vel_z", 0);

	g_Configs.v3DEventOrientation.x = iniFile.GetValueAsFloat(szSect, "3D_Event_Ori_x", 0);
	g_Configs.v3DEventOrientation.y = iniFile.GetValueAsFloat(szSect, "3D_Event_Ori_y", 0);
	g_Configs.v3DEventOrientation.z = iniFile.GetValueAsFloat(szSect, "3D_Event_Ori_z", 1);

	strcpy(szSect, "Listener Prop");
	g_Configs.vListenerPos.x = iniFile.GetValueAsFloat(szSect, "Listener_Pos_x", 0);
	g_Configs.vListenerPos.y = iniFile.GetValueAsFloat(szSect, "Listener_Pos_y", 0);
	g_Configs.vListenerPos.z = iniFile.GetValueAsFloat(szSect, "Listener_Pos_z", 0);

	g_Configs.vListenerVel.x = iniFile.GetValueAsFloat(szSect, "Listener_Vel_x", 0);
	g_Configs.vListenerVel.y = iniFile.GetValueAsFloat(szSect, "Listener_Vel_y", 0);
	g_Configs.vListenerVel.z = iniFile.GetValueAsFloat(szSect, "Listener_Vel_z", 0);

	g_Configs.vListenerForward.x = iniFile.GetValueAsFloat(szSect, "Listener_Forward_x", 0);
	g_Configs.vListenerForward.y = iniFile.GetValueAsFloat(szSect, "Listener_Forward_y", 0);
	g_Configs.vListenerForward.z = iniFile.GetValueAsFloat(szSect, "Listener_Forward_z", 1);

	g_Configs.vListenerUp.x = iniFile.GetValueAsFloat(szSect, "Listener_Up_x", 0);
	g_Configs.vListenerUp.y = iniFile.GetValueAsFloat(szSect, "Listener_Up_y", 1);
	g_Configs.vListenerUp.z = iniFile.GetValueAsFloat(szSect, "Listener_Up_z", 0);

	strValue = iniFile.GetValueAsString(szSect, "Export_Binary_Path", g_Configs.szExportBinaryPath);
	strcpy(g_Configs.szExportBinaryPath, strValue);

	strcpy(szSect, "Group Class");
	for (int i=0; i<MAX_GROUP_CLASS_NUM; ++i)
	{
		char szKey[128] = {0};
		sprintf_s(szKey, 128, "Class_%d", i);
		strValue = iniFile.GetValueAsString(szSect, szKey);
		strcpy(g_Configs.szClassNames[i], strValue);
	}	

	iniFile.Close();
}

void GF_SaveConfigs()
{
	AIniFile iniFile;	
	char szSect[MAX_PATH];

	//	General section
	strcpy(szSect, "General");
	iniFile.WriteStringValue(szSect, "Audio_Res_Path", g_Configs.szResServerPath);

	strcpy(szSect, "3D Event Prop");
	iniFile.WriteFloatValue(szSect, "3D_Event_Pos_x", g_Configs.v3DEventPos.x);
	iniFile.WriteFloatValue(szSect, "3D_Event_Pos_y", g_Configs.v3DEventPos.y);
	iniFile.WriteFloatValue(szSect, "3D_Event_Pos_z", g_Configs.v3DEventPos.z);

	iniFile.WriteFloatValue(szSect, "3D_Event_Vel_x", g_Configs.v3DEventVel.x);
	iniFile.WriteFloatValue(szSect, "3D_Event_Vel_y", g_Configs.v3DEventVel.y);
	iniFile.WriteFloatValue(szSect, "3D_Event_Vel_z", g_Configs.v3DEventVel.z);

	iniFile.WriteFloatValue(szSect, "3D_Event_Ori_x", g_Configs.v3DEventOrientation.x);
	iniFile.WriteFloatValue(szSect, "3D_Event_Ori_y", g_Configs.v3DEventOrientation.y);
	iniFile.WriteFloatValue(szSect, "3D_Event_Ori_z", g_Configs.v3DEventOrientation.z);

	strcpy(szSect, "Listener Prop");
	iniFile.WriteFloatValue(szSect, "Listener_Pos_x", g_Configs.vListenerPos.x);
	iniFile.WriteFloatValue(szSect, "Listener_Pos_y", g_Configs.vListenerPos.y);
	iniFile.WriteFloatValue(szSect, "Listener_Pos_z", g_Configs.vListenerPos.z);

	iniFile.WriteFloatValue(szSect, "Listener_Vel_x", g_Configs.vListenerVel.x);
	iniFile.WriteFloatValue(szSect, "Listener_Vel_y", g_Configs.vListenerVel.y);
	iniFile.WriteFloatValue(szSect, "Listener_Vel_z", g_Configs.vListenerVel.z);

	iniFile.WriteFloatValue(szSect, "Listener_Forward_x", g_Configs.vListenerForward.x);
	iniFile.WriteFloatValue(szSect, "Listener_Forward_y", g_Configs.vListenerForward.y);
	iniFile.WriteFloatValue(szSect, "Listener_Forward_z", g_Configs.vListenerForward.z);

	iniFile.WriteFloatValue(szSect, "Listener_Up_x", g_Configs.vListenerUp.x);
	iniFile.WriteFloatValue(szSect, "Listener_Up_y", g_Configs.vListenerUp.y);
	iniFile.WriteFloatValue(szSect, "Listener_Up_z", g_Configs.vListenerUp.z);

	iniFile.WriteStringValue(szSect, "Export_Binary_Path", g_Configs.szExportBinaryPath);

	strcpy(szSect, "Group Class");
	for (int i=0; i<MAX_GROUP_CLASS_NUM; ++i)
	{
		char szKey[128] = {0};
		sprintf_s(szKey, 128, "Class_%d", i);
		iniFile.WriteStringValue(szSect, szKey, g_Configs.szClassNames[i]);
	}

	if(!iniFile.Save(g_Configs.szINIFilePath))
		return;
	iniFile.Close();
}

bool GF_CreateDirectory(const char* szPath)
{
	SetCurrentDirectory(g_Configs.szRootPath);
	if(!PathIsDirectory(szPath) && !CreateDirectory(szPath, NULL))
		return false;
	return true;
}

CString GF_GetFolderPath(const char* szPath)
{
	CString str(szPath);
	return str.Left(str.ReverseFind('\\'));
}

bool GF_IsExist(const char* szPath)
{
	if(0 == _access(szPath, 0))
		return true;
	return false;
}

CMainFrame* GF_GetMainFrame()
{
	return (CMainFrame*)AfxGetMainWnd();
}

CAudioEditorView* GF_GetView()
{
	CView *pView = NULL ;
	POSITION pos1 ;
	CAudioEditorApp *pApp = NULL ;
	CAudioEditorDoc *pDoc  = NULL ;
	CSingleDocTemplate *pDocTemplate ;

	pApp = (CAudioEditorApp*)AfxGetApp() ;
	pos1 = pApp->GetFirstDocTemplatePosition() ;
	pDocTemplate = (CSingleDocTemplate*)pApp->GetNextDocTemplate(pos1) ;
	pos1 = pDocTemplate->GetFirstDocPosition() ;
	pDoc = (CAudioEditorDoc*)pDocTemplate->GetNextDoc(pos1) ;
	pos1 = pDoc->GetFirstViewPosition() ;

	pView = pDoc->GetNextView(pos1) ;

	return (CAudioEditorView*)pView;
}

bool GF_CheckName(const char* szName, HWND hWnd)
{
	CString strName(szName);
	if(0 == strName.GetLength())
	{
		MessageBox(hWnd, "不可为空", "警告", MB_ICONWARNING);
		return false;
	}
	if( -1 != strName.Find('\\') || 
		-1 != strName.Find('/') || 
		-1 != strName.Find(':') ||
		-1 != strName.Find('*') ||
		-1 != strName.Find('?') ||
		-1 != strName.Find('\"') ||
		-1 != strName.Find('<') ||
		-1 != strName.Find('>') ||
		-1 != strName.Find('|'))
	{
		MessageBox(hWnd, "不能包含下列字符之一：\n\\ / : * ? \" < > |", "警告", MB_ICONWARNING);
		return false;
	}
	return true;
}

bool GF_DeleteFolder(const char* szPath)
{
	return true;
	SHFILEOPSTRUCT FileOp;
	ZeroMemory(&FileOp, sizeof(FileOp));

	TCHAR ToBuf[MAX_PATH];
	TCHAR FromBuf[MAX_PATH];
	ZeroMemory(ToBuf, sizeof(ToBuf));
	ZeroMemory(FromBuf, sizeof(FromBuf));
	lstrcpy(FromBuf, szPath);

	FileOp.hwnd = NULL;
	FileOp.pFrom = FromBuf;
	FileOp.pTo = ToBuf;
	FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_ALLOWUNDO;
	FileOp.fAnyOperationsAborted = FALSE;
	FileOp.hNameMappings = NULL;
	FileOp.lpszProgressTitle = NULL;
	FileOp.wFunc = FO_DELETE;

	int ir = 0;
	ir = SHFileOperation(&FileOp);

	return ir == 0;
}

void GF_Log(LogLevel logLevel, const char* szInfo, ...)
{
	g_ShowStatusCriticalSection.Lock();
	char szTemp[1024] = {0};

	va_list ap;
	va_start(ap, szInfo);
	vsprintf(szTemp, szInfo, ap);
	va_end(ap);
	Engine::GetInstance().GetLog()->Write(logLevel, szTemp);

	g_csLog = Engine::GetInstance().GetLog()->GetLastLog();

	if (g_hOutputBar)
	{
		::SendMessage(g_hOutputBar, WM_LOG, 0, logLevel);
	}

	g_ShowStatusCriticalSection.Unlock();
}

void GF_VolumeTip(double val, CString& strTip)
{
	float r = GF_ConvertVolumeFromDB2Ratio((float)val)*100.0f;
	strTip.Format("%.1f%%", r);
}

void GF_PitchTip(double val, CString& strTip)
{
	float r = GF_ConvertPitchFromTone2Ratio((float)val)*100.0f;
	strTip.Format("%.1f%%", r);
}