#!/usr/bin/env python
# -*- coding: utf-8 -*-

import util.utility


#http://note.youdao.com/noteshare?id=7019a6d986d313ce13da19ed423fef56
class Token(object):
  def __init__(self):
    self.__GToken = None
    self.__path = None

  def Init(self, path):
    self.__path = path
    tmp = util.utility.ReadJsonProp(path)
    if tmp is not None and len(tmp['token']) > 0:
      self.__GToken = tmp['token']
    
    
    
  def Set(self, new_token):
    self.__GToken = new_token
    tmp = {}
    if new_token is not None:
      tmp['token'] = new_token
    else:
      tmp['token'] = ''
    util.utility.WriteJsonFile(self.__path, tmp)
    
    
  def Get(self):
    return self.__GToken
  
  

token = Token()