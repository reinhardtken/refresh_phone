#!/usr/bin/env python
# -*- coding: utf-8 -*-

import time
import datetime
import Queue
import os
import json
import sys
import traceback
import requests
import requests.exceptions

def Get(url, head=None):
  try:
    if head is None:
      headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36',
        #'X-atoken-Authorization': my_token.token.Get(),
      }
    else:
      headers = head
    
    try:
      timeout = (10, 180)
      response = requests.request("get", url, stream=True, data=None, headers=headers, timeout=timeout)
    except requests.exceptions.SSLError as e:
      response = requests.request("get", url, stream=True, data=None, headers=headers, verify=False, timeout=timeout)
    
    if response.status_code == 200:
      return (True, response.content)
    else:
      return (False, response.status_code)
    
  except requests.ConnectionError as e:
    return (False, e)
  except Exception as e:
    return (False, e)
  return (False, None)


def Post(url, data=None, head=None):
  try:
    if head is None:
      headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36',
        # 'X-atoken-Authorization': my_token.token.Get(),
      }
    else:
      headers = head
    
    try:
      timeout = (10, 180)
      response = requests.request("post", url, stream=True, data=data, headers=headers, timeout=timeout)
    except requests.exceptions.SSLError as e:
      response = requests.request("post", url, stream=True, data=data, headers=headers, verify=False, timeout=timeout)
    
    if response.status_code == 200:
      return (True, response.content)
    else:
      return (False, response.status_code)
  
  except requests.ConnectionError as e:
    return (False, e)
  except Exception as e:
    return (False, e)
  return (False, None)



def DownloadFile(url, filePath, callback):
  headers = {
    'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36',
  }
  try:
    timeout = (10, 180)
    try:
      response = requests.request("GET", url, stream=True, data=None, headers=headers, timeout=timeout)
    except requests.exceptions.SSLError as e:
      response = requests.request("GET", url, stream=True, data=None, headers=headers, verify=False, timeout=timeout)
  
    total_length = int(response.headers.get("Content-Length"))
    now = 0
    with open(filePath, 'wb') as f:
      for chunk in response.iter_content(chunk_size=8096):
        if chunk:
          f.write(chunk)
          f.flush()
          now += len(chunk)
          callback(now, total_length)
      else:
        callback(total_length, total_length)
        return True,
  except requests.exceptions.Timeout as e:
    exstr = traceback.format_exc()
    print(exstr)
    return False, e
  except Exception as e:
    exstr = traceback.format_exc()
    print(exstr)
    return False, e
  return (False, None)