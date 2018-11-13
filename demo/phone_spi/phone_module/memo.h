//第一条命令：启动server
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



//20181106

cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\adb version"

cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\adb start-server"

cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\adb devices -l"

cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\adb -s aa1ee7d1 get-serialno"

cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\adb -s aa1ee7d1 root"

cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\adb -s aa1ee7d1 shell getevent -p" > C:\workspace\code\chromium24\src\build\Debug\adb\74020625

cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\adb -s aa1ee7d1 shell getprop ro.build.version.release"

cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\adb -s aa1ee7d1 shell "echo 8 > /proc/sys/kernel/printk""

cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\adb -s aa1ee7d1 shell "cat /proc/sys/kernel/printk""

cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\adb -s aa1ee7d1 shell pm list packages -f " > C:\workspace\code\chromium24\src\build\Debug\adb\74094015

cmd.exe /c "C:\workspace\code\chromium24\src\build\Debug\adb\adb -s aa1ee7d1 shell pm list packages -3 " > C:\workspace\code\chromium24\src\build\Debug\adb\74109906

https://github.com/google/python-adb/issues/93
使用py-adb遇到
usb1.USBErrorAccess: LIBUSB_ERROR_ACCESS [-3]

Normally, the adb server is started by an adb command, or by some process like an emulator. If you switch to python-adb entirely (and don't use emulators), then the adb server shouldn't be running anymore.

We can't be in charge of stopping adb servers, because if it's running, it's likely being used by another process, and we don't want to cause errors in other programs that become impossible to diagnose.

If you manage to stop the adb server and stop it from coming back, python-adb can be useful to you, but until then only one process can 'own' a USB device and if the adb server is running it's 'owning' all android USB devices.

从最后那句话看，此问题貌似现阶段无解

环境=====
libusb相关
py地址
https://pypi.org/simple/libusb1/
so地址
https://github.com/libusb/libusb/releases
pyadb地址
https://github.com/google/python-adb

adb start-server
adb kill-server

netstat -ano | findstr "5037"


device.ConnectDevice(
    rsa_keys=[signer])

    xxx操作
device.Close()

*/