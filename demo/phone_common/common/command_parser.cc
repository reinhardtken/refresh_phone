// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "command_parser.h"

#include <algorithm>
#include <ostream>

#include "base/basictypes.h"
#include "base/file_path.h"
#include "base/logging.h"
#include "base/string_split.h"
#include "base/string_util.h"
#include "base/utf_string_conversions.h"
#include "build/build_config.h"

#if defined(OS_WIN)
#include <windows.h>
#include <shellapi.h>
#endif

//CommandParser* CommandParser::current_process_commandline_ = NULL;

namespace {
const CommandParser::CharType kSwitchTerminator[] = FILE_PATH_LITERAL("--");
const CommandParser::CharType kSwitchValueSeparator[] = FILE_PATH_LITERAL("=");
// Since we use a lazy match, make sure that longer versions (like "--") are
// listed before shorter versions (like "-") of similar prefixes.
#if defined(OS_WIN)
const CommandParser::CharType* const kSwitchPrefixes[] = {L"--", L"-", L"/"};
#elif defined(OS_POSIX)
// Unixes don't use slash as a switch.
const CommandParser::CharType* const kSwitchPrefixes[] = {"--", "-"};
#endif

size_t GetSwitchPrefixLength(const CommandParser::StringType& string) {
  for (size_t i = 0; i < arraysize(kSwitchPrefixes); ++i) {
    CommandParser::StringType prefix(kSwitchPrefixes[i]);
    if (string.compare(0, prefix.length(), prefix) == 0)
      return prefix.length();
  }
  return 0;
}

// Fills in |switch_string| and |switch_value| if |string| is a switch.
// This will preserve the input switch prefix in the output |switch_string|.
bool IsSwitch(const CommandParser::StringType& string,
              CommandParser::StringType* switch_string,
              CommandParser::StringType* switch_value) {
  switch_string->clear();
  switch_value->clear();
  size_t prefix_length = GetSwitchPrefixLength(string);
  if (prefix_length == 0 || prefix_length == string.length())
    return false;

  const size_t equals_position = string.find(kSwitchValueSeparator);
  *switch_string = string.substr(0, equals_position);
  if (equals_position != CommandParser::StringType::npos)
    *switch_value = string.substr(equals_position + 1);
  return true;
}

// Append switches and arguments, keeping switches before arguments.
void AppendSwitchesAndArguments(CommandParser& command_line,
                                const CommandParser::StringVector& argv) {
  bool parse_switches = true;
  for (size_t i = 1; i < argv.size(); ++i) {
    CommandParser::StringType arg = argv[i];
    TrimWhitespace(arg, TRIM_ALL, &arg);

    CommandParser::StringType switch_string;
    CommandParser::StringType switch_value;
    parse_switches &= (arg != kSwitchTerminator);
    if (parse_switches && IsSwitch(arg, &switch_string, &switch_value)) {
#if defined(OS_WIN)
      command_line.AppendSwitchNative(WideToASCII(switch_string), switch_value);
#elif defined(OS_POSIX)
      command_line.AppendSwitchNative(switch_string, switch_value);
#endif
    } else {
      command_line.AppendArgNative(arg);
    }
  }
}

// Lowercase switches for backwards compatiblity *on Windows*.
std::string LowerASCIIOnWindows(const std::string& string) {
#if defined(OS_WIN)
  return StringToLowerASCII(string);
#elif defined(OS_POSIX)
  return string;
#endif
}


#if defined(OS_WIN)
// Quote a string as necessary for CommandLineToArgvW compatiblity *on Windows*.
std::wstring QuoteForCommandLineToArgvW(const std::wstring& arg) {
  // We follow the quoting rules of CommandLineToArgvW.
  // http://msdn.microsoft.com/en-us/library/17w5ykft.aspx
  if (arg.find_first_of(L" \\\"") == std::wstring::npos) {
    // No quoting necessary.
    return arg;
  }

  std::wstring out;
  out.push_back(L'"');
  for (size_t i = 0; i < arg.size(); ++i) {
    if (arg[i] == '\\') {
      // Find the extent of this run of backslashes.
      size_t start = i, end = start + 1;
      for (; end < arg.size() && arg[end] == '\\'; ++end)
        /* empty */;
      size_t backslash_count = end - start;

      // Backslashes are escapes only if the run is followed by a double quote.
      // Since we also will end the string with a double quote, we escape for
      // either a double quote or the end of the string.
      if (end == arg.size() || arg[end] == '"') {
        // To quote, we need to output 2x as many backslashes.
        backslash_count *= 2;
      }
      for (size_t j = 0; j < backslash_count; ++j)
        out.push_back('\\');

      // Advance i to one before the end to balance i++ in loop.
      i = end - 1;
    } else if (arg[i] == '"') {
      out.push_back('\\');
      out.push_back('"');
    } else {
      out.push_back(arg[i]);
    }
  }
  out.push_back('"');

  return out;
}
#endif

}  // namespace

CommandParser::CommandParser(NoProgram no_program)
    : argv_(1),
      begin_args_(1) {
}

CommandParser::CommandParser(const FilePath& program)
    : argv_(1),
      begin_args_(1) {
  SetProgram(program);
}

CommandParser::CommandParser(int argc, const CommandParser::CharType* const* argv)
    : argv_(1),
      begin_args_(1) {
  InitFromArgv(argc, argv);
}

CommandParser::CommandParser(const StringVector& argv)
    : argv_(1),
      begin_args_(1) {
  InitFromArgv(argv);
}

CommandParser::~CommandParser() {
}

// static

#if defined(OS_WIN)
// static
CommandParser CommandParser::FromString(const std::wstring& command_line) {
  CommandParser cmd(NO_PROGRAM);
  cmd.ParseFromString(command_line);
  return cmd;
}
#endif

void CommandParser::InitFromArgv(int argc,
                               const CommandParser::CharType* const* argv) {
  StringVector new_argv;
  for (int i = 0; i < argc; ++i)
    new_argv.push_back(argv[i]);
  InitFromArgv(new_argv);
}

void CommandParser::InitFromArgv(const StringVector& argv) {
  argv_ = StringVector(1);
  begin_args_ = 1;
  SetProgram(argv.empty() ? FilePath() : FilePath(argv[0]));
  AppendSwitchesAndArguments(*this, argv);
}

CommandParser::StringType CommandParser::GetCommandLineString() const {
  StringType string(argv_[0]);
#if defined(OS_WIN)
  string = QuoteForCommandLineToArgvW(string);
#endif
  StringType params(GetArgumentsString());
  if (!params.empty()) {
    string.append(StringType(FILE_PATH_LITERAL(" ")));
    string.append(params);
  }
  return string;
}

CommandParser::StringType CommandParser::GetArgumentsString() const {
  StringType params;
  // Append switches and arguments.
  bool parse_switches = true;
  for (size_t i = 1; i < argv_.size(); ++i) {
    StringType arg = argv_[i];
    StringType switch_string;
    StringType switch_value;
    parse_switches &= arg != kSwitchTerminator;
    if (i > 1)
      params.append(StringType(FILE_PATH_LITERAL(" ")));
    if (parse_switches && IsSwitch(arg, &switch_string, &switch_value)) {
      params.append(switch_string);
      if (!switch_value.empty()) {
#if defined(OS_WIN)
        switch_value = QuoteForCommandLineToArgvW(switch_value);
#endif
        params.append(kSwitchValueSeparator + switch_value);
      }
    }
    else {
#if defined(OS_WIN)
      arg = QuoteForCommandLineToArgvW(arg);
#endif
      params.append(arg);
    }
  }
  return params;
}

FilePath CommandParser::GetCommand() const {
  return FilePath(argv_[0]);
}


void CommandParser::SetProgram(const FilePath& program) {
  TrimWhitespace(program.value(), TRIM_ALL, &argv_[0]);
}

bool CommandParser::HasSwitch(const std::string& switch_string) const {
  return switches_.find(LowerASCIIOnWindows(switch_string)) != switches_.end();
}

std::string CommandParser::GetSwitchValueASCII(
    const std::string& switch_string) const {
  StringType value = GetSwitchValueNative(switch_string);
  if (!IsStringASCII(value)) {
    DLOG(WARNING) << "Value of switch (" << switch_string << ") must be ASCII.";
    return std::string();
  }
#if defined(OS_WIN)
  return WideToASCII(value);
#else
  return value;
#endif
}

FilePath CommandParser::GetSwitchValuePath(
    const std::string& switch_string) const {
  return FilePath(GetSwitchValueNative(switch_string));
}

CommandParser::StringType CommandParser::GetSwitchValueNative(
    const std::string& switch_string) const {
  SwitchMap::const_iterator result = switches_.end();
  result = switches_.find(LowerASCIIOnWindows(switch_string));
  return result == switches_.end() ? StringType() : result->second;
}

void CommandParser::AppendSwitch(const std::string& switch_string) {
  AppendSwitchNative(switch_string, StringType());
}

void CommandParser::AppendSwitchPath(const std::string& switch_string,
                                   const FilePath& path) {
  AppendSwitchNative(switch_string, path.value());
}

void CommandParser::AppendSwitchNative(const std::string& switch_string,
                                     const CommandParser::StringType& value) {
  std::string switch_key(LowerASCIIOnWindows(switch_string));
#if defined(OS_WIN)
  StringType combined_switch_string(ASCIIToWide(switch_key));
#elif defined(OS_POSIX)
  StringType combined_switch_string(switch_string);
#endif
  size_t prefix_length = GetSwitchPrefixLength(combined_switch_string);
  switches_[switch_key.substr(prefix_length)] = value;
  // Preserve existing switch prefixes in |argv_|; only append one if necessary.
  if (prefix_length == 0)
    combined_switch_string = kSwitchPrefixes[0] + combined_switch_string;
  if (!value.empty())
    combined_switch_string += kSwitchValueSeparator + value;
  // Append the switch and update the switches/arguments divider |begin_args_|.
  argv_.insert(argv_.begin() + begin_args_++, combined_switch_string);
}

void CommandParser::AppendSwitchASCII(const std::string& switch_string,
                                    const std::string& value_string) {
#if defined(OS_WIN)
  AppendSwitchNative(switch_string, ASCIIToWide(value_string));
#elif defined(OS_POSIX)
  AppendSwitchNative(switch_string, value_string);
#endif
}

void CommandParser::CopySwitchesFrom(const CommandParser& source,
                                   const char* const switches[],
                                   size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (source.HasSwitch(switches[i]))
      AppendSwitchNative(switches[i], source.GetSwitchValueNative(switches[i]));
  }
}

CommandParser::StringVector CommandParser::GetArgs() const {
  // Gather all arguments after the last switch (may include kSwitchTerminator).
  StringVector args(argv_.begin() + begin_args_, argv_.end());
  // Erase only the first kSwitchTerminator (maybe "--" is a legitimate page?)
  StringVector::iterator switch_terminator =
      std::find(args.begin(), args.end(), kSwitchTerminator);
  if (switch_terminator != args.end())
    args.erase(switch_terminator);
  return args;
}

void CommandParser::AppendArg(const std::string& value) {
#if defined(OS_WIN)
  DCHECK(IsStringUTF8(value));
  AppendArgNative(UTF8ToWide(value));
#elif defined(OS_POSIX)
  AppendArgNative(value);
#endif
}

void CommandParser::AppendArgPath(const FilePath& path) {
  AppendArgNative(path.value());
}

void CommandParser::AppendArgNative(const CommandParser::StringType& value) {
  argv_.push_back(value);
}

void CommandParser::AppendArguments(const CommandParser& other,
                                  bool include_program) {
  if (include_program)
    SetProgram(other.GetCommand());
  AppendSwitchesAndArguments(*this, other.argv());
}

void CommandParser::PrependWrapper(const CommandParser::StringType& wrapper) {
  if (wrapper.empty())
    return;
  // The wrapper may have embedded arguments (like "gdb --args"). In this case,
  // we don't pretend to do anything fancy, we just split on spaces.
  StringVector wrapper_argv;
  base::SplitString(wrapper, FILE_PATH_LITERAL(' '), &wrapper_argv);
  // Prepend the wrapper and update the switches/arguments |begin_args_|.
  argv_.insert(argv_.begin(), wrapper_argv.begin(), wrapper_argv.end());
  begin_args_ += wrapper_argv.size();
}

#if defined(OS_WIN)
void CommandParser::ParseFromString(const std::wstring& command_line) {
  std::wstring command_line_string;
  TrimWhitespace(command_line, TRIM_ALL, &command_line_string);
  if (command_line_string.empty())
    return;

  int num_args = 0;
  wchar_t** args = NULL;
  args = ::CommandLineToArgvW(command_line_string.c_str(), &num_args);

  DPLOG_IF(FATAL, !args) << "CommandLineToArgvW failed on command line: "
                         << command_line;
  InitFromArgv(num_args, args);
  LocalFree(args);
}
#endif
