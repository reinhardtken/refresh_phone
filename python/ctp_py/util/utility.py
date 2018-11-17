import datetime
import time
import json
import uuid
import os
import os.path
import hashlib
#// The internal representation of Time uses FILETIME, whose epoch is 1601-01-01
#// 00:00:00 UTC.  ((1970-1601)*365+89)*24*60*60*1000*1000, where 89 is the
#// number of leap year days between 1601 and 1970: (1970-1601)/4 excluding
#// 1700, 1800, and 1900.
__kTimeTToMicrosecondsOffse = 11644473600000000

def UTCTime2DBTimeInMicroseconds(seconds):
  return seconds * 1000 * 1000 + __kTimeTToMicrosecondsOffse
  pass


def OneMintueInMicroseconds():
  return 60 * 1000 * 1000


def LargeNumber2String(number):
  return "%20.3f" % number
#====================================================

def IsSpecialType(type):
  if type == 'if' or type == 'ic' or type == 'ih':
    return True
	
  return False
  
  
__TYPE_LIST = ('if', 'rb', 'ru', 'fg', 'm', 'ic', 'ih')  

def GetTypeList():
  return list(__TYPE_LIST)

def TodayTimeInSeconds():
  today = datetime.date.today()
  t = time.mktime(today.timetuple())
  s = "%20.3f" % t
  #print('today and utc', today, s)
  return t


def TomorrowTimeInSeconds():
  today = datetime.date.today()
  tomorrow = today + datetime.timedelta(days=1)
  t = time.mktime(tomorrow.timetuple())
  s = "%20.3f" % t
  print('tomorrow and utc', tomorrow, t)
  return t

def YesterdayTimeInSeconds():
  today = datetime.date.today()
  yesterday = today - datetime.timedelta(days=1)
  t = time.mktime(yesterday.timetuple())
  s = "%20.3f" % t
  print('Yesterday and utc', yesterday, t)
  return t


#====================================================
def TodayTimeInMicroseconds():
  today = TodayTimeInSeconds()
  micro = UTCTime2DBTimeInMicroseconds(today)
  s = "%20.3f" % micro
  #print('today micro', s)
  return micro


def TomorrowTimeInMicroseconds():
  tomorrow = TomorrowTimeInSeconds()
  micro = UTCTime2DBTimeInMicroseconds(tomorrow)
  s = "%20.3f" % micro
  print('tomorrow micro', s)
  return micro

def YesterdayTimeInMicroseconds():
  yesterday = YesterdayTimeInSeconds()
  micro = UTCTime2DBTimeInMicroseconds(yesterday)
  s = "%20.3f" % micro
  print('yesterday micro', s)
  return micro


def OneDayInSeconds():
  return 3600 * 24


def ATimeInMicroseconds(years, months, days, hours=0, minutes=0, seconds=0):
  day = datetime.date(years, months, days)
  #print('orgin day ', day)
  t = time.mktime(day.timetuple())
  t += hours * 3600
  t += minutes * 60
  t += seconds
  micro = UTCTime2DBTimeInMicroseconds(t)
  s = "%20.3f" % micro
  #print('ATimeInMicroseconds', s)
  return micro


def TimeInMicroseconds2String(microSeconds):
  microSecondsFrom1970 = microSeconds - __kTimeTToMicrosecondsOffse
  microSecondsFrom1970 /= (1000 * 1000)
  local = time.localtime(microSecondsFrom1970)
  s = time.strftime('%Y-%m-%d %H:%M:%S', local)
  #print('TimeInMicroseconds2String ', s)
  return s

def TimeInMicroseconds2NumberString(microSeconds):
  s = "%20.3f" % microSeconds
  return s

def TodayString():
  today = datetime.date.today()
  return today.strftime("%Y-%m-%d")


def NowTimeString():
  now = datetime.datetime.now()
  return now.strftime("%H-%M-%S")

def TodayMonthStrig():
  today = datetime.date.today()
  return today.strftime("%Y%m")  

def SomeDayString(microSeconds):
  microSecondsFrom1970 = microSeconds - __kTimeTToMicrosecondsOffse
  microSecondsFrom1970 /= (1000 * 1000)
  local = time.localtime(microSecondsFrom1970)
  s = time.strftime('%Y-%m-%d', local)
  return s

#===============================================================
def ReadJsonProp(fileName):
  s = None
  try:
    f = file(fileName);
    s = json.load(f)
    f.close()
  except Exception as e:
    pass
  return s


def WriteJsonFile(fileName, content):
  try:
    content = json.dumps(content, sort_keys=True, indent=2)
    file = open(fileName, "w")
    file.write(content)
    file.close()
    return True
  except Exception as e:
    return False

def GetJsonString(content):
  return json.dumps(content, sort_keys=True, indent=2)


def GetMAC():
  return uuid.uuid1().hex[-12:]


def GetFileMD5(filename):
  if not os.path.isfile(filename):
    return
  myhash = hashlib.md5()
  f = open(filename, 'rb')
  while True:
    b = f.read(8096)
    if not b :
      break
    myhash.update(b)
  f.close()
  return myhash.hexdigest()


def CreateDir(dir):
  try:
    if not os.path.isdir(dir):
      os.makedirs(dir)
  except Exception as e:
    pass

def RemoveDir(dir):
  dir = dir.replace('\\', '/')
  if (os.path.isdir(dir)):
    for p in os.listdir(dir):
      RemoveDir(os.path.join(dir, p))
    if (os.path.exists(dir)):
      os.rmdir(dir)
  else:
    if (os.path.exists(dir)):
      os.remove(dir)



def GetPackageNameFromPath(apk):
  index = apk.rfind('\\')
  if index != -1:
    return apk[index + 1:]
  else:
    index = apk.rfind('/')
    if index != -1:
      return apk[index + 1:]

  return ''
#==============================================================
def SomeDayStartLine(type, microSeconds):
  microSecondsFrom1970 = microSeconds - __kTimeTToMicrosecondsOffse
  microSecondsFrom1970 /= (1000 * 1000)
  local = time.localtime(microSecondsFrom1970)
  #https://docs.python.org/2/library/time.html#time.struct_time
  return GenATimeStartLine(type, local[0], local[1], local[2])



def GenTodayStartLine(type):
  today = datetime.date.today()
  if IsSpecialType(type):
    return ATimeInMicroseconds(today.year, today.month, today.day, 9, 15)
  else:
    return ATimeInMicroseconds(today.year, today.month, today.day, 9)

def GenATimeStartLine(type, year, month, day):
  if IsSpecialType(type):
    return ATimeInMicroseconds(year, month, day, 9, 15)
  else:
    return ATimeInMicroseconds(year, month, day, 9)

def GenTodayOverLine(type):
  today = datetime.date.today()
  if IsSpecialType(type):
    return ATimeInMicroseconds(today.year, today.month, today.day, 15, 15)
  else:
    return ATimeInMicroseconds(today.year, today.month, today.day, 15)

def GenATimeOverLine(type, year, month, day):
  if IsSpecialType(type):
    return ATimeInMicroseconds(year, month, day, 15, 15)
  else:
    return ATimeInMicroseconds(year, month, day, 15)


def GenTodayRestStartOver(type):
  today = datetime.date.today()
  if IsSpecialType(type):
    restStart = ATimeInMicroseconds(today.year, today.month, today.day, 11, 30, 1)
    restOver = ATimeInMicroseconds(today.year, today.month, today.day, 12, 59, 59)
    return (restStart, restOver)
  else:
    restStart = ATimeInMicroseconds(today.year, today.month, today.day, 11, 30, 1)
    restOver = ATimeInMicroseconds(today.year, today.month, today.day, 12, 59, 59)
    return (restStart, restOver)
    pass



def GenATimeRestStartOver(type, year, month, day):
  if IsSpecialType(type):
    restStart = ATimeInMicroseconds(year, month, day, 11, 30, 1)
    restOver = ATimeInMicroseconds(year, month, day, 12, 59, 59)
    return (restStart, restOver)
  else:
    restStart = ATimeInMicroseconds(year, month, day, 11, 30, 1)
    restOver = ATimeInMicroseconds(year, month, day, 12, 59, 59)
    return (restStart, restOver)
    pass


def GenTodayInvalidateMQ():
  today = datetime.date.today()
  return GenATimeInvalidateMQ(today.year, today.month, today.day)
  pass

def GenATimeInvalidateMQ(year, month, day):
  utc = ATimeInMicroseconds(year, month, day, 16)
  utc_string = TimeInMicroseconds2String(utc)
  return [utc, utc_string, 0, 0, 0, 0, 0, 0]
  pass

def Last1S(v):
  return v - (1000 * 1000)

def Next1S(v):
  return v + (1000 * 1000)

def Next5S(v):
  return v + (1000 * 1000 * 5)

def Next1M(v):
  return v + (1000 * 1000 * 60)

def Next5M(v):
  return v + (1000 * 1000 * 60 * 5)

def Next15M(v):
  return v + (1000 * 1000 * 60 * 15)

def Next1H(v):
  return v + (1000 * 1000 * 60 * 60)

def Next1D(v):
  return v + (1000 * 1000 * 60 * 60 * 6)
  #return v + (1000 * 1000 * 60 * 60 * 24)

def Next1DSuccessive(v):
  return v + (1000 * 1000 * 60 * 60 * 24)

 #====================================
def StringTime2TimeWithFormat(data, format):
  return datetime.datetime.strptime(data, format) 


def StringTime2Time1(data):
  #2013-07-01 09:17:00
  data = data.strip()
  if data.find(':') == -1:
    data += ' 00:00:00'
  return StringTime2TimeWithFormat(data, '%Y-%m-%d %H:%M:%S')


def StringTime2NumberTime(data):
  t = StringTime2Time1(data)
  return ATimeInMicroseconds(t.year, t.month, t.day, t.hour, t.minute, t.second)



def SecondTimeStamp():
  now = datetime.datetime.now()
  timeStamp = now.hour * 60 * 60 + now.minute * 60 + now.second
  return timeStamp



def StringNow():
  now = datetime.datetime.now()
  return "%d/%02d/%02d %02d:%02d:%02d" % (now.year, now.month, now.day, now.hour, now.minute, now.second)




#Traversal(codeDir, self.ProcessOneFile, ".java")
def Traversal(base_dir, action, ext):
  if base_dir == None or action == None or ext == None:
    return 

 
  exts = ext.split("|")

  def EndsWith(name):
    for v in exts:
      if name.endswith(v):
        return True
    return False;



  for parent, dirnames, filenames in os.walk(base_dir):
    for filename in filenames:
      #absolute_filename = os.path.join(parent,filename)
      if ext != None and EndsWith(filename):
        action(parent, filename)


#=======================================================
if __name__ == '__main__':
  print('<=============(__main__ utility.py)==================>')
  #TodayTimeInMicroseconds()
  #TomorrowTimeInMicroseconds()
  #t = ATimeInMicroseconds(2014, 2, 25, 10, 10, 10)
  #TimeInMicroseconds2String(t)
  print(StringTime2Time1('2013-07-01 09:17:00'))
