// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Im.SwitchService.proto

#ifndef PROTOBUF_Im_2eSwitchService_2eproto__INCLUDED
#define PROTOBUF_Im_2eSwitchService_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace Im {
namespace SwitchService {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_Im_2eSwitchService_2eproto();
void protobuf_AssignDesc_Im_2eSwitchService_2eproto();
void protobuf_ShutdownFile_Im_2eSwitchService_2eproto();

class P2PCommand;

// ===================================================================

class P2PCommand : public ::google::protobuf::Message {
 public:
  P2PCommand();
  virtual ~P2PCommand();

  P2PCommand(const P2PCommand& from);

  inline P2PCommand& operator=(const P2PCommand& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const P2PCommand& default_instance();

  void Swap(P2PCommand* other);

  // implements Message ----------------------------------------------

  P2PCommand* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const P2PCommand& from);
  void MergeFrom(const P2PCommand& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required uint32 from_user_id = 1;
  inline bool has_from_user_id() const;
  inline void clear_from_user_id();
  static const int kFromUserIdFieldNumber = 1;
  inline ::google::protobuf::uint32 from_user_id() const;
  inline void set_from_user_id(::google::protobuf::uint32 value);

  // required uint32 to_user_id = 2;
  inline bool has_to_user_id() const;
  inline void clear_to_user_id();
  static const int kToUserIdFieldNumber = 2;
  inline ::google::protobuf::uint32 to_user_id() const;
  inline void set_to_user_id(::google::protobuf::uint32 value);

  // required string cmd_msg_data = 3;
  inline bool has_cmd_msg_data() const;
  inline void clear_cmd_msg_data();
  static const int kCmdMsgDataFieldNumber = 3;
  inline const ::std::string& cmd_msg_data() const;
  inline void set_cmd_msg_data(const ::std::string& value);
  inline void set_cmd_msg_data(const char* value);
  inline void set_cmd_msg_data(const char* value, size_t size);
  inline ::std::string* mutable_cmd_msg_data();
  inline ::std::string* release_cmd_msg_data();
  inline void set_allocated_cmd_msg_data(::std::string* cmd_msg_data);

  // @@protoc_insertion_point(class_scope:Im.SwitchService.P2PCommand)
 private:
  inline void set_has_from_user_id();
  inline void clear_has_from_user_id();
  inline void set_has_to_user_id();
  inline void clear_has_to_user_id();
  inline void set_has_cmd_msg_data();
  inline void clear_has_cmd_msg_data();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint32 from_user_id_;
  ::google::protobuf::uint32 to_user_id_;
  ::std::string* cmd_msg_data_;
  friend void  protobuf_AddDesc_Im_2eSwitchService_2eproto();
  friend void protobuf_AssignDesc_Im_2eSwitchService_2eproto();
  friend void protobuf_ShutdownFile_Im_2eSwitchService_2eproto();

  void InitAsDefaultInstance();
  static P2PCommand* default_instance_;
};
// ===================================================================


// ===================================================================

// P2PCommand

// required uint32 from_user_id = 1;
inline bool P2PCommand::has_from_user_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void P2PCommand::set_has_from_user_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void P2PCommand::clear_has_from_user_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void P2PCommand::clear_from_user_id() {
  from_user_id_ = 0u;
  clear_has_from_user_id();
}
inline ::google::protobuf::uint32 P2PCommand::from_user_id() const {
  // @@protoc_insertion_point(field_get:Im.SwitchService.P2PCommand.from_user_id)
  return from_user_id_;
}
inline void P2PCommand::set_from_user_id(::google::protobuf::uint32 value) {
  set_has_from_user_id();
  from_user_id_ = value;
  // @@protoc_insertion_point(field_set:Im.SwitchService.P2PCommand.from_user_id)
}

// required uint32 to_user_id = 2;
inline bool P2PCommand::has_to_user_id() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void P2PCommand::set_has_to_user_id() {
  _has_bits_[0] |= 0x00000002u;
}
inline void P2PCommand::clear_has_to_user_id() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void P2PCommand::clear_to_user_id() {
  to_user_id_ = 0u;
  clear_has_to_user_id();
}
inline ::google::protobuf::uint32 P2PCommand::to_user_id() const {
  // @@protoc_insertion_point(field_get:Im.SwitchService.P2PCommand.to_user_id)
  return to_user_id_;
}
inline void P2PCommand::set_to_user_id(::google::protobuf::uint32 value) {
  set_has_to_user_id();
  to_user_id_ = value;
  // @@protoc_insertion_point(field_set:Im.SwitchService.P2PCommand.to_user_id)
}

// required string cmd_msg_data = 3;
inline bool P2PCommand::has_cmd_msg_data() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void P2PCommand::set_has_cmd_msg_data() {
  _has_bits_[0] |= 0x00000004u;
}
inline void P2PCommand::clear_has_cmd_msg_data() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void P2PCommand::clear_cmd_msg_data() {
  if (cmd_msg_data_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    cmd_msg_data_->clear();
  }
  clear_has_cmd_msg_data();
}
inline const ::std::string& P2PCommand::cmd_msg_data() const {
  // @@protoc_insertion_point(field_get:Im.SwitchService.P2PCommand.cmd_msg_data)
  return *cmd_msg_data_;
}
inline void P2PCommand::set_cmd_msg_data(const ::std::string& value) {
  set_has_cmd_msg_data();
  if (cmd_msg_data_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    cmd_msg_data_ = new ::std::string;
  }
  cmd_msg_data_->assign(value);
  // @@protoc_insertion_point(field_set:Im.SwitchService.P2PCommand.cmd_msg_data)
}
inline void P2PCommand::set_cmd_msg_data(const char* value) {
  set_has_cmd_msg_data();
  if (cmd_msg_data_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    cmd_msg_data_ = new ::std::string;
  }
  cmd_msg_data_->assign(value);
  // @@protoc_insertion_point(field_set_char:Im.SwitchService.P2PCommand.cmd_msg_data)
}
inline void P2PCommand::set_cmd_msg_data(const char* value, size_t size) {
  set_has_cmd_msg_data();
  if (cmd_msg_data_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    cmd_msg_data_ = new ::std::string;
  }
  cmd_msg_data_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:Im.SwitchService.P2PCommand.cmd_msg_data)
}
inline ::std::string* P2PCommand::mutable_cmd_msg_data() {
  set_has_cmd_msg_data();
  if (cmd_msg_data_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    cmd_msg_data_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:Im.SwitchService.P2PCommand.cmd_msg_data)
  return cmd_msg_data_;
}
inline ::std::string* P2PCommand::release_cmd_msg_data() {
  clear_has_cmd_msg_data();
  if (cmd_msg_data_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = cmd_msg_data_;
    cmd_msg_data_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void P2PCommand::set_allocated_cmd_msg_data(::std::string* cmd_msg_data) {
  if (cmd_msg_data_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete cmd_msg_data_;
  }
  if (cmd_msg_data) {
    set_has_cmd_msg_data();
    cmd_msg_data_ = cmd_msg_data;
  } else {
    clear_has_cmd_msg_data();
    cmd_msg_data_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:Im.SwitchService.P2PCommand.cmd_msg_data)
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace SwitchService
}  // namespace Im

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_Im_2eSwitchService_2eproto__INCLUDED