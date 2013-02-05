/* Generated by the protocol buffer compiler.  DO NOT EDIT! */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C_NO_DEPRECATED
#define PROTOBUF_C_NO_DEPRECATED
#endif

#include "config_player.pb-c.h"
void   cfg__player__init
                     (CFGPLAYER         *message)
{
  static CFGPLAYER init_value = CFG__PLAYER__INIT;
  *message = init_value;
}
size_t cfg__player__get_packed_size
                     (const CFGPLAYER *message)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__player__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t cfg__player__pack
                     (const CFGPLAYER *message,
                      uint8_t       *out)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__player__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t cfg__player__pack_to_buffer
                     (const CFGPLAYER *message,
                      ProtobufCBuffer *buffer)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__player__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
CFGPLAYER *
       cfg__player__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (CFGPLAYER *)
     protobuf_c_message_unpack (&cfg__player__descriptor,
                                allocator, len, data);
}
void   cfg__player__free_unpacked
                     (CFGPLAYER *message,
                      ProtobufCAllocator *allocator)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__player__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   cfg__players__init
                     (CFGPLAYERS         *message)
{
  static CFGPLAYERS init_value = CFG__PLAYERS__INIT;
  *message = init_value;
}
size_t cfg__players__get_packed_size
                     (const CFGPLAYERS *message)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__players__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t cfg__players__pack
                     (const CFGPLAYERS *message,
                      uint8_t       *out)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__players__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t cfg__players__pack_to_buffer
                     (const CFGPLAYERS *message,
                      ProtobufCBuffer *buffer)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__players__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
CFGPLAYERS *
       cfg__players__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (CFGPLAYERS *)
     protobuf_c_message_unpack (&cfg__players__descriptor,
                                allocator, len, data);
}
void   cfg__players__free_unpacked
                     (CFGPLAYERS *message,
                      ProtobufCAllocator *allocator)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__players__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor cfg__player__field_descriptors[5] =
{
  {
    "prof",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGPLAYER, prof),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "level",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGPLAYER, level),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "sex",
    3,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGPLAYER, sex),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "exp",
    4,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGPLAYER, exp),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "attribute",
    5,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGPLAYER, attribute),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned cfg__player__field_indices_by_name[] = {
  4,   /* field[4] = attribute */
  3,   /* field[3] = exp */
  1,   /* field[1] = level */
  0,   /* field[0] = prof */
  2,   /* field[2] = sex */
};
static const ProtobufCIntRange cfg__player__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 5 }
};
const ProtobufCMessageDescriptor cfg__player__descriptor =
{
  PROTOBUF_C_MESSAGE_DESCRIPTOR_MAGIC,
  "CFG_PLAYER",
  "CFGPLAYER",
  "CFGPLAYER",
  "",
  sizeof(CFGPLAYER),
  5,
  cfg__player__field_descriptors,
  cfg__player__field_indices_by_name,
  1,  cfg__player__number_ranges,
  (ProtobufCMessageInit) cfg__player__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor cfg__players__field_descriptors[1] =
{
  {
    "player",
    1,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    PROTOBUF_C_OFFSETOF(CFGPLAYERS, n_player),
    PROTOBUF_C_OFFSETOF(CFGPLAYERS, player),
    &cfg__player__descriptor,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned cfg__players__field_indices_by_name[] = {
  0,   /* field[0] = player */
};
static const ProtobufCIntRange cfg__players__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor cfg__players__descriptor =
{
  PROTOBUF_C_MESSAGE_DESCRIPTOR_MAGIC,
  "CFG_PLAYERS",
  "CFGPLAYERS",
  "CFGPLAYERS",
  "",
  sizeof(CFGPLAYERS),
  1,
  cfg__players__field_descriptors,
  cfg__players__field_indices_by_name,
  1,  cfg__players__number_ranges,
  (ProtobufCMessageInit) cfg__players__init,
  NULL,NULL,NULL    /* reserved[123] */
};
