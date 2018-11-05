﻿//第一条命令：启动server
//cmd.exe / c "C:\workspace\code\chromium24\src\build\Debug\adb\\/adb start-server"

/*
第二条命令：枚举设备
cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\\/adb devices -l"

第三条命令：获取设备的serialno
cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\adb -s aa1ee7d1 get-serialno"

第四条命令：判断是否root了
cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\adb -s aa1ee7d1 root"

第五条命令：
cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\adb -s aa1ee7d1 shell getevent -p" > adb\result.echo12345678

6 获取设备信息
cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\adb -s aa1ee7d1 shell getprop ro.build.version.release"
返回：
6.0.1


7
cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\adb -s aa1ee7d1 shell "echo 8 > /proc/sys/kernel/printk""
返回：
/system/bin/sh: can't create /proc/sys/kernel/printk: Permission denied

8
cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\adb -s aa1ee7d1 shell "cat /proc/sys/kernel/printk""
返回：
6	6	1	7


获取包名
cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\adb -s aa1ee7d1 shell pm list packages -f -U " > adb\result.echo12345678


*/