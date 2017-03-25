// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Im.Test.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "Im.Test.pb.h"

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
namespace Test {

namespace {

const ::google::protobuf::Descriptor* Test_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Test_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_Im_2eTest_2eproto() {
  protobuf_AddDesc_Im_2eTest_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "Im.Test.proto");
  GOOGLE_CHECK(file != NULL);
  Test_descriptor_ = file->message_type(0);
  static const int Test_offsets_[3] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Test, id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Test, name_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Test, created_time_),
  };
  Test_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Test_descriptor_,
      Test::default_instance_,
      Test_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Test, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Test, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Test));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_Im_2eTest_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Test_descriptor_, &Test::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_Im_2eTest_2eproto() {
  delete Test::default_instance_;
  delete Test_reflection_;
}

void protobuf_AddDesc_Im_2eTest_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\rIm.Test.proto\022\007Im.Test\"6\n\004Test\022\n\n\002id\030\001"
    " \002(\005\022\014\n\004name\030\002 \003(\t\022\024\n\014created_time\030\003 \001(\r"
    "B)\n\'com.example.user.myapplication.proto"
    "buf", 123);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "Im.Test.proto", &protobuf_RegisterTypes);
  Test::default_instance_ = new Test();
  Test::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_Im_2eTest_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_Im_2eTest_2eproto {
  StaticDescriptorInitializer_Im_2eTest_2eproto() {
    protobuf_AddDesc_Im_2eTest_2eproto();
  }
} static_descriptor_initializer_Im_2eTest_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int Test::kIdFieldNumber;
const int Test::kNameFieldNumber;
const int Test::kCreatedTimeFieldNumber;
#endif  // !_MSC_VER

Test::Test()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Im.Test.Test)
}

void Test::InitAsDefaultInstance() {
}

Test::Test(const Test& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:Im.Test.Test)
}

void Test::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  id_ = 0;
  created_time_ = 0u;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Test::~Test() {
  // @@protoc_insertion_point(destructor:Im.Test.Test)
  SharedDtor();
}

void Test::SharedDtor() {
  if (this != default_instance_) {
  }
}

void Test::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Test::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Test_descriptor_;
}

const Test& Test::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_Im_2eTest_2eproto();
  return *default_instance_;
}

Test* Test::default_instance_ = NULL;

Test* Test::New() const {
  return new Test;
}

void Test::Clear() {
#define OFFSET_OF_FIELD_(f) (reinterpret_cast<char*>(      \
  &reinterpret_cast<Test*>(16)->f) - \
   reinterpret_cast<char*>(16))

#define ZR_(first, last) do {                              \
    size_t f = OFFSET_OF_FIELD_(first);                    \
    size_t n = OFFSET_OF_FIELD_(last) - f + sizeof(last);  \
    ::memset(&first, 0, n);                                \
  } while (0)

  ZR_(id_, created_time_);

#undef OFFSET_OF_FIELD_
#undef ZR_

  name_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Test::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:Im.Test.Test)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 id = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &id_)));
          set_has_id();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(18)) goto parse_name;
        break;
      }

      // repeated string name = 2;
      case 2: {
        if (tag == 18) {
         parse_name:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->add_name()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->name(this->name_size() - 1).data(),
            this->name(this->name_size() - 1).length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "name");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(18)) goto parse_name;
        if (input->ExpectTag(24)) goto parse_created_time;
        break;
      }

      // optional uint32 created_time = 3;
      case 3: {
        if (tag == 24) {
         parse_created_time:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &created_time_)));
          set_has_created_time();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:Im.Test.Test)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:Im.Test.Test)
  return false;
#undef DO_
}

void Test::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:Im.Test.Test)
  // required int32 id = 1;
  if (has_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->id(), output);
  }

  // repeated string name = 2;
  for (int i = 0; i < this->name_size(); i++) {
  ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
    this->name(i).data(), this->name(i).length(),
    ::google::protobuf::internal::WireFormat::SERIALIZE,
    "name");
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->name(i), output);
  }

  // optional uint32 created_time = 3;
  if (has_created_time()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(3, this->created_time(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:Im.Test.Test)
}

::google::protobuf::uint8* Test::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:Im.Test.Test)
  // required int32 id = 1;
  if (has_id()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->id(), target);
  }

  // repeated string name = 2;
  for (int i = 0; i < this->name_size(); i++) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->name(i).data(), this->name(i).length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "name");
    target = ::google::protobuf::internal::WireFormatLite::
      WriteStringToArray(2, this->name(i), target);
  }

  // optional uint32 created_time = 3;
  if (has_created_time()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(3, this->created_time(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Im.Test.Test)
  return target;
}

int Test::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required int32 id = 1;
    if (has_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->id());
    }

    // optional uint32 created_time = 3;
    if (has_created_time()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->created_time());
    }

  }
  // repeated string name = 2;
  total_size += 1 * this->name_size();
  for (int i = 0; i < this->name_size(); i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::StringSize(
      this->name(i));
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

void Test::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Test* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Test*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Test::MergeFrom(const Test& from) {
  GOOGLE_CHECK_NE(&from, this);
  name_.MergeFrom(from.name_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_id()) {
      set_id(from.id());
    }
    if (from.has_created_time()) {
      set_created_time(from.created_time());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Test::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Test::CopyFrom(const Test& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Test::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void Test::Swap(Test* other) {
  if (other != this) {
    std::swap(id_, other->id_);
    name_.Swap(&other->name_);
    std::swap(created_time_, other->created_time_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Test::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Test_descriptor_;
  metadata.reflection = Test_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace Test
}  // namespace Im

// @@protoc_insertion_point(global_scope)