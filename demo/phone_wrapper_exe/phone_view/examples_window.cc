// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "examples_window.h"

#include <string>

#include "base/memory/scoped_vector.h"
#include "base/utf_string_conversions.h"
#include "content/browser/notification_service_impl.h"
#include "content/public/common/content_switches.h"

#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/notification_observer.h"
#include "content/browser/notification_service_impl.h"

#include "ui/base/models/combobox_model.h"
#include "ui/base/ui_base_paths.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/combobox/combobox.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/tabbed_pane/tabbed_pane.h"


#include "grit/theme_resources.h"

#include "ui/views/focus/accelerator_handler.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/widget/widget.h"
#include "ui/views/widget/widget_delegate.h"
#include "ui/base/theme_provider.h"


#include "tabbed_pane_example.h"
#include "auto_tabbed_pane_example.h"
#include "guard_view.h"
#include "combobox_example.h"
//#include "button_example.h"
#include "login_example.h"


#include "../../phone_common/common/common_common.h"

#include "adb_view/auto_install_apk_list_table.h"
#include "../../phone_common/common/thread_message_filter.h"
#include "../../phone_common/include/ctp_notification_types.h"

namespace views {
namespace examples {

// Model for the examples that are being added via AddExample().
class ComboboxModelExampleList : public ui::ComboboxModel {
 public:
  ComboboxModelExampleList() {}
  virtual ~ComboboxModelExampleList() {}

  // Overridden from ui::ComboboxModel:
  virtual int GetItemCount() const OVERRIDE { return example_list_.size(); }
  virtual string16 GetItemAt(int index) OVERRIDE {
    return (example_list_[index]->example_title());
  }

  View* GetItemViewAt(int index) {
    string16 temp = example_list_[index]->example_title();
    if (temp == L"手动模式") {
      AutoInstallApkListTable::g_current_mode_ = 1;
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_ModeChange(1));
    } else {
      AutoInstallApkListTable::g_current_mode_ = 2;
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_ModeChange(2));
    }
    
    return example_list_[index]->example_view();
  }

  void AddExample(CTPViewBase* example) {
    example_list_.push_back(example);
  }

 private:
  ScopedVector<CTPViewBase> example_list_;

  DISALLOW_COPY_AND_ASSIGN(ComboboxModelExampleList);
};

class CTPWindowContents : public WidgetDelegateView,
                               public ThreadMessageFilter,
                               public ComboboxListener {
 public:
  CTPWindowContents(Operation operation, CommandLine const & c)
      : combobox_(new Combobox(&combobox_model_)),
        example_shown_(new View),
        status_label_(new Label),
        command_(c),
        icon_(NULL),
        debug_(false),
        ThreadMessageFilter(true),
        operation_(operation) {
    instance_ = this;
    combobox_->set_listener(this);
    process_type_ = command_.GetSwitchValueASCII(switches::kProcessType);
    notification_service_on_ui_thread_.reset(new content::NotificationServiceImpl);
    
  }
  virtual ~CTPWindowContents() {}

  virtual bool OnMessageReceived(IPC::Message const & message) OVERRIDE {
    if (message.routing_id() == MSG_ROUTING_CONTROL) {
      // Dispatch control messages.
      bool msg_is_ok = false;
      IPC_BEGIN_MESSAGE_MAP_EX(CTPWindowContents, message, msg_is_ok)

        //IPC_MESSAGE_HANDLER(L2U_ApkInstallInfo, OnUpdatePackageList)
        IPC_MESSAGE_HANDLER(L2U_InstallApkDigest, OnUpdateInstallApkDigest)
        IPC_MESSAGE_HANDLER(L2U_ApkTotalAutoModeInfoToString, OnApkUpdateInfoToString)
        IPC_MESSAGE_HANDLER(L2U_DevicesList, OnDeviceUpdate)
        IPC_MESSAGE_HANDLER(L2U_ApkIRStatus, OnUpdateApkIRStatus)
        IPC_MESSAGE_HANDLER(L2U_LoginResponse, OnLoginResponse)
        IPC_MESSAGE_HANDLER(L2U_DebugMode, OnDebugMode)

        //IPC_MESSAGE_UNHANDLED_ERROR()
        IPC_END_MESSAGE_MAP_EX()

        if (msg_is_ok) {
          return true;
        }
    }
    return false;
  }

  void OnUpdateInstallApkDigest(PointerWrapper<phone_module::InstallDigest> const & p) {
    phone_module::InstallDigest & digest = *p.get();
    content::NotificationService::current()->Notify(
      phone_module::NOTIFICATION_PHONE_TRANSFER_INSTALL_APK_DIGEST,
      content::Source<CTPWindowContents>(this),
      content::Details<phone_module::InstallDigest const>(&digest));
  }


  void OnApkUpdateInfoToString(std::wstring const & s) {
    content::NotificationService::current()->Notify(
      phone_module::NOTIFICATION_PHONE_TRANSFER_APK_UPDATE_INFO,
      content::Source<CTPWindowContents>(this),
      content::Details<std::wstring const>(&s));

  }

  void OnDeviceUpdate(PointerWrapper< phone_module::DevicesList> const & p) {
    phone_module::DevicesList & list = *p.get();
    OnUpdateStatusLine(list);
    content::NotificationService::current()->Notify(
      phone_module::NOTIFICATION_PHONE_TRANSFER_DEVICES_LIST,
      content::Source<CTPWindowContents>(this),
      content::Details<phone_module::DevicesList const>(&list));
  }
  
  void OnDebugMode(bool const debug) {
    debug_ = debug;
    CTPWindowContents::instance()->GetWidget()->UpdateWindowTitle();
  }

  void OnLoginResponse(bool const success, std::wstring const & info) {
    if (success) {
      //切换到自动界面
      example_shown_->RemoveAllChildViews(false);
      example_shown_->AddChildView(example_list_["auto"]->example_view());
      example_shown_->RequestFocus();
      Layout();
      //通知切换到自动模式
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_TotalAutoCmd(true));
    } else {
      //切换到登录界面
      example_shown_->RemoveAllChildViews(false);
      example_shown_->AddChildView(example_list_["login"]->example_view());
      example_shown_->RequestFocus();
      Layout();
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_TotalAutoCmd(false));
      content::NotificationService::current()->Notify(
        phone_module::NOTIFICATION_PHONE_LOGIN_FAILED_STATUS,
        content::Source<CTPWindowContents>(this),
        content::Details<std::wstring const>(&info));
    }
  }

  void OnUpdateApkIRStatus(PointerWrapper<phone_module::ApkIRStatus> const & p) {
    phone_module::ApkIRStatus & status = *p.get();
    content::NotificationService::current()->Notify(
      phone_module::NOTIFICATION_PHONE_TRANSFER_APK_IR_STATUS,
      content::Source<CTPWindowContents>(this),
      content::Details<phone_module::ApkIRStatus const>(&status));
  }

  void OnUpdateStatusLine(phone_module::DevicesList const & list) {
    std::wstring device(L"当前设备ID: ");
    for (auto it = list.begin(); it != list.end(); ++it) {
      device.append(it->serial_no).append(L", ");
    }
    SetStatus(device);
  }

  void Command(CommandLine const & c) {
    //command_ = &c;
  }

  CommandLine const & Command() {
    return command_;
  }

  // Prints a message in the status area, at the bottom of the window.
  void SetStatus(const std::wstring& status) {
    status_label_->SetText(status);
  }

  static CTPWindowContents* instance() { return instance_; }

 private:
  // Overridden from WidgetDelegateView:
  virtual bool CanResize() const OVERRIDE { return true; }
  virtual bool CanMaximize() const OVERRIDE { return true; }
  virtual string16 GetWindowTitle() const OVERRIDE {
    std::wstring head = L"刷包大师 (1.0.0.31) ";
    if (debug_) {
      head.append(L" : debug");
    } else {

    }
    return head;
  }
  virtual View* GetContentsView() OVERRIDE { return this; }
  virtual void WindowClosing() OVERRIDE {
    instance_ = NULL;
    if (operation_ == QUIT_ON_CLOSE)
      MessageLoopForUI::current()->Quit();
  }

  // Overridden from View:
  virtual void ViewHierarchyChanged(bool is_add,
                                    View* parent,
                                    View* child) OVERRIDE {
    if (is_add && child == this)
      InitExamplesWindow();
  }

  virtual gfx::ImageSkia GetWindowIcon() OVERRIDE {
    if (icon_ == NULL) {
      ui::ThemeProvider* tp = GetThemeProvider();
      icon_ = tp->GetImageSkiaNamed(IDR_FILE_MANAGER_IMG_FILETYPE_DRIVE);
    } 
    return gfx::ImageSkia(*icon_);
  }


  // Overridden from ComboboxListener:
  virtual void OnSelectedIndexChanged(Combobox* combobox) OVERRIDE {
    DCHECK_EQ(combobox, combobox_);
    DCHECK(combobox->selected_index() < combobox_model_.GetItemCount());
    example_shown_->RemoveAllChildViews(false);
    example_shown_->AddChildView(combobox_model_.GetItemViewAt(
        combobox->selected_index()));
    example_shown_->RequestFocus();
    //SetStatus(std::string());
    Layout();
  }

  // Creates the layout within the examples window.
  void InitExamplesWindow() {
    AddExamples();

    set_background(Background::CreateStandardPanelBackground());
    GridLayout* layout = new GridLayout(this);
    SetLayoutManager(layout);
    ColumnSet* column_set = layout->AddColumnSet(0);
    column_set->AddPaddingColumn(0, 5);
    column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
                          GridLayout::USE_PREF, 0, 0);
    column_set->AddPaddingColumn(0, 5);
    layout->AddPaddingRow(0, 5);
    layout->StartRow(0 /* no expand */, 0);
    layout->AddView(combobox_);

    if (combobox_model_.GetItemCount() > 0) {
      layout->StartRow(1, 0);
      example_shown_->SetLayoutManager(new FillLayout());
      example_shown_->AddChildView(combobox_model_.GetItemViewAt(0));
      layout->AddView(example_shown_);
    }

    layout->StartRow(0 /* no expand */, 0);
    layout->AddView(status_label_);
    layout->AddPaddingRow(0, 5);
  }

  // Adds all the individual examples to the combobox model.
  void AddExamples() {
    // Please keep this list in alphabetical order!
    //combobox_model_.AddExample(new BubbleExample);
    //combobox_model_.AddExample(new ButtonExample);
    
    
    // combobox_model_.AddExample(new DoubleSplitViewExample);
    // combobox_model_.AddExample(new LabelExample);
    // combobox_model_.AddExample(new LinkExample);
    // combobox_model_.AddExample(new MenuExample);
    // combobox_model_.AddExample(new MessageBoxExample);
    // combobox_model_.AddExample(new NativeThemeButtonExample);
    // combobox_model_.AddExample(new NativeThemeCheckboxExample);
    // combobox_model_.AddExample(new ProgressBarExample);
    // combobox_model_.AddExample(new RadioButtonExample);
    // combobox_model_.AddExample(new ScrollViewExample);
    // combobox_model_.AddExample(new SingleSplitViewExample);
    // combobox_model_.AddExample(new SliderExample);
    
    //combobox_model_.AddExample(new MQTable);
    if (process_type_.size() == 0) {
      CTPViewBase* p = new LoginExample;
      example_list_.insert(std::make_pair("login", p));
      combobox_model_.AddExample(p);
      p = new AutoTabbedPane;
      example_list_.insert(std::make_pair("auto", p));
      combobox_model_.AddExample(p);
      p = new CTPTabbedPane;
      example_list_.insert(std::make_pair("manual", p));
      combobox_model_.AddExample(p); 
      
    } else {
      combobox_model_.AddExample(new GuardView);
    }
    // combobox_model_.AddExample(new TextExample);
    // combobox_model_.AddExample(new TextfieldExample);
    // combobox_model_.AddExample(new ThrobberExample);
    // combobox_model_.AddExample(new TreeViewExample);
    // combobox_model_.AddExample(new WidgetExample);
    //combobox_model_.AddExample(new ComboboxExample);
  }

  static CTPWindowContents* instance_;
  ComboboxModelExampleList combobox_model_;
  std::map<std::string, CTPViewBase*> example_list_;
  Combobox* combobox_;
  View* example_shown_;
  Label* status_label_;
  const Operation operation_;
  scoped_ptr<content::NotificationServiceImpl> notification_service_on_ui_thread_;
  CommandLine const& command_;
  std::string process_type_;
  gfx::ImageSkia* icon_;
  bool debug_;

  DISALLOW_COPY_AND_ASSIGN(CTPWindowContents);
};

// static
CTPWindowContents* CTPWindowContents::instance_ = NULL;

void ShowExamplesWindow(Operation operation, CommandLine const & c) {
  if (CTPWindowContents::instance()) {
    //CTPWindowContents::instance()->Command(c);
    CTPWindowContents::instance()->GetWidget()->Activate();
  } else {
    gfx::Rect rect;
    std::string const process_type = c.GetSwitchValueASCII(switches::kProcessType);
    if (process_type.size() == 0) {
      rect = gfx::Rect(0, 0, 1400, 800); 
    } else {
      rect = gfx::Rect(0, 0, 550, 600);
    }
    Widget::CreateWindowWithBounds(new CTPWindowContents(operation, c),
                                   rect)->Show();
  }
}

CommandLine const & GetCommandLine() {
  return CTPWindowContents::instance()->Command();
}

void LogStatus(const std::wstring& string) {
  CTPWindowContents::instance()->SetStatus(string);
}

}  // namespace examples
}  // namespace views
