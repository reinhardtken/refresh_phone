// ExceptionHandler.cpp: implementation of the ExceptionHandler class.
//
//////////////////////////////////////////////////////////////////////
#include "common_exception_handler.h"


#include <windows.h>
#include <dbghelp.h>
#include <shlobj.h>
#include <vector>
#include <tchar.h>
#include <shlwapi.h>



#include "base/utf_string_conversions.h"
#include "base/string_util.h"
#include "base/string_number_conversions.h"
#include "base/win/windows_version.h"
#include "base/base64.h"
#include "base/string_split.h"
#include "base/string_piece.h"
#include "base/stringprintf.h"
#include "base/file_util.h"
#include "base/md5.h"
#include "base/values.h"

#define SIZEOF(x) (sizeof(x)/sizeof(x[0]))


ExceptionHandler* exception_handler() {
  CR_DEFINE_STATIC_LOCAL(ExceptionHandler, eh, ());
  return &eh;
}

TCHAR ExceptionHandler::m_szName[MAX_PATH] = _T("");
TCHAR ExceptionHandler::m_szVersion[MAX_PATH] = _T("");
TCHAR ExceptionHandler::m_szFileVer[MAX_PATH] = _T("");
TCHAR ExceptionHandler::m_szCmdLine[MAX_PATH] = _T("");
TCHAR ExceptionHandler::m_szModule[MAX_PATH] = _T("");
TCHAR ExceptionHandler::m_szModuleVer[MAX_PATH] = _T("");
TCHAR ExceptionHandler::m_szCode[MAX_PATH] = _T("");
TCHAR ExceptionHandler::m_szOffset[MAX_PATH] = _T("");
TCHAR ExceptionHandler::m_szOSVersion[MAX_PATH] = _T("");

TCHAR ExceptionHandler::m_mid[MAX_PATH]=_T("");        
  

TCHAR ExceptionHandler::m_szUserData[MAX_PATH] = _T("");
TCHAR ExceptionHandler::m_szIniFile[MAX_PATH] = _T("");
TCHAR ExceptionHandler::m_szDumpFile[MAX_PATH] = _T("");


CRITICAL_SECTION ExceptionHandler::m_CriticalReport;

ExceptionUserData  g_ExceptionUserData = {0};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ExceptionHandler::ExceptionHandler()
{
}

ExceptionHandler::~ExceptionHandler()
{
}


LPCTSTR ExceptionHandler::GetAppVersion()
{
  static TCHAR version[MAX_PATH]={0};
  if(_T('\0')==version[0]) {
    TCHAR file[MAX_PATH]={0};
    GetModuleFileName(NULL, file, sizeof(file)/sizeof(TCHAR));
    GetFileVersion(file, version, sizeof(version)/sizeof(TCHAR));
  }
  return version;
}

void ExceptionHandler::InitExceptionInfo(PEXCEPTION_RECORD lpExceptionRecord)
{
	_sntprintf(m_szName, SIZEOF(m_szName)-1, _T("%s"), EXCEPTION_VERSION);
	_sntprintf(m_szVersion, SIZEOF(m_szVersion)-1, _T("%s"), GetAppVersion());
	_sntprintf(m_szFileVer, SIZEOF(m_szFileVer)-1, _T("%s"), GetAppVersion());
	_sntprintf(m_szCmdLine, SIZEOF(m_szCmdLine)-1, _T("%s"), GetCommandLine());
 
	_sntprintf(m_szCode, SIZEOF(m_szCode)-1, _T("0x%08x"), lpExceptionRecord->ExceptionCode);

	DWORD dwOffset = 0;
	TCHAR szBuffer[MAX_PATH]=_T("");
	if(GetLogicalAddress(lpExceptionRecord->ExceptionAddress, szBuffer, SIZEOF(szBuffer)-1, dwOffset)) {
    GetFileVersion(szBuffer, m_szModuleVer, SIZEOF(m_szModuleVer)-1);
		LPTSTR lpszName = PathFindFileName(szBuffer);
		if(NULL==lpszName)
			lpszName = szBuffer;		
		_sntprintf(m_szModule, SIZEOF(m_szModule)-1, _T("%s"), lpszName);
		_sntprintf(m_szOffset, SIZEOF(m_szOffset)-1, _T("0x%08x"), dwOffset);
    if(wcslen(m_szModuleVer) == 0) {
      _sntprintf(m_szModuleVer, SIZEOF(m_szModuleVer)-1, _T("%s"), L"empty_na");
    }
	}
  else {
    _sntprintf(m_szModule, SIZEOF(m_szModule)-1, _T("unknown") );
    _sntprintf(m_szModuleVer, SIZEOF(m_szModuleVer)-1, _T("%s"), L"empty_na");
    _sntprintf(m_szOffset, SIZEOF(m_szOffset)-1, _T("0x%08x"), (DWORD)lpExceptionRecord->ExceptionAddress);
  }

  OSVERSIONINFO osVersion;
	memset(&osVersion, 0, sizeof(osVersion));
	osVersion.dwOSVersionInfoSize = sizeof(osVersion);
	if(GetVersionEx(&osVersion))
		_sntprintf(m_szOSVersion, SIZEOF(m_szOSVersion)-1, _T("%d.%d.%d %s"), osVersion.dwMajorVersion, osVersion.dwMinorVersion, osVersion.dwBuildNumber, osVersion.szCSDVersion);



  char mid[128] = {0};
  //GenClientId3(mid, _countof(mid));
  if('\0' == mid[0]) {
    wcscpy_s(m_mid,L"empty_na");
  }
  else {
    wcscpy_s(m_mid,ASCIIToWide(mid).c_str());
  }

}

LONG WINAPI ExceptionHandler::ExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo)
{

  EnterCriticalSection(&m_CriticalReport);
	InitExceptionInfo(ExceptionInfo->ExceptionRecord);


  CreateDumpFile(ExceptionInfo);

  LeaveCriticalSection(&m_CriticalReport);
  TerminateProcess(GetCurrentProcess(), 0);
  return EXCEPTION_EXECUTE_HANDLER;


}

BOOL ExceptionHandler::GetLogicalAddress(PVOID addr, PTSTR szModule, DWORD len, DWORD& offset)
{
  MEMORY_BASIC_INFORMATION mbi={0};
  if(!VirtualQuery(addr, &mbi, sizeof(mbi)))
    return FALSE;
  DWORD hMod = (DWORD)mbi.AllocationBase;
  if( NULL == hMod ) {
		return FALSE;
	}
  if(!GetModuleFileName((HMODULE)hMod, szModule, len))
    return FALSE;

	offset = (DWORD)addr-hMod;
    return TRUE;
}

void ExceptionHandler::SetExceptionHandler()
{
  InitializeCriticalSection(&m_CriticalReport);
  SetUnhandledExceptionFilter(ExceptionFilter);
}


void ExceptionHandler::CreateDumpFile(PEXCEPTION_POINTERS pExceptionInfo)
{
	typedef BOOL (__stdcall *LPMiniDumpWriteDump)(HANDLE hProcess,
    DWORD ProcessId, HANDLE hFile, MINIDUMP_TYPE DumpType,
    PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
    PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
    PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

  HMODULE hDbgHelp = LoadLibrary(_T("dbghelp.dll"));
  if( hDbgHelp ) {
    LPMiniDumpWriteDump lpMiniDumpWriteDump =
      (LPMiniDumpWriteDump)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
    if( lpMiniDumpWriteDump ) {
      TCHAR szPath[MAX_PATH]=_T("");
      GetTempPath(SIZEOF(szPath)-1, szPath);
      GetTempFileName(szPath, _T("ctp_"), GetTickCount(), m_szDumpFile);

      HANDLE hFile = CreateFile(m_szDumpFile, GENERIC_WRITE,
        FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
      if( hFile!=INVALID_HANDLE_VALUE ) {
        MINIDUMP_EXCEPTION_INFORMATION exInfo;
        memset(&exInfo, 0, sizeof(exInfo));

        exInfo.ThreadId = ::GetCurrentThreadId();
        exInfo.ExceptionPointers = pExceptionInfo;
        exInfo.ClientPointers = FALSE;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        // [10/10/2011 ken]
        std::string version=WideToASCII(m_szFileVer);
        std::string crash_name=WideToASCII(m_szModule);
        std::string crash_version=WideToASCII(m_szModuleVer);
        std::string system_version=WideToASCII(m_szOSVersion);
        std::string exe_cmd=WideToASCII(m_szCmdLine);



        memcpy_s(g_ExceptionUserData.version,CRASH_STRING_SIZE-1,version.c_str()
          ,version.size()>CRASH_STRING_SIZE-1?CRASH_STRING_SIZE-1:version.size());
        g_ExceptionUserData.version[CRASH_STRING_SIZE-1]=0;
      

        MINIDUMP_USER_STREAM user_stream_array[1];
        user_stream_array[0].Type = EX_USER_INFO_STREAM_360;
        user_stream_array[0].BufferSize = sizeof(ExceptionUserData);
        user_stream_array[0].Buffer = &g_ExceptionUserData;

        MINIDUMP_USER_STREAM_INFORMATION user_streams;
        user_streams.UserStreamCount = 1;
        user_streams.UserStreamArray = user_stream_array;
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        lpMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile,
          (true ? MiniDumpWithFullMemory : 
            (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithHandleData)),
          &exInfo, &user_streams, NULL);
        CloseHandle(hFile);
      }
    }
    FreeLibrary(hDbgHelp);
  }
}

BOOL ExceptionHandler::GetFileVersion( LPCTSTR strFile, LPTSTR pszVersion, int nVersionLen )
{
	TCHAR  szVersionBuffer[8192] = {0};
	DWORD dwVerSize;
	DWORD dwHandle;

	dwVerSize = GetFileVersionInfoSize( strFile, &dwHandle );
	if( dwVerSize == 0 || dwVerSize > (sizeof(szVersionBuffer)-1) )
		return FALSE;

	if( GetFileVersionInfo( strFile,0,dwVerSize,szVersionBuffer) ) {
		VS_FIXEDFILEINFO * pInfo;
		unsigned int nInfoLen;

		if( VerQueryValue( szVersionBuffer, _T("\\"), (void**)&pInfo, &nInfoLen ) ) {
			_sntprintf( pszVersion, nVersionLen, _T("%d.%d.%d.%d"), 
				HIWORD( pInfo->dwFileVersionMS ), LOWORD( pInfo->dwFileVersionMS ), 
				HIWORD( pInfo->dwFileVersionLS ), LOWORD( pInfo->dwFileVersionLS ) );
			pszVersion[nVersionLen-1] = _T('\0');
			return TRUE;
		}
	}

	return FALSE;
}


