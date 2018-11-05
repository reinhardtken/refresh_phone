# -*- coding: utf-8 -*-
#!/usr/bin/python
#Filename:copyfile.py
import os
import shutil
#=========================================
IN_DIR = r"K:\workspace\chromium\version24\src\ctp\python\ctp_py"
OUT_DIR = r"K:\workspace\chromium\version24\src\ctp\python\out"
EXT = '.pyc'
#=========================================
# 目录递归拷贝函数
def dir_copyTree(src, dst):
    names = os.listdir(src)
    # 目标文件夹不存在，则新建
    if not os.path.exists(dst):
        os.mkdir(dst)
    # 遍历源文件夹中的文件与文件夹
    for name in names:
        srcname = os.path.join(src, name)
        dstname = os.path.join(dst, name)
        try:
            # 是文件夹则递归调用本拷贝函数，否则直接拷贝文件
            if os.path.isdir(srcname):               
              dir_copyTree(srcname, dstname)
            else:
              if (os.path.splitext(srcname)[1] == EXT):
                if (not os.path.exists(dstname)
                    or ((os.path.exists(dstname))
                        and (os.path.getsize(dstname) != os.path.getsize(srcname)))):
                  print dstname
                  shutil.copy2(srcname, dst)
        except:
            error.traceback();
            raise

    



def DeleteNoUsed(path):
  for root,dirs,files in os.walk(path,True):
    for eachfile in files:
      pass
#=======================================
if __name__ == '__main__':
  dir_copyTree(IN_DIR, OUT_DIR)