/* Generated by the protocol buffer compiler.  DO NOT EDIT! */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C_NO_DEPRECATED
#define PROTOBUF_C_NO_DEPRECATED
#endif

#include "config_keyword.pb-c.h"
void   cfg__keyword__init
                     (CFGKEYWORD         *message)
{
  static CFGKEYWORD init_value = CFG__KEYWORD__INIT;
  *message = init_value;
}
size_t cfg__keyword__get_packed_size
                     (const CFGKEYWORD *message)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__keyword__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t cfg__keyword__pack
                     (const CFGKEYWORD *message,
                      uint8_t       *out)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__keyword__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t cfg__keyword__pack_to_buffer
                     (const CFGKEYWORD *message,
                      ProtobufCBuffer *buffer)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__keyword__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
CFGKEYWORD *
       cfg__keyword__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (CFGKEYWORD *)
     protobuf_c_message_unpack (&cfg__keyword__descriptor,
                                allocator, len, data);
}
void   cfg__keyword__free_unpacked
                     (CFGKEYWORD *message,
                      ProtobufCAllocator *allocator)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__keyword__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   cfg__keywords__init
                     (CFGKEYWORDS         *message)
{
  static CFGKEYWORDS init_value = CFG__KEYWORDS__INIT;
  *message = init_value;
}
size_t cfg__keywords__get_packed_size
                     (const CFGKEYWORDS *message)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__keywords__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t cfg__keywords__pack
                     (const CFGKEYWORDS *message,
                      uint8_t       *out)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__keywords__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t cfg__keywords__pack_to_buffer
                     (const CFGKEYWORDS *message,
                      ProtobufCBuffer *buffer)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__keywords__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
CFGKEYWORDS *
       cfg__keywords__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (CFGKEYWORDS *)
     protobuf_c_message_unpack (&cfg__keywords__descriptor,
                                allocator, len, data);
}
void   cfg__keywords__free_unpacked
                     (CFGKEYWORDS *message,
                      ProtobufCAllocator *allocator)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__keywords__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor cfg__keyword__field_descriptors[3] =
{
  {
    "group",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGKEYWORD, group),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "value",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGKEYWORD, value),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "desc",
    3,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGKEYWORD, desc),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned cfg__keyword__field_indices_by_name[] = {
  2,   /* field[2] = desc */
  0,   /* field[0] = group */
  1,   /* field[1] = value */
};
static const ProtobufCIntRange cfg__keyword__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor cfg__keyword__descriptor =
{
  PROTOBUF_C_MESSAGE_DESCRIPTOR_MAGIC,
  "CFG_KEYWORD",
  "CFGKEYWORD",
  "CFGKEYWORD",
  "",
  sizeof(CFGKEYWORD),
  3,
  cfg__keyword__field_descriptors,
  cfg__keyword__field_indices_by_name,
  1,  cfg__keyword__number_ranges,
  (ProtobufCMessageInit) cfg__keyword__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor cfg__keywords__field_descriptors[1] =
{
  {
    "keyword",
    1,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    PROTOBUF_C_OFFSETOF(CFGKEYWORDS, n_keyword),
    PROTOBUF_C_OFFSETOF(CFGKEYWORDS, keyword),
    &cfg__keyword__descriptor,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned cfg__keywords__field_indices_by_name[] = {
  0,   /* field[0] = keyword */
};
static const ProtobufCIntRange cfg__keywords__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor cfg__keywords__descriptor =
{
  PROTOBUF_C_MESSAGE_DESCRIPTOR_MAGIC,
  "CFG_KEYWORDS",
  "CFGKEYWORDS",
  "CFGKEYWORDS",
  "",
  sizeof(CFGKEYWORDS),
  1,
  cfg__keywords__field_descriptors,
  cfg__keywords__field_indices_by_name,
  1,  cfg__keywords__number_ranges,
  (ProtobufCMessageInit) cfg__keywords__init,
  NULL,NULL,NULL    /* reserved[123] */
};