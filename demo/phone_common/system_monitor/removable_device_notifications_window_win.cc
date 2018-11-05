// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "phone_common/system_monitor/removable_device_notifications_window_win.h"

#include <windows.h>
#include <dbt.h>
#include <fileapi.h>

#include "base/file_path.h"
#include "base/win/wrapped_window_proc.h"
#include "phone_common/system_monitor/media_storage_util.h"

using base::SystemMonitor;
using base::win::WrappedWindowProc;

namespace chrome {

namespace {

const char16 kWindowClassName[] = L"Chrome_RemovableDeviceNotificationWindow";

RemovableDeviceNotificationsWindowWin*
    g_removable_device_notifications_window_win = NULL;

}  // namespace

// static
RemovableDeviceNotificationsWindowWin*
RemovableDeviceNotificationsWindowWin::Create() {
  return new RemovableDeviceNotificationsWindowWin(new VolumeMountWatcherWin());
}

RemovableDeviceNotificationsWindowWin::
RemovableDeviceNotificationsWindowWin(
    VolumeMountWatcherWin* volume_mount_watcher)
    : window_class_(0),
      instance_(NULL),
      window_(NULL),
      volume_mount_watcher_(volume_mount_watcher) {
  DCHECK(volume_mount_watcher_);
  DCHECK(!g_removable_device_notifications_window_win);
  g_removable_device_notifications_window_win = this;
}

RemovableDeviceNotificationsWindowWin::
~RemovableDeviceNotificationsWindowWin() {
  if (window_)
    DestroyWindow(window_);

  if (window_class_)
    UnregisterClass(MAKEINTATOM(window_class_), instance_);

  DCHECK_EQ(this, g_removable_device_notifications_window_win);
  g_removable_device_notifications_window_win = NULL;
}

// static
RemovableDeviceNotificationsWindowWin*
RemovableDeviceNotificationsWindowWin::GetInstance() {
  DCHECK(g_removable_device_notifications_window_win);
  return g_removable_device_notifications_window_win;
}

void RemovableDeviceNotificationsWindowWin::Init() {
  volume_mount_watcher_->Init();

  WNDCLASSEX window_class;
  base::win::InitializeWindowClass(
      kWindowClassName,
      &WrappedWindowProc<RemovableDeviceNotificationsWindowWin::WndProcThunk>,
      0, 0, 0, NULL, NULL, NULL, NULL, NULL,
      &window_class);
  instance_ = window_class.hInstance;
  window_class_ = RegisterClassEx(&window_class);
  DCHECK(window_class_);

  window_ = CreateWindow(MAKEINTATOM(window_class_), 0, 0, 0, 0, 0, 0, 0, 0,
                         instance_, 0);
  SetWindowLongPtr(window_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

bool RemovableDeviceNotificationsWindowWin::GetDeviceInfoForPath(
    const FilePath& path,
    base::SystemMonitor::RemovableStorageInfo* device_info) {
  string16 location;
  std::string unique_id;
  string16 name;
  bool removable;
  if (!GetDeviceInfo(path, &location, &unique_id, &name, &removable))
    return false;

  // To compute the device id, the device type is needed.  For removable
  // devices, that requires knowing if there's a DCIM directory, which would
  // require bouncing over to the file thread.  Instead, just iterate the
  // devices in SystemMonitor.
  std::string device_id;
  if (removable) {
    std::vector<SystemMonitor::RemovableStorageInfo> attached_devices =
        SystemMonitor::Get()->GetAttachedRemovableStorage();
    bool found = false;
    for (size_t i = 0; i < attached_devices.size(); i++) {
      MediaStorageUtil::Type type;
      std::string id;
      MediaStorageUtil::CrackDeviceId(attached_devices[i].device_id, &type,
                                      &id);
      if (id == unique_id) {
        found = true;
        device_id = attached_devices[i].device_id;
        break;
      }
    }
    if (!found)
      return false;
  } else {
    device_id = MediaStorageUtil::MakeDeviceId(
        MediaStorageUtil::FIXED_MASS_STORAGE, unique_id);
  }

  if (device_info) {
    device_info->device_id = device_id;
    device_info->name = name;
    device_info->location = location;
  }
  return true;
}

// static
LRESULT CALLBACK RemovableDeviceNotificationsWindowWin::WndProcThunk(
    HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
  RemovableDeviceNotificationsWindowWin* msg_wnd =
      reinterpret_cast<RemovableDeviceNotificationsWindowWin*>(
          GetWindowLongPtr(hwnd, GWLP_USERDATA));
  if (msg_wnd)
    return msg_wnd->WndProc(hwnd, message, wparam, lparam);
  return ::DefWindowProc(hwnd, message, wparam, lparam);
}

LRESULT CALLBACK RemovableDeviceNotificationsWindowWin::WndProc(
    HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
  switch (message) {
    case WM_DEVICECHANGE:
      OnDeviceChange(static_cast<UINT>(wparam), lparam);
      return TRUE;
    default:
      break;
  }

  return ::DefWindowProc(hwnd, message, wparam, lparam);
}

bool RemovableDeviceNotificationsWindowWin::GetDeviceInfo(
    const FilePath& device_path, string16* device_location,
    std::string* unique_id, string16* name, bool* removable) {
  return volume_mount_watcher_->GetDeviceInfo(device_path, device_location,
                                              unique_id, name, removable);
}

void RemovableDeviceNotificationsWindowWin::OnDeviceChange(UINT event_type,
                                                           LPARAM data) {
  volume_mount_watcher_->OnWindowMessage(event_type, data);
}

}  // namespace chrome
