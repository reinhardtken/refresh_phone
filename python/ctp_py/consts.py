#!/usr/bin/env python
# -*- coding: utf-8 -*-


ERROR_CODE_OK = 0
ERROR_CODE_PULL_JSON_FAILED = 1
ERROR_CODE_PARSE_JSON_FAILED = 2
ERROR_CODE_DOWNLOAD_APK_FAILED = 3
ERROR_CODE_MD5_APK_FAILED = 4
ERROR_CODE_SAVE_JSON_FILE_FAILED = 5
ERROR_CODE_SAVE_APK_FILE_FAILED = 6

ERROR_CODE_REMOVE_JSON_FILE_FAILED = 7
ERROR_CODE_REMOVE_APK_DIR_FAILED = 8

ERROR_CODE_LOAD_LOCAL_APKLIST_FAILED = 9

ERROR_CODE_PYADB_SCAN_DEVICES_FAILED = 10
ERROR_CODE_PYADB_INSTALL_APK_FAILED = 11
ERROR_CODE_PYADB_OP_TIMEOUT_FAILED = 12
ERROR_CODE_PYADB_UNINSTALL_APK_FAILED = 13

ERROR_CODE_DOWNLOAD_APK_TIMEOUT_FAILED = 14
ERROR_CODE_LOGIN_FAILED = 100



ERROR_CODE_UNKNOWN = 100000

ERROR_CODE_STRING = {
  ERROR_CODE_OK: '成功',  # 0
  ERROR_CODE_PULL_JSON_FAILED: '拉取配置文件失败',
  ERROR_CODE_PARSE_JSON_FAILED: '解析配置文件失败',
  ERROR_CODE_DOWNLOAD_APK_FAILED: '下载apk文件失败',
  ERROR_CODE_MD5_APK_FAILED: '校验apk文件失败',
  ERROR_CODE_SAVE_JSON_FILE_FAILED: '保存配置文件失败',
  ERROR_CODE_SAVE_APK_FILE_FAILED: '保存apk文件失败',
  ERROR_CODE_REMOVE_JSON_FILE_FAILED: '删除配置文件失败',
  ERROR_CODE_REMOVE_APK_DIR_FAILED: '删除apk目录失败',
  ERROR_CODE_LOAD_LOCAL_APKLIST_FAILED: '加载本地配置文件失败',
ERROR_CODE_DOWNLOAD_APK_TIMEOUT_FAILED: '下载apk文件超时'
}

PACKAGE_INSTALL = 0  # 只安装
PACKAGE_REMOVE = 1  # 只卸载
PACKAGE_BOTH = 2  # 若存在，先卸载后安装



def error_string(code):
  if code in ERROR_CODE_STRING:
    info = ERROR_CODE_STRING[code]
  else:
    info = '未知错误'
  
  return info



COMMAND_CHECK_UPDATE = 'check_net_package_list'
COMMAND_GET_LOCAL_PACKAGELIST = 'get_local_package_list'
COMMAND_SCAN_DEVICES = 'pyadb.scan_devices'
COMMAND_INSTALL_APK = 'pyadb.install_apk'
COMMAND_GET_PACKAGE_LIST = 'pyadb.get_package_list'
COMMAND_REFRESH = 'pyadb.refresh'
COMMAND_AUTO_INSTALL = 'pyadb.auto_install'
COMMAND_NOTIFY_INSTALL_APK_DIGEST = 'pyadb.notify.install_digest'
COMMAND_TOTAL_AUTO_INSTALL = 'pyadb.total_auto_install'
COMMAND_INIT = 'pyadb.init'
COMMAND_VERIFY_CODE = 'pyadb.get_verify_code'
COMMAND_LOGIN = 'pyadb.login'
COMMAND_GET_IMEI = 'pyadb.imei'


#inner
COMMAND_INNER_START_CHECKOUT_TIMEOUT = 'inner.start_check_timeout'
COMMAND_INNER_STOP_CHECKOUT_TIMEOUT = 'inner.stop_check_timeout'
COMMAND_INNER_PACKAGE_LIST = 'inner.package_list'
COMMAND_INNER_GET_IMEI = 'inner.get_imei'


#net打点
COMMAND_NET_REPORT_DEVICE_INFO = 'net.report_device_info'
COMMAND_NET_REPORT_INSTALL_APK = 'net.report_install_apk'


INSTALL_TYPE_DELTE_FIRST = r'先删除\后安装'
INSTALL_TYPE_JUST_INSTALL = r'全新安装'



def AdbMessage2UserMessage(msg):
  #INSTALL_FAILED_CONFLICTING_PROVIDER
  #cannot stat 'C:\workspace\code\chromium24\src\build\Release\ctp_data\apk\com.youku.phone.apk': No such file or directory
  message = {
    u'INSTALL_FAILED_VERSION_DOWNGRADE': u'版本降级',
    u'INSTALL_FAILED_INSUFFICIENT_STORAGE': u'存储已满',
  }
  
  contain_message = {
    u'INSTALL_FAILED_USER_RESTRICTED': u'用户拒绝',
    u'No such file or directory': u'apk安装文件不存在'
  }
  
  if msg in message:
    return message[msg]
  
  for key in contain_message.keys():
    if key in msg:
      return contain_message[key]
    
  return u''



def LoginCode2ErrorString(code):
  message = {
    1000: u'请输入正确的手机号码',
    1001: u'验证码发送频繁',
    1002: u'短信发送失败',
    1003: u'验证码验证失败',
    1004: u'登录失败',
    1005: u'设备已存在',
    1006: u'设备不存在',
    9993: u'服务器异常',
    10001: u'无效标识，用户登录失效',
  }
  
  
  if code in message:
    return message[code]
  
  return u'未知错误'
  