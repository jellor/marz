// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Im.Other.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "Im.Other.pb.h"

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

namespace Im {
namespace Other {

namespace {

const ::google::protobuf::Descriptor* HeartBeat_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  HeartBeat_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_Im_2eOther_2eproto() {
  protobuf_AddDesc_Im_2eOther_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "Im.Other.proto");
  GOOGLE_CHECK(file != NULL);
  HeartBeat_descriptor_ = file->message_type(0);
  static const int HeartBeat_offsets_[1] = {
  };
  HeartBeat_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      HeartBeat_descriptor_,
      HeartBeat::default_instance_,
      HeartBeat_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HeartBeat, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HeartBeat, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(HeartBeat));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_Im_2eOther_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    HeartBeat_descriptor_, &HeartBeat::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_Im_2eOther_2eproto() {
  delete HeartBeat::default_instance_;
  delete HeartBeat_reflection_;
}

void protobuf_AddDesc_Im_2eOther_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\016Im.Other.proto\022\010Im.Other\"\013\n\tHeartBeatB"
    ")\n\'com.example.user.myapplication.protob"
    "uf", 82);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "Im.Other.proto", &protobuf_RegisterTypes);
  HeartBeat::default_instance_ = new HeartBeat();
  HeartBeat::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_Im_2eOther_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_Im_2eOther_2eproto {
  StaticDescriptorInitializer_Im_2eOther_2eproto() {
    protobuf_AddDesc_Im_2eOther_2eproto();
  }
} static_descriptor_initializer_Im_2eOther_2eproto_;

// ===================================================================

#ifndef _MSC_VER
#endif  // !_MSC_VER

HeartBeat::HeartBeat()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Im.Other.HeartBeat)
}

void HeartBeat::InitAsDefaultInstance() {
}

HeartBeat::HeartBeat(const HeartBeat& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:Im.Other.HeartBeat)
}

void HeartBeat::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

HeartBeat::~HeartBeat() {
  // @@protoc_insertion_point(destructor:Im.Other.HeartBeat)
  SharedDtor();
}

void HeartBeat::SharedDtor() {
  if (this != default_instance_) {
  }
}

void HeartBeat::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* HeartBeat::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return HeartBeat_descriptor_;
}

const HeartBeat& HeartBeat::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_Im_2eOther_2eproto();
  return *default_instance_;
}

HeartBeat* HeartBeat::default_instance_ = NULL;

HeartBeat* HeartBeat::New() const {
  return new HeartBeat;
}

void HeartBeat::Clear() {
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool HeartBeat::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:Im.Other.HeartBeat)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
  handle_unusual:
    if (tag == 0 ||
        ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
        ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
      goto success;
    }
    DO_(::google::protobuf::internal::WireFormat::SkipField(
          input, tag, mutable_unknown_fields()));
  }
success:
  // @@protoc_insertion_point(parse_success:Im.Other.HeartBeat)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:Im.Other.HeartBeat)
  return false;
#undef DO_
}

void HeartBeat::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:Im.Other.HeartBeat)
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:Im.Other.HeartBeat)
}

::google::protobuf::uint8* HeartBeat::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:Im.Other.HeartBeat)
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Im.Other.HeartBeat)
  return target;
}

int HeartBeat::ByteSize() const {
  int total_size = 0;

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

void HeartBeat::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const HeartBeat* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const HeartBeat*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void HeartBeat::MergeFrom(const HeartBeat& from) {
  GOOGLE_CHECK_NE(&from, this);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void HeartBeat::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void HeartBeat::CopyFrom(const HeartBeat& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool HeartBeat::IsInitialized() const {

  return true;
}

void HeartBeat::Swap(HeartBeat* other) {
  if (other != this) {
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata HeartBeat::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = HeartBeat_descriptor_;
  metadata.reflection = HeartBeat_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace Other
}  // namespace Im

// @@protoc_insertion_point(global_scope)
