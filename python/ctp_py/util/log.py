# -*- coding: utf-8 -*-

import logging



import utility
import config
#=======================================================
"""
import logging

logger= logging.getLogger( "module_name" )
logger_a = logger.getLogger( "module_name.function_a" )
logger_b = logger.getLogger( "module_name.function_b" )

def function_a( ... ):
    logger_a.debug( "a message" )

def functio_b( ... ):
    logger_b.debug( "another message" )

if __name__ == "__main__":
    logging.basicConfig( stream=sys.stderr, level=logging.DEBUG )
    logger_a.setLevel( logging.DEBUG )
    logger_b.setLevel( logging.WARN )
"""
#=======================================================
DEBUG = logging.DEBUG
INFO = logging.INFO
WARN = logging.WARNING
ERROR = logging.ERROR
FATAL = logging.CRITICAL

def Init(prop):
  fileDir = r'c:\ctp-py-log'

  if prop != None:
    fileDir = prop["logPath"]

  midDir = utility.TodayString() + '-' + 'py'
  fileName = utility.NowTimeString() + '.log'
  utility.CreateDir(fileDir + '/' + midDir)
  fileName = fileDir + '/' + midDir + '/' + fileName
  print("the log file process and name " + fileName)

  logging.basicConfig(filename = fileName, level = logging.DEBUG, format='%(asctime)s %(filename)s[line:%(lineno)d] %(levelname)s %(message)s', datefmt='%H:%M:%S')
  logging.info("the py version: " + config.Version())
  logging.debug("test debug output")
  logging.warning("test warn output %d", 123)
  logging.error("test error output  %d: %s" % (123, "456"))
  logging.critical("test critical output")

  #AddFilter("not allow debug")


__currentLogger = None

def GetLogger(name):
  __currentLogger = logging.getLogger(name)
  return __currentLogger


 #很多工具类模块，不关心当前的log是什么，只要用。所以
def GetCurrentLogger():
  if __currentLogger != None:
    return __currentLogger
  else:
    return GetLogger('Anonymous')


def SetLevel(l):
  logging.setLevel(l)
