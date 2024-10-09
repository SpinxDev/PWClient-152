//#include "stdafx.h"
#include "EnumWin32Pro.h"
#include "infocollection.h"
using namespace Collector;

// 
// The EnumProcs function takes a pointer to a callback function
// that will be called once per process with the process filename 
// and process ID.
// 
// lpProc -- Address of callback routine.
// 
// lParam -- A user-defined LPARAM value to be passed to
//           the callback routine.
// 
// Callback function definition:
// BOOL CALLBACK Proc(DWORD dw, WORD w, LPCSTR lpstr, LPARAM lParam);
// 
BOOL WINAPI EnumProcs(PROCENUMPROC lpProc, LPARAM lParam) {

   HANDLE         hSnapShot = NULL;
   LPDWORD        lpdwPIDs  = NULL;
   PROCESSENTRY32 procentry;
   BOOL           bFlag;
   DWORD          dwSize;
   DWORD          dwSize2;
   DWORD          dwIndex;
   HMODULE        hMod;
   HANDLE         hProcess;
   char           szFileName[MAX_PATH];
   EnumInfoStruct sInfo;

   // If Windows NT 4.0
   if (Engine::GetInstance().HasToolHelp32() )
   {

      __try {

         // Get a handle to a Toolhelp snapshot of all processes.
         hSnapShot = Engine::GetInstance().CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
         if (hSnapShot == INVALID_HANDLE_VALUE) {
            return FALSE;
         }

         // Get the first process' information.
         procentry.dwSize = sizeof(PROCESSENTRY32);
         bFlag = Engine::GetInstance().Process32First(hSnapShot, &procentry);

         // While there are processes, keep looping.
         while (bFlag) {//
            
            // Call the enum func with the filename and ProcID.
            if (lpProc(procentry.th32ProcessID, 0,
                  procentry.szExeFile, lParam)) {

               // Did we just bump into an NTVDM? 
               if (_stricmp(procentry.szExeFile, "NTVDM.EXE") == 0) {

                  // Fill in some info for the 16-bit enum proc.
                  sInfo.dwPID = procentry.th32ProcessID;
                  sInfo.lpProc = lpProc;
                  sInfo.lParam = (DWORD) lParam;
                  sInfo.bEnd = FALSE;

                  // Enum the 16-bit stuff.
                  Engine::GetInstance().VDMEnumTaskWOWEx(procentry.th32ProcessID,
                     (TASKENUMPROCEX) Enum16, (LPARAM) &sInfo);

                  // Did our main enum func say quit?
                  if (sInfo.bEnd)
                     break;
               }

               procentry.dwSize = sizeof(PROCESSENTRY32);
               bFlag = Engine::GetInstance().Process32Next(hSnapShot, &procentry);

            } else
               bFlag = FALSE;
         }

      } __finally {
		  if( hSnapShot )
			CloseHandle(hSnapShot);
      }

   } else if (Engine::GetInstance().HasPSAPI() ) {

      __try {

         dwSize2 = 256 * sizeof(DWORD);
         do {

            if (lpdwPIDs) {
               HeapFree(GetProcessHeap(), 0, lpdwPIDs);
               dwSize2 *= 2;
            }

            lpdwPIDs = (LPDWORD) HeapAlloc(GetProcessHeap(), 0, 
                  dwSize2);
            if (lpdwPIDs == NULL)
               __leave;
            
            if (!Engine::GetInstance().EnumProcesses(lpdwPIDs, dwSize2, &dwSize))
               __leave;

         } while (dwSize == dwSize2);

         // How many ProcID's did we get?
         dwSize /= sizeof(DWORD);

         // Loop through each ProcID.
         for (dwIndex = 0; dwIndex < dwSize; dwIndex++) {

            szFileName[0] = 0;
            
            // Open the process (if we can... security does not
            // permit every process in the system to be opened).
            hProcess = OpenProcess(
                  PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                  FALSE, lpdwPIDs[dwIndex]);
            if (hProcess != NULL) {

               // Here we call EnumProcessModules to get only the
               // first module in the process. This will be the 
               // EXE module for which we will retrieve the name.
               if (Engine::GetInstance().EnumProcessModules(hProcess, &hMod,
                     sizeof(hMod), &dwSize2)) {

                  // Get the module name
                  if (!Engine::GetInstance().GetModuleBaseName(hProcess, hMod,
                        szFileName, sizeof(szFileName)))
                     szFileName[0] = 0;
               }
               CloseHandle(hProcess);
            }
            // Regardless of OpenProcess success or failure, we
            // still call the enum func with the ProcID.
            if (!lpProc(lpdwPIDs[dwIndex], 0, szFileName, lParam))
               break;

            // Did we just bump into an NTVDM?
            if (_stricmp(szFileName, "NTVDM.EXE") == 0) {

               // Fill in some info for the 16-bit enum proc.
               sInfo.dwPID = lpdwPIDs[dwIndex];
               sInfo.lpProc = lpProc;
               sInfo.lParam = (DWORD) lParam;
               sInfo.bEnd = FALSE;

               // Enum the 16-bit stuff.
               Engine::GetInstance().VDMEnumTaskWOWEx(lpdwPIDs[dwIndex],
                  (TASKENUMPROCEX) Enum16, (LPARAM) &sInfo);

               // Did our main enum func say quit?
               if (sInfo.bEnd)
                  break;
            }
         }

      } __finally {

         if (lpdwPIDs)
            HeapFree(GetProcessHeap(), 0, lpdwPIDs);
      }
   } else
      return FALSE;

   return TRUE;
}


BOOL WINAPI Enum16(DWORD dwThreadId, WORD hMod16, WORD hTask16,
      PSZ pszModName, PSZ pszFileName, LPARAM lpUserDefined) {

   BOOL bRet;

   EnumInfoStruct *psInfo = (EnumInfoStruct *)lpUserDefined;

   bRet = psInfo->lpProc(psInfo->dwPID, hTask16, pszFileName,
      psInfo->lParam);

   if (!bRet) 
      psInfo->bEnd = TRUE;

   return !bRet;
} 
