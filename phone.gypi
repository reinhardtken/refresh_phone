# Copyright (c) 2010 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

#chinese example
#http://erikge.com/articles/HelloGyp/

{
   'targets': [
    #phone_common##################################################################
    {
      'target_name': 'phone_common',
      'type': 'shared_library',      
      'msvs_guid': '1E4CFDB6-FC49-4F62-AC83-3503FC373EA0',
      'msvs_settings': {
	'VCCLCompilerTool': {
	  'WarningLevel': '3',
	},
      },		
      'dependencies': [
        '../base/base.gyp:base',
		'../base/base.gyp:base_static',
		'../content/content.gyp:content',
		'../base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
		'../ipc/ipc.gyp:ipc',
		'../ui/ui.gyp:ui',
		'../net/net.gyp:net',
      ],
      'include_dirs': [
      '../third_party/boost_1350_vc80/include/boost-1_35',
	  '../ctp_protobuf/src',
	  './demo',
      ],
      'defines': [
        '<@(nacl_defines)',
	    'PHONE_COMMON',
      ],
      'direct_dependent_settings': {
        'defines': [
          '<@(nacl_defines)',
        ],
      },
      'sources': [
        #common
		'../phone/demo/phone_common/common/command_parser.cc',
		'../phone/demo/phone_common/common/command_parser.h',
		'../phone/demo/phone_common/common/common_common.cc',
		'../phone/demo/phone_common/common/common_common.h',
		'../phone/demo/phone_common/common/common_exception_handler.cc',
		'../phone/demo/phone_common/common/common_exception_handler.h',
		'../phone/demo/phone_common/common/common_logging_new.cc',
		'../phone/demo/phone_common/common/common_logging_new.h',
		'../phone/demo/phone_common/common/common_main_parts.cc',
		'../phone/demo/phone_common/common/common_main_parts.h',
		'../phone/demo/phone_common/common/common_platform.cc',
		'../phone/demo/phone_common/common/common_platform.h',
		'../phone/demo/phone_common/common/common_process.cc',
		'../phone/demo/phone_common/common/common_process.h',
		'../phone/demo/phone_common/common/common_thread.cc',
		'../phone/demo/phone_common/common/common_thread.h',
		'../phone/demo/phone_common/common/thread_message_filter.cc',
		'../phone/demo/phone_common/common/thread_message_filter.h',
		'../phone/demo/phone_common/common/common_ui.cc',
		'../phone/demo/phone_common/common/common_ui.h',
		'../phone/demo/phone_common/common/common_util.cc',
		'../phone/demo/phone_common/common/common_util.h',
		'../phone/demo/phone_common/common/result_codes.h',
		'../phone/demo/phone_common/common/module_manager.h',
		'../phone/demo/phone_common/common/module_manager.cpp',
		'../phone/demo/phone_common/common/core_module.cpp',
		'../phone/demo/phone_common/common/core_module.h',
		'../phone/demo/phone_common/common/blowfish.cpp',
		'../phone/demo/phone_common/common/blowfish.h',

		'../phone/demo/phone_common/common/common_ipc_client.cc',
		'../phone/demo/phone_common/common/common_ipc_client.h',
		'../phone/demo/phone_common/common/common_ipc_server.cc',
		'../phone/demo/phone_common/common/common_ipc_server.h',
		'../phone/demo/phone_common/common/paths.cc',
		'../phone/demo/phone_common/common/paths.h',
		
		#copy from browser
		'../phone/demo/phone_common/common/pref_service.cc',
		'../phone/demo/phone_common/common/pref_service.h',
		'../phone/demo/phone_common/common/pref_value_store.cc',
		'../phone/demo/phone_common/common/pref_value_store.h',
		#end
		'../phone/demo/phone_common/common/common_observable.h',
		'../phone/demo/phone_common/common/initlist.h',
		#copy from base/prefs
		'../phone/demo/phone_common/common/prefs/default_pref_store.cc',
		'../phone/demo/phone_common/common/prefs/default_pref_store.h',
		'../phone/demo/phone_common/common/prefs/json_pref_store.cc',
		'../phone/demo/phone_common/common/prefs/json_pref_store.h',
		
		'../phone/demo/phone_common/common/prefs/overlay_user_pref_store.cc',
		'../phone/demo/phone_common/common/prefs/overlay_user_pref_store.h',
		'../phone/demo/phone_common/common/prefs/pref_notifier.h',
		'../phone/demo/phone_common/common/prefs/pref_store.cc',
		'../phone/demo/phone_common/common/prefs/pref_store.h',
		'../phone/demo/phone_common/common/prefs/pref_value_map.cc',
		'../phone/demo/phone_common/common/prefs/pref_value_map.h',
		'../phone/demo/phone_common/common/prefs/public/pref_service_base.h',
		'../phone/demo/phone_common/common/prefs/value_map_pref_store.cc',
		'../phone/demo/phone_common/common/prefs/value_map_pref_store.h',
		#include 
		'../phone/demo/phone_common/include/ctp_type.h',
		'../phone/demo/phone_common/include/imodule.h',
		'../phone/demo/phone_common/include/imodulemanager.h',
		'../phone/demo/phone_common/include/moduleimpl.h',
		'../phone/demo/phone_common/include/ctp_define.h',
		'../phone/demo/phone_common/include/common_macro.h',
		'../phone/demo/phone_common/include/ctp_messages.h',
		'../phone/demo/phone_common/include/ctp_messages.cc',
		'../phone/demo/phone_common/include/ctp_notification_types.h',
		'../phone/demo/phone_common/include/ctp_notification_types_internal.h',
		'../phone/demo/phone_common/include/ctp_notification_types.cc',
		#channel
			#codec
		'../phone/demo/phone_common/channel/codec/codec.cc',
		'../phone/demo/phone_common/channel/codec/codec.h',
		'../phone/demo/phone_common/channel/codec/codec_util.cc',
		'../phone/demo/phone_common/channel/codec/codec_util.h',
		'../phone/demo/phone_common/channel/codec/protobuf_codec.cc',
		'../phone/demo/phone_common/channel/codec/protobuf_codec.h',
		'../phone/demo/phone_common/channel/codec/tlv_codec.cc',
		'../phone/demo/phone_common/channel/codec/tlv_codec.h',
		'../phone/demo/phone_common/channel/codec/tlv_define.cc',
		'../phone/demo/phone_common/channel/codec/tlv_define.h',
		
		'../phone/demo/phone_common/channel/channel_host.cc',
		'../phone/demo/phone_common/channel/channel_host.h',
		'../phone/demo/phone_common/channel/ctp_socket.cc',
		'../phone/demo/phone_common/channel/ctp_socket.h',
		'../phone/demo/phone_common/channel/ctp_tcp_socket.cc',
		'../phone/demo/phone_common/channel/ctp_tcp_socket.h',
		
		#system_monitor
		'../phone/demo/phone_common/system_monitor/media_device_notifications_utils.cc',
		'../phone/demo/phone_common/system_monitor/media_device_notifications_utils.h',
		'../phone/demo/phone_common/system_monitor/media_storage_util.cc',
		'../phone/demo/phone_common/system_monitor/media_storage_util.h',
		'../phone/demo/phone_common/system_monitor/removable_device_constants.cc',
		'../phone/demo/phone_common/system_monitor/removable_device_constants.h',
		'../phone/demo/phone_common/system_monitor/removable_device_notifications_window_win.cc',
		'../phone/demo/phone_common/system_monitor/removable_device_notifications_window_win.h',
		'../phone/demo/phone_common/system_monitor/volume_mount_watcher_win.cc',
		'../phone/demo/phone_common/system_monitor/volume_mount_watcher_win.h',
		
		#protobuf
		'../phone/demo/protobuf/out/apk_protomsg.pb.cc',
		'../phone/demo/protobuf/out/apk_protomsg.pb.h',
      ],
      'link_settings': {
	  'libraries': [
              '-l$(OutDir)/lib/libprotobuf.lib',
            ],
      },
    },
    #phone_spi##################################################################
    {
      'target_name': 'phone_spi',
      'type': 'loadable_module',
      'msvs_guid': '9AF1ACD2-0E6A-43CB-8848-5FC795692FD2',
      'msvs_settings': {
		'VCCLCompilerTool': {
		  'WarningLevel': '3',
		},
        'VCLinkerTool': {
	      'AdditionalDependencies': [
			  
            ],
	      'AdditionalLibraryDirectories':
              ['../phone/demo/lib',
			  
			  ],
	  },
	  },		
      'dependencies': [
        'phone_common',
        '../base/base.gyp:base',
	'../content/content.gyp:content',
	'../ipc/ipc.gyp:ipc',
	
      ],
      'include_dirs': [
      '../third_party/boost_1350_vc80/include/boost-1_35',
	  '../ctp_protobuf/src',
	  './demo',
      ],
      'defines': [
        '<@(nacl_defines)',
		'PHONE_SPI',
      ],
      'direct_dependent_settings': {
        'defines': [
          '<@(nacl_defines)',
        ],
      },
      'sources': [
        
        '../phone/demo/phone_spi/dllmain.cpp',
		'../phone/demo/phone_spi/targetver.h',
		
		
		'../phone/demo/phone_spi/phone_module/phone_module.cpp',
		'../phone/demo/phone_spi/phone_module/phone_module.h',
		'../phone/demo/phone_spi/phone_module/SMFramework.h',
		'../phone/demo/phone_spi/phone_module/ctp_factory.cpp',
		
		'../phone/demo/phone_spi/phone_module/common.h',
		'../phone/demo/phone_spi/phone_module/common.cpp',
		
		'../phone/demo/phone_spi/phone_module/paths.cc',
		'../phone/demo/phone_spi/phone_module/paths.h',
		
		
		'../phone/demo/phone_spi/phone_module/memo.h',
		
		#protobuf
		
		'../phone/demo/protobuf/out/apk_protomsg.pb.cc',
		'../phone/demo/protobuf/out/apk_protomsg.pb.h',
		#sqlite
		'../phone/demo/phone_spi/sqlite/persistent_store_base_ex.cc',
		'../phone/demo/phone_spi/sqlite/persistent_store_base_ex.h',
		'../phone/demo/phone_spi/sqlite/persistent_store_file_ex.cc',
		'../phone/demo/phone_spi/sqlite/persistent_store_file_ex.h',
		'../phone/demo/phone_spi/sqlite/tableset_order_footprints.cc',
		'../phone/demo/phone_spi/sqlite/tableset_order_footprints.h',
		
		
		#adbtool
		'../phone/demo/phone_spi/adbtool/adb_interface.cc',
		'../phone/demo/phone_spi/adbtool/adb_interface.h',
		'../phone/demo/phone_spi/adbtool/adb_py_interface.cc',
		'../phone/demo/phone_spi/adbtool/adb_py_interface.h',

		
		#phone_message.cc
		'../phone/demo/phone_common/include/ctp_messages.cc',
	
      ],
      'link_settings': {
		'libraries': [
              '-l$(OutDir)/lib/libprotobuf.lib',
			  '-l$(OutDir)/lib/sql.lib',
			  '-l$(OutDir)/lib/phone_common.lib',
      ],
      },
    },
    
	#phone_exe##################################################################
    {
      'target_name': 'phone_exe',
      'type': 'executable',
      'msvs_guid': 'B0E7F032-249A-470B-9BA3-1BAE43F1C6D8',
      'msvs_settings': {
		'VCCLCompilerTool': {
		  'WarningLevel': '3',
		},
		'VCLinkerTool': {
		  'SubSystem': '2',
	    },
	  },		
      'dependencies': [
        'phone_common',
		'phone_spi',
        '../base/base.gyp:base',
		'../base/base.gyp:base_static',
		'../base/base.gyp:base_i18n',

		'../content/content.gyp:content',
		'../ipc/ipc.gyp:ipc',
		
		'../ui/base/strings/ui_strings.gyp:ui_strings',
        '../ui/compositor/compositor.gyp:compositor',
        '../ui/surface/surface.gyp:surface',
        '../ui/ui.gyp:ui',
        '../ui/ui.gyp:ui_resources',
      ],
      'include_dirs': [
        '../third_party/boost_1350_vc80/include/boost-1_35',
		'../ctp_protobuf/src',
		'./demo',
		'../third_party/skia/include/config',
		'../third_party/skia/include/core',
		'<(PRODUCT_DIR)/obj/global_intermediate/chrome',
      ],
      'defines': [
        '<@(nacl_defines)',
      ],
      'direct_dependent_settings': {
        'defines': [
          '<@(nacl_defines)',
        ],
      },
      'sources': [
	
		#phone
		'../phone/demo/phone_wrapper_exe/custom_list_view.h',
		'../phone/demo/phone_wrapper_exe/phone_wrapper_exe.cpp',
		'../phone/demo/phone_wrapper_exe/targetver.h',
		#phone_view
			#backtesting_view
		'../phone/demo/phone_wrapper_exe/phone_view/backtesting_view/backtesting_table.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/backtesting_view/backtesting_table.h',
			#tu_view
		'../phone/demo/phone_wrapper_exe/phone_view/tu_view/tu_commission_table.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/tu_view/tu_commission_table.h',
		'../phone/demo/phone_wrapper_exe/phone_view/tu_view/tu_control_table.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/tu_view/tu_control_table.h',
		'../phone/demo/phone_wrapper_exe/phone_view/tu_view/tu_marginrate_table.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/tu_view/tu_marginrate_table.h',
		'../phone/demo/phone_wrapper_exe/phone_view/tu_view/tu_order_result_table.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/tu_view/tu_order_result_table.h',
		'../phone/demo/phone_wrapper_exe/phone_view/tu_view/tu_tabbed_pane_example.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/tu_view/tu_tabbed_pane_example.h',
		'../phone/demo/phone_wrapper_exe/phone_view/tu_view/tu_table.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/tu_view/tu_table.h',
		
		#adb_view
		'../phone/demo/phone_wrapper_exe/phone_view/adb_view/package_list_table.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/adb_view/package_list_table.h',
		'../phone/demo/phone_wrapper_exe/phone_view/adb_view/install_apk_list_table.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/adb_view/install_apk_list_table.h',
		'../phone/demo/phone_wrapper_exe/phone_view/adb_view/update_view.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/adb_view/update_view.h',
		'../phone/demo/phone_wrapper_exe/phone_view/adb_view/main_view.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/adb_view/main_view.h',
		
		'../phone/demo/phone_wrapper_exe/phone_view/cc_table.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/cc_table.h',
		'../phone/demo/phone_wrapper_exe/phone_view/combobox_example.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/combobox_example.h',
		'../phone/demo/phone_wrapper_exe/phone_view/example_base.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/example_base.h',
		'../phone/demo/phone_wrapper_exe/phone_view/examples_window.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/examples_window.h',
		'../phone/demo/phone_wrapper_exe/phone_view/guard_view.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/guard_view.h',
		'../phone/demo/phone_wrapper_exe/phone_view/refresh_view.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/refresh_view.h',
		'../phone/demo/phone_wrapper_exe/phone_view/mq_guard_view.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/mq_guard_view.h',
		'../phone/demo/phone_wrapper_exe/phone_view/mq_tabbed_pane_example.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/mq_tabbed_pane_example.h',
		'../phone/demo/phone_wrapper_exe/phone_view/mq_table.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/mq_table.h',
		'../phone/demo/phone_wrapper_exe/phone_view/ms_table.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/ms_table.h',
		'../phone/demo/phone_wrapper_exe/phone_view/tabbed_pane_example.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/tabbed_pane_example.h',
		'../phone/demo/phone_wrapper_exe/phone_view/position_data.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/position_data.h',
		'../phone/demo/phone_wrapper_exe/phone_view/ui_common.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/ui_common.h',
		'../phone/demo/phone_wrapper_exe/phone_view/test_view.cc',
		'../phone/demo/phone_wrapper_exe/phone_view/test_view.h',
		
		#ctp_message.cc
		'../phone/demo/phone_common/include/ctp_messages.cc',
      ],
      'link_settings': {
		'libraries': [
              '-l$(OutDir)/lib/libprotobuf.lib',
			  '-l$(OutDir)/lib/phone_common.lib',
			  '-l$(OutDir)/lib/phone_spi.lib',
			  '-l$(OutDir)/lib/views.lib',
			  '-l$(OutDir)/lib/skia.lib',
			  '-l$(OutDir)/lib/base_i18n.lib',

      ],
      },
    },
  ],
}

# Local Variables:
# tab-width:2
# indent-tabs-mode:nil
# End:
# vim: set expandtab tabstop=2 shiftwidth=2:
