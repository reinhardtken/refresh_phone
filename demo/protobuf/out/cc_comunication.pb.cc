// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: cc_comunication.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "cc_comunication.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace ctp {
namespace c2c {

namespace {

const ::google::protobuf::Descriptor* Command_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Command_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_cc_5fcomunication_2eproto() {
  protobuf_AddDesc_cc_5fcomunication_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "cc_comunication.proto");
  GOOGLE_CHECK(file != NULL);
  Command_descriptor_ = file->message_type(0);
  static const int Command_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Command, cmd_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Command, params_),
  };
  Command_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Command_descriptor_,
      Command::default_instance_,
      Command_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Command, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Command, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Command));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_cc_5fcomunication_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Command_descriptor_, &Command::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_cc_5fcomunication_2eproto() {
  delete Command::default_instance_;
  delete Command_reflection_;
}

void protobuf_AddDesc_cc_5fcomunication_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\025cc_comunication.proto\022\007ctp.c2c\"&\n\007Comm"
    "and\022\013\n\003cmd\030\001 \002(\t\022\016\n\006params\030\002 \001(\t", 72);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "cc_comunication.proto", &protobuf_RegisterTypes);
  Command::default_instance_ = new Command();
  Command::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_cc_5fcomunication_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_cc_5fcomunication_2eproto {
  StaticDescriptorInitializer_cc_5fcomunication_2eproto() {
    protobuf_AddDesc_cc_5fcomunication_2eproto();
  }
} static_descriptor_initializer_cc_5fcomunication_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int Command::kCmdFieldNumber;
const int Command::kParamsFieldNumber;
#endif  // !_MSC_VER

Command::Command()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Command::InitAsDefaultInstance() {
}

Command::Command(const Command& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Command::SharedCtor() {
  _cached_size_ = 0;
  cmd_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  params_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Command::~Command() {
  SharedDtor();
}

void Command::SharedDtor() {
  if (cmd_ != &::google::protobuf::internal::kEmptyString) {
    delete cmd_;
  }
  if (params_ != &::google::protobuf::internal::kEmptyString) {
    delete params_;
  }
  if (this != default_instance_) {
  }
}

void Command::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Command::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Command_descriptor_;
}

const Command& Command::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_cc_5fcomunication_2eproto();
  return *default_instance_;
}

Command* Command::default_instance_ = NULL;

Command* Command::New() const {
  return new Command;
}

void Command::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_cmd()) {
      if (cmd_ != &::google::protobuf::internal::kEmptyString) {
        cmd_->clear();
      }
    }
    if (has_params()) {
      if (params_ != &::google::protobuf::internal::kEmptyString) {
        params_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Command::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string cmd = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_cmd()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->cmd().data(), this->cmd().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_params;
        break;
      }

      // optional string params = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_params:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_params()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->params().data(), this->params().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Command::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required string cmd = 1;
  if (has_cmd()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->cmd().data(), this->cmd().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->cmd(), output);
  }

  // optional string params = 2;
  if (has_params()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->params().data(), this->params().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->params(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Command::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required string cmd = 1;
  if (has_cmd()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->cmd().data(), this->cmd().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->cmd(), target);
  }

  // optional string params = 2;
  if (has_params()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->params().data(), this->params().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->params(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Command::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string cmd = 1;
    if (has_cmd()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->cmd());
    }

    // optional string params = 2;
    if (has_params()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->params());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Command::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Command* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Command*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Command::MergeFrom(const Command& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_cmd()) {
      set_cmd(from.cmd());
    }
    if (from.has_params()) {
      set_params(from.params());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Command::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Command::CopyFrom(const Command& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Command::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void Command::Swap(Command* other) {
  if (other != this) {
    std::swap(cmd_, other->cmd_);
    std::swap(params_, other->params_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Command::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Command_descriptor_;
  metadata.reflection = Command_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace c2c
}  // namespace ctp

// @@protoc_insertion_point(global_scope)
