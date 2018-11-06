#pragma once

#include <vector>
#include <set>
#include <map>
#include <utility>
using std::map;
using std::set;
using std::vector;
//typedef std::wstring CString;
#include <atlstr.h>
#include "adb_define.h"

#define MSG_APPEND_ADB_CMD		(WM_USER + 0x4323)
#define UWM_RECONNECT_DEVICE	(WM_USER + 0x1234)


#define KERN_EMERG 0 /* system is unusable */
#define KERN_ALERT 1 /* action must be taken immediately */
#define KERN_CRIT 2 /* critical conditions */
#define KERN_ERR 3 /* error conditions */
#define KERN_WARNING 4 /* warning conditions */
#define KERN_NOTICE 5 /* normal but significant condition */
#define KERN_INFO 6 /* informational */
#define KERN_DEBUG 7 /* debug-level messages */
#define KERN_ALL 8 /* debug-level messages */



struct FILE_COLUMN_T {
	FILE_COLUMN_T(CString n, int i, int w) : name(n), index(i), width(w) {}
	CString name;
	int index;
	int width;
};

#define ASCEND_SUFFIX		0x01
#define ORDER_IS_ASCEND(o) (o & ASCEND_SUFFIX)
#define CMD_READ_BUFFER_SIZE	256
enum
{
	CMD_INFINITE = 0x01,
	CMD_WAIT = 0x02,
	CMD_JUST_RETURN = 0x04,
	CMD_READ_BYTES_COUNT  = 0x08,
	CMD_ABSOLUTE_INFINITE = 0x10,
	CMD_NOADD_SERIALNO_LINE = 0x20,
	CMD_READ_BACK_FILE = 0x40,
	CMD_DIR_ADB,
};

typedef struct 
{
	CString strCmd; 
	CString strReturn;
	int nType;		// 类型
	int nWaitMs;	// 等待的时间(ms)
	DWORD nReadBytesCount;	// 读取指定的字节数
	BOOL bRet;
}PARAM_T, *P_PARAM_T;

enum AndroidVersion{
	ANDROID_O_MR1 = 27,
	ANDROID_O = 26,
	ANDROID_N_MR1 = 25,
	ANDROID_N = 24,
	ANDROID_M = 23,
	ANDROID_LOLLIPOP_MR1 = 22,
	ANDROID_LOLLIPOP = 21,
	ANDROID_KITKAT_WATCH = 20,
	ANDROID_KITKAT = 19,
	ANDROID_JELLY_BEAN_MR2 = 18,
	ANDROID_JELLY_BEAN_MR1 = 17,
	ANDROID_JELLY_BEAN = 16,
	ANDROID_ICE_CREAM_SANDWICH_MR1 = 15,
	ANDROID_ICE_CREAM_SANDWICH = 14,
	ANDROID_HONEYCOMB_MR2 = 13,
	ANDROID_HONEYCOMB_MR1 = 12,
	ANDROID_HONEYCOMB = 11,
	ANDROID_GINGERBREAD_MR1 = 10,
	ANDROID_GINGERBREAD = 9,
	ANDROID_FROYO = 8,
	ANDROID_ECLAIR_MR1 = 7,
	ANDROID_ECLAIR_0_1 = 6,
	ANDROID_ECLAIR = 5,
	ANDROID_DONUT = 4,
	ANDROID_CUPCAKE = 3,
	ANDROID_BASE_1_1 = 2,
	ANDROID_BASE = 1,
};

struct ANDROID_OS_INFO {
	ANDROID_OS_INFO() : apiLevel(0) {}
	ANDROID_OS_INFO(CString ver, int api, CString code) : versionCode(code), platformVersion(ver), apiLevel(api) {}
	ANDROID_OS_INFO(CString release);
	CString platformVersion;
	int apiLevel;
	CString versionCode;

};

typedef struct  
{
	CString strName;
	int no;
	vector<CString> vecAttrs;
}INPUT_EVENT_DEV_T, *P_INPUT_EVENT_DEV_T;

struct ADB_DEVICE
{
	CString serial_no;
	CString state;
	CString model;
	CString product;
	CString device;

	BOOL IsValidDevice() const;

	void Reset() {
		serial_no = TEXT("");
		state = TEXT("");
		model = TEXT("");
		device = TEXT("");
		product = TEXT("");
	}

};
//=============================================================
//package


enum {
	LPPARAM_FILE = 0x01,
	LPPARAM_THIRDPARTY = 0x02,
	LPPARAM_DISABLED = 0x04,
	LPPARAM_ENABLED = 0x08,
	LPPARAM_SYSTEM = 0x10,
	LPPARAM_UID = 0x20,
	LPPARAM_INSTALLER = 0x40,
	LPPARAM_UNINSTALLED = 0x80,
};

enum {
	PKGORDER_DEFAULT = 0x00,
	PKGORDER_NAME = (0x10 << 0),
	PKGORDER_APP = (0x10 << 1),
	PKGORDER_PATH = (0x10 << 2),
	PKGORDER_UID = (0x10 << 3),
};
enum {
	LISTPKG_BEGIN,
	LISTPKG_PACKAGE,
	LISTPKG_UID,
	LISTPKG_END,
};


//=============================================================
class CAdbInterface
{
public:
	CAdbInterface(void);
	~CAdbInterface(void);

	enum {
		GD_SERIALNO = 0,
		GD_CONN_STATE,
		GD_PRODUCT,
		GD_MODEL,
		GD_DEVICE,
	};

public:
	void Init();
	void Deinit();
	void Reset();
	void Reboot();
	BOOL SendKeyEvent(int nEvent);
	BOOL SendEvent(int dev, int type, int code, int value, int cmdType = CMD_INFINITE);
	void SendText(CString strText);
	//void SendClick(CPoint point);
	//void SendSwipe(CPoint pntBegin, CPoint pntEnd);
	BOOL GetDevices();
	BOOL ToRoot();
	BOOL IsMultiDevConnected() const { return m_vecAdbDevices.size() > 1;}
	BOOL IsConnected() const { return m_bDevConnected; }
	BOOL IsDeviceOnWifi() const { return m_bWifiConnected; } 
	BOOL IsRooted() const { return m_bDevRooted; }
	BOOL IsUsbConnected() const { return (m_bDevConnected && !m_bWifiConnected); }
	CString GetSerialNo() const { return m_curDevice.serial_no; }
  std::wstring GetSerialNo2() {
    std::wstring tmp = m_curDevice.serial_no.GetBuffer();
    m_curDevice.serial_no.ReleaseBuffer();
    return tmp; 
  }
	CString GetModel() const { return m_curDevice.model; }
	BOOL GetScreenShot(CString strDstPath);
	BOOL StartActivity(CString strActivity);
	BOOL SetPrintkLevel(int level);
	int GetPrintkLevel();
	//BOOL GetResolution(CPoint &resolution);
	std::pair<bool, std::wstring> InstallApk(CString strApkPath);

	BOOL NeedAddSNo() const;

	void VivoInit();
	//CString GetAdbLastError() { 
	//	CString error = m_strError; 
	//	m_strError = TEXT("");
	//	return error;
	//}

	int GetPowerDev() const { return m_nPowerDevice; }
	int GetTpDev() const { return m_nFtInputDevNo;}

private:
	inline BOOL IsWhitespace(WCHAR single) {
		return ((single == TEXT(' ')) ||
			(single == TEXT('\n')) ||
			(single == TEXT('\r')) ||
			(single == TEXT('\t')));
	}
	void ClearTmps();
	void GetModalName();
	CString GetSNo();
	void GetInputDevices();
	void GetOsInfo();
	void AnalyseInputDevices(CString strDevices);
	//static BOOL m_bUseToolAdb;
public:
	static BOOL GetFileContents(CString path, CString &contents);
	static void GetProcessIDs(CString strExe);
	static void GetAdbProcessInfo();
	static void StopAllThread();
	static CString CleanString(CString str);
	static CString AddAdbPrefix(CString strCmd);
	static CString AddCmdPrefix(CString strCmd);
	static BOOL CreateAdbProcess(P_PARAM_T para);
	static BOOL _CreateAdbProcess(P_PARAM_T para);
	static BOOL CreateCmdWindow(P_PARAM_T para);	// 显示窗口
	static vector<DWORD> s_vecAdbProcessIds;
	static set<HANDLE> s_setAllSubThreads;
	static void AddSubThread(HANDLE hdl);
	static void RemoveSubThread(HANDLE hdl);
	static void StopAllSubThreads();
	static map<CString, ANDROID_OS_INFO> CAdbInterface::s_mapAndroidOsInfos;
	static void StaticInit();
	//static CString CMD_ECHO_FILE;
public:
	BOOL SelectDevice(int i);
	BOOL SelectDevice(ADB_DEVICE &dev);
	vector<ADB_DEVICE> m_vecAdbDevices;

	BOOL OsEqual(int os) const { return m_osAndroid.apiLevel == os; }
	BOOL OsHigher(int os) const { return m_osAndroid.apiLevel >= os; }
	BOOL OsLower(int os) const { return m_osAndroid.apiLevel <= os; }
	BOOL OsInRange(int beg, int end) const {
		return (m_osAndroid.apiLevel >= beg) && (m_osAndroid.apiLevel <= end);
	}

	static void GetPackages(int param, vector<ANDROID_PACKAGE_T> &packages);

private:
	ADB_DEVICE m_curDevice;

	//CString m_strError;
	BOOL m_bDevRooted;
	BOOL m_bDevConnected;
	BOOL m_bWifiConnected;
	int m_nFtInputDevNo;
	int m_nPowerDevice;
	ANDROID_OS_INFO m_osAndroid;
	static std::wstring adb_exe_path_;

private:
	static std::wstring ToParams(int nParam);
	static ANDROID_PACKAGE_T ParsePackageLine(CString line);
	

public:
	vector<INPUT_EVENT_DEV_T> m_vecInputDevices;

};

extern CAdbInterface* ADB;