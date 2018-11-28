# -*- coding: utf-8 -*-

import platform



# Operating system	Version number
# Windows 10	10.0*
# Windows Server 2016	10.0*
# Windows 8.1	6.3*
# Windows Server 2012 R2	6.3*
# Windows 8	6.2
# Windows Server 2012	6.2
# Windows 7	6.1

# Windows Server 2008 R2	6.1
# Windows Server 2008	6.0
# Windows Vista	6.0
# Windows Server 2003 R2	5.2
# Windows Server 2003	5.2
# Windows XP 64-Bit Edition	5.2
# Windows XP	5.1
# Windows 2000	5.0


# RuntimeError: this Windows version is too old (< Windows Vista); psutil 3.4.2 is
#  the latest version which supports Windows 2000, XP and 2003 server
def isXP():
  version = platform.version()
  if version.startswith('5'):
    return True
  
  return False



XP = isXP()