// ExceptionHandler.h: interface for the ExceptionHandler class.
//
//////////////////////////////////////////////////////////////////////

#pragma once



#include "base/values.h"
#include "base/file_path.h"
#include "base/logging.h"


#include "../include/ctp_define.h"

#define EXCEPTION_VERSION		_T("CTP_EXE")
#define CRASH_STRING_SIZE 256


#define EX_USER_INFO_STREAM_360  0x17830000
#pragma pack(push,1)
struct ExceptionUserData{
  char version[CRASH_STRING_SIZE];
};	
#pragma pack(pop)



class PHONE_COMMON_EXPORT ExceptionHandler {
public:

	ExceptionHandler();
	virtual ~ExceptionHandler();
	void SetExceptionHandler();

  static LONG WINAPI ExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo);

protected:
	static void InitExceptionInfo(PEXCEPTION_RECORD lpExceptionRecord);
	static BOOL GetLogicalAddress(PVOID addr, PTSTR szModule, DWORD len, DWORD& offset);
	//static LPBYTE CreateReportData(DWORD& dwSize);
	//static void AddFormData(LPBYTE lpData, DWORD& dwMaxSize, LPCSTR lpszName, LPCSTR lpszValue, BOOL bFile=FALSE);
	//static void SendReportToServer();
	static void CreateDumpFile(PEXCEPTION_POINTERS pExceptionInfo);
  static BOOL GetFileVersion(LPCTSTR strFile, LPTSTR pszVersion, int nVersionLen);
  //static BOOL GetIEVersion( LPTSTR lpszVersion, DWORD dwSize );
  //static BOOL AskforNeedDump();
  //static BOOL InitGetAnswerUrl(PEXCEPTION_POINTERS ExceptionInfo);
  //static std::string dumpfileMD5(FilePath filepath,std::string addstring);
  //static LPCTSTR GetIEPath();
  static LPCTSTR GetAppVersion();
  //static LPCTSTR GetFlashVersion();
  //static void SendLog();
  //static bool bFullMemoryDump_;
//  static std::vector<CrashData> InitStackInfo(PEXCEPTION_POINTERS ExceptionInfo);

protected:
	static TCHAR m_szName[MAX_PATH];		// 名称
	static TCHAR m_szVersion[MAX_PATH];		// 版本
	static TCHAR m_szFileVer[MAX_PATH];		// 文件版本
	static TCHAR m_szCmdLine[MAX_PATH];		// 命令行参数
	static TCHAR m_szModule[MAX_PATH];		// 故障模块名
	static TCHAR m_szModuleVer[MAX_PATH];	// 故障模块版本
	static TCHAR m_szCode[MAX_PATH];		// 异常代码
	static TCHAR m_szOffset[MAX_PATH];		// 异常偏移量
	static TCHAR m_szOSVersion[MAX_PATH];	// OS版本

  static TCHAR m_mid[MAX_PATH];         //mid


	static TCHAR m_szUserData[MAX_PATH];
	static TCHAR m_szIniFile[MAX_PATH];
	static TCHAR m_szDumpFile[MAX_PATH];

	static CRITICAL_SECTION m_CriticalReport;

};

ExceptionHandler PHONE_COMMON_EXPORT *exception_handler();
