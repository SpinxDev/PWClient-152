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
	char szProjectName[128];			// 工程名
	char szAudioEditorPath[MAX_PATH];	// 音频编辑器总目录，即Audio
	char szProjectsPath[MAX_PATH];		// 所有工程总目录，即Audio\Projects
	char szReverbsPath[MAX_PATH];		// 混响目录，即Audio\Reverbs
	char szINIFilePath[MAX_PATH];		// 配置文件AudioEditor.ini全路径
	char szRootPath[MAX_PATH];			// 根目录，工作目录
	char szCurProjectPath[MAX_PATH];	// 工程根目录，Projects\ProjectName
	char szProjectListFile[MAX_PATH];	// 工程列表文件ProjectList.txt全路径
	char szResServerPath[MAX_PATH];		// 资源服务器路径
	char szPresetsPath[MAX_PATH];		// 音频编辑器预设目录，即Presets
	char szEventPresetPath[MAX_PATH];	// 音频编辑器预设目录，即Presets\Event
	char szSoundDefPresetPath[MAX_PATH];// 音频编辑器预设目录，即Presets\SoundDef
	char szEffectPresetPath[MAX_PATH];	// 音频编辑器预设目录，即Presets\Effect
	char szSoundInsPresetPath[MAX_PATH];// 音频编辑器预设目录，即Presets\SoundInstance
	char szExportBinaryPath[MAX_PATH];	// 二进制工程导出路径
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