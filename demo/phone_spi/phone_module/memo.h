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


ANDROID_ADB_SERVER_PORT

adb -


当先于360枪战了adb server的时候，如果尝试启动5037，360可能会挂
E:\workspace\chromium24\src\build\Debug\adb>adb -P 5037 devices -l
List of devices attached
error: protocol fault (couldn't read status): Invalid argument
error: protocol fault (couldn't read status): Invalid argument

当adb -P xxxx fork-server server这样的进程存在多个的时候。只有一个会真正拿到
连接权限，而其他的会等待真正拿到权限的那个gg。估计然后又会随机的启动一个吧。

从如下测试可知，猜测正确。因为在37启动之前41已结bind到了端口
E:\>cd E:\workspace\chromium24\src\build\Debug\adb

E:\workspace\chromium24\src\build\Debug\adb>adb devices -l
List of devices attached
* daemon not running. starting it now on port 5037 *
* daemon started successfully *


E:\workspace\chromium24\src\build\Debug\adb>adb devices -l
List of devices attached


E:\workspace\chromium24\src\build\Debug\adb>adb devices -l
List of devices attached


E:\workspace\chromium24\src\build\Debug\adb>adb -P 5040 devices -l
List of devices attached


E:\workspace\chromium24\src\build\Debug\adb>adb -P 5041 devices -l
List of devices attached
aa1ee7d1               device product:LeMax2_CN model:Le_X820 device:le_x2


E:\workspace\chromium24\src\build\Debug\adb>adb -P 5042 devices -l
List of devices attached


E:\workspace\chromium24\src\build\Debug\adb>


*/