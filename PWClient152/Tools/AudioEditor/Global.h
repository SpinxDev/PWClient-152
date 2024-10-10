#pragma once

#include "FLog.h"
#include <A3DVector.h>

class CMainFrame;
class CAudioEditorView;

using AudioEngine::LogLevel;
using AudioEngine::LOG_ERROR;
using AudioEngine::LOG_WARNING;
using AudioEngine::LOG_NORMAL;

#define WM_LOG							(WM_USER+1564)
#define WM_UPDATE_PROP_LIST_REATTACH	(WM_USER+1565)
#define WM_KILLBCGPPROPSEL				(WM_USER+1566)

#define MAX_GROUP_CLASS_NUM	5

struct CONFIGS 
{
	char szProjectName[128];			// ������
	char szAudioEditorPath[MAX_PATH];	// ��Ƶ�༭����Ŀ¼����Audio
	char szProjectsPath[MAX_PATH];		// ���й�����Ŀ¼����Audio\Projects
	char szReverbsPath[MAX_PATH];		// ����Ŀ¼����Audio\Reverbs
	char szINIFilePath[MAX_PATH];		// �����ļ�AudioEditor.iniȫ·��
	char szRootPath[MAX_PATH];			// ��Ŀ¼������Ŀ¼
	char szCurProjectPath[MAX_PATH];	// ���̸�Ŀ¼��Projects\ProjectName
	char szProjectListFile[MAX_PATH];	// �����б��ļ�ProjectList.txtȫ·��
	char szResServerPath[MAX_PATH];		// ��Դ������·��
	char szPresetsPath[MAX_PATH];		// ��Ƶ�༭��Ԥ��Ŀ¼����Presets
	char szEventPresetPath[MAX_PATH];	// ��Ƶ�༭��Ԥ��Ŀ¼����Presets\Event
	char szSoundDefPresetPath[MAX_PATH];// ��Ƶ�༭��Ԥ��Ŀ¼����Presets\SoundDef
	char szEffectPresetPath[MAX_PATH];	// ��Ƶ�༭��Ԥ��Ŀ¼����Presets\Effect
	char szSoundInsPresetPath[MAX_PATH];// ��Ƶ�༭��Ԥ��Ŀ¼����Presets\SoundInstance
	char szExportBinaryPath[MAX_PATH];	// �����ƹ��̵���·��
	A3DVECTOR3 v3DEventPos;
	A3DVECTOR3 v3DEventVel;
	A3DVECTOR3 v3DEventOrientation;
	A3DVECTOR3 vListenerPos;
	A3DVECTOR3 vListenerVel;
	A3DVECTOR3 vListenerForward;
	A3DVECTOR3 vListenerUp;
	int iClassNum;
	char szClassNames[MAX_GROUP_CLASS_NUM][MAX_PATH];
};

extern CONFIGS g_Configs;

void GF_InitConfigs();
void GF_LoadConfigs();
void GF_SaveConfigs();
bool GF_IsExist(const char* szPath);
bool GF_CheckName(const char* szName, HWND hWnd = NULL);
bool GF_DeleteFolder(const char* szPath);
bool GF_MoveArchive(const char* szSrc, const char* szDest);
void GF_VolumeTip(double val, CString& strTip);
void GF_PitchTip(double val, CString& strTip);

CMainFrame* GF_GetMainFrame();
CAudioEditorView* GF_GetView();

bool GF_CreateDirectory(const char* szPath);
CString GF_GetFolderPath(const char* szPath);

void GF_Log(LogLevel logLevel, const char* szInfo, ...);