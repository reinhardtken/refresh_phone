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

ERROR_CODE_UNKNOWN = 100000

ERROR_CODE_STRING = [
  '成功',  # 0
  '拉取配置文件失败',
  '解析配置文件失败',
  '下载apk文件失败',
  '校验apk文件失败',
  '保存配置文件失败',
  '保存apk文件失败',
  '删除配置文件失败',
  '删除apk目录失败',
  '加载本地配置文件失败'
]

PACKAGE_INSTALL = 0  # 只安装
PACKAGE_REMOVE = 1  # 只卸载
PACKAGE_BOTH = 2  # 若存在，先卸载后安装



def error_string(code):
  if len(ERROR_CODE_STRING) > code:
    info = ERROR_CODE_STRING[code]
  else:
    info = '未知错误'
  
  return info