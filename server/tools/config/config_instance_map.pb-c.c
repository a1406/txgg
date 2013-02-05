/* Generated by the protocol buffer compiler.  DO NOT EDIT! */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C_NO_DEPRECATED
#define PROTOBUF_C_NO_DEPRECATED
#endif

#include "config_instance_map.pb-c.h"
void   cfg__instance__map__node__init
                     (CFGINSTANCEMAPNODE         *message)
{
  static CFGINSTANCEMAPNODE init_value = CFG__INSTANCE__MAP__NODE__INIT;
  *message = init_value;
}
size_t cfg__instance__map__node__get_packed_size
                     (const CFGINSTANCEMAPNODE *message)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__instance__map__node__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t cfg__instance__map__node__pack
                     (const CFGINSTANCEMAPNODE *message,
                      uint8_t       *out)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__instance__map__node__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t cfg__instance__map__node__pack_to_buffer
                     (const CFGINSTANCEMAPNODE *message,
                      ProtobufCBuffer *buffer)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__instance__map__node__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
CFGINSTANCEMAPNODE *
       cfg__instance__map__node__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (CFGINSTANCEMAPNODE *)
     protobuf_c_message_unpack (&cfg__instance__map__node__descriptor,
                                allocator, len, data);
}
void   cfg__instance__map__node__free_unpacked
                     (CFGINSTANCEMAPNODE *message,
                      ProtobufCAllocator *allocator)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__instance__map__node__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   cfg__instance__map__init
                     (CFGINSTANCEMAP         *message)
{
  static CFGINSTANCEMAP init_value = CFG__INSTANCE__MAP__INIT;
  *message = init_value;
}
size_t cfg__instance__map__get_packed_size
                     (const CFGINSTANCEMAP *message)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__instance__map__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t cfg__instance__map__pack
                     (const CFGINSTANCEMAP *message,
                      uint8_t       *out)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__instance__map__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t cfg__instance__map__pack_to_buffer
                     (const CFGINSTANCEMAP *message,
                      ProtobufCBuffer *buffer)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__instance__map__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
CFGINSTANCEMAP *
       cfg__instance__map__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (CFGINSTANCEMAP *)
     protobuf_c_message_unpack (&cfg__instance__map__descriptor,
                                allocator, len, data);
}
void   cfg__instance__map__free_unpacked
                     (CFGINSTANCEMAP *message,
                      ProtobufCAllocator *allocator)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__instance__map__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor cfg__instance__map__node__field_descriptors[7] =
{
  {
    "id",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGINSTANCEMAPNODE, id),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "action",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGINSTANCEMAPNODE, action),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "up_pos",
    3,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGINSTANCEMAPNODE, up_pos),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "left_pos",
    4,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGINSTANCEMAPNODE, left_pos),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "down_pos",
    5,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGINSTANCEMAPNODE, down_pos),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "right_pos",
    6,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGINSTANCEMAPNODE, right_pos),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "seq_id",
    7,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGINSTANCEMAPNODE, seq_id),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned cfg__instance__map__node__field_indices_by_name[] = {
  1,   /* field[1] = action */
  4,   /* field[4] = down_pos */
  0,   /* field[0] = id */
  3,   /* field[3] = left_pos */
  5,   /* field[5] = right_pos */
  6,   /* field[6] = seq_id */
  2,   /* field[2] = up_pos */
};
static const ProtobufCIntRange cfg__instance__map__node__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 7 }
};
const ProtobufCMessageDescriptor cfg__instance__map__node__descriptor =
{
  PROTOBUF_C_MESSAGE_DESCRIPTOR_MAGIC,
  "CFG_INSTANCE_MAP_NODE",
  "CFGINSTANCEMAPNODE",
  "CFGINSTANCEMAPNODE",
  "",
  sizeof(CFGINSTANCEMAPNODE),
  7,
  cfg__instance__map__node__field_descriptors,
  cfg__instance__map__node__field_indices_by_name,
  1,  cfg__instance__map__node__number_ranges,
  (ProtobufCMessageInit) cfg__instance__map__node__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor cfg__instance__map__field_descriptors[1] =
{
  {
    "node",
    1,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    PROTOBUF_C_OFFSETOF(CFGINSTANCEMAP, n_node),
    PROTOBUF_C_OFFSETOF(CFGINSTANCEMAP, node),
    &cfg__instance__map__node__descriptor,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned cfg__instance__map__field_indices_by_name[] = {
  0,   /* field[0] = node */
};
static const ProtobufCIntRange cfg__instance__map__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor cfg__instance__map__descriptor =
{
  PROTOBUF_C_MESSAGE_DESCRIPTOR_MAGIC,
  "CFG_INSTANCE_MAP",
  "CFGINSTANCEMAP",
  "CFGINSTANCEMAP",
  "",
  sizeof(CFGINSTANCEMAP),
  1,
  cfg__instance__map__field_descriptors,
  cfg__instance__map__field_indices_by_name,
  1,  cfg__instance__map__number_ranges,
  (ProtobufCMessageInit) cfg__instance__map__init,
  NULL,NULL,NULL    /* reserved[123] */
};
