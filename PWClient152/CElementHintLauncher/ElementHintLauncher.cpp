// ElementHintLauncher.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../CElementHintTool/MyLog.h"

#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <mmsystem.h>

HANDLE  g_hMutex = NULL;
const char *g_szMutex = "WMGJ_ElementHintLauncher";
const char *g_szLogFileName = "wmgj_HintLauncher.log";
const char *g_szChildProcessName = "wmgj_HintTool.exe";

bool initInstance();
void clearInstance();

int main(int argc, char* argv[])
{
	if (!initInstance())
		return -1;

	//	获取当前可执行程序所在文件夹
	char curPath[MAX_PATH] = {0};
	DWORD dwLength = ::GetModuleFileNameA(NULL, curPath, MAX_PATH);
	if (dwLength == 0)
	{
		printf("Unable to get exe file path!\n");
		return -1;
	}
	while (dwLength>0
		&& curPath[dwLength-1] != '\\'
		&& curPath[dwLength-1] != '/')
	{
		curPath[--dwLength] = '\0';
	}
	if (dwLength == 0)
	{
		printf("Unable to get exe file path!\n");
		return -2;
	}

	//	初始化日志文件
	char szLogFile[MAX_PATH] = {0};
	strcat(szLogFile, curPath);
	strcat(szLogFile, g_szLogFileName);
	if (!MyLog::GetInstance().Init(szLogFile, "wmgj Element Hint Tool Launcher log file create(or open)"))
	{
		printf("Unable to init log file!\n");
		return -3;
	}

	//	生成目标进程路径
	char szChildPath[MAX_PATH] = {0};
	strcat(szChildPath, curPath);
	strcat(szChildPath, g_szChildProcessName);
	
	//	设置错误处理模式为安静模式（以使子进程的创建不弹出对话框）
	SetErrorMode(SEM_FAILCRITICALERRORS);
	
	//	不断尝试创建目标进程
	while (true)
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );

		DWORD dwTime1 = timeGetTime();
		
		// 创建被守护进程
		if (!CreateProcess(szChildPath,
			"",
			NULL,			// Process handle not inheritable. 
			NULL,			// Thread handle not inheritable. 
			FALSE,			// Set handle inheritance to FALSE. 
			0,				// No creation flags. 
			NULL,			// Use parent's environment block. 
			NULL,			// Use parent's starting directory. 
			&si,			// Pointer to STARTUPINFO structure.
			&pi)			// Pointer to PROCESS_INFORMATION structure.
			)
		{
			//	创建失败，退出
			MyLog::GetInstance().Log("Create child process failed, launcher is now to exit!");
			break;
		}
		else
		{
			//	创建成功
			MyLog::GetInstance().Log("Create child process success!");
			
			//	等待子进程退出
			DWORD dwResult = WaitForSingleObject(pi.hProcess, INFINITE);
			if (dwResult == WAIT_FAILED)
			{
				//	等待子进程退出错误，可能子进程先于当前进程启动
				DWORD dwLastError = GetLastError();
				MyLog::GetInstance().Log("Wait for child process exit failed(LastError: %08x), launcher is now to exit!", dwLastError);
				break;
			}

			//	子进程退出

			//	查询进程退出代码
			DWORD dwExitCode(0);
			if (GetExitCodeProcess(pi.hProcess, &dwExitCode))
			{
				//	查询成功
				MyLog::GetInstance().Log("Child process Exited(ExitCode: %08x)!", dwExitCode);
			}
			else
			{
				//	查询退出代码失败
				MyLog::GetInstance().Log("Child process Exited(Unable to get Exitcode)!");
			}
			
			//	关闭子进程句柄
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);

			//	检查是否是创建不久即退出
			DWORD dwTime2 = timeGetTime();
			DWORD dwTimeDelta = (dwTime2<dwTime1) ? 0 : (dwTime2-dwTime1);
			if (dwTimeDelta < 10000)
			{
				//	是，则被守护进程有问题（如缺少DLL），不宜再进行尝试
				MyLog::GetInstance().Log("Child process exit too quickly(in %d ms), launcher is now to exit!", dwTimeDelta);
				break;
			}
		}

		//	睡眠后稍后再重新创建
		Sleep(1000);
	}

	MyLog::GetInstance().Log("Log is to be closed!");
	clearInstance();
	return 0;
}

bool initInstance()
{
	//	检查是否有多个实例运行
	g_hMutex = CreateMutexA(NULL, TRUE, g_szMutex);
	DWORD dwLastError = GetLastError();
	if (dwLastError == ERROR_ALREADY_EXISTS)
	{
		//	已经存在一个实例
		printf("An ElementHintLauncher instance already exists!");
		CloseHandle(g_hMutex);
		return false;
	}
	if (!g_hMutex)
	{
		printf("Create instance mutex failed!");
		return false;
	}
	return true;
}

void clearInstance()
{
	if (g_hMutex)
	{
		CloseHandle(g_hMutex);
		g_hMutex = NULL;
	}
}