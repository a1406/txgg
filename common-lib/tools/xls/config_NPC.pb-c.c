/* Generated by the protocol buffer compiler.  DO NOT EDIT! */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C_NO_DEPRECATED
#define PROTOBUF_C_NO_DEPRECATED
#endif

#include "config_NPC.pb-c.h"
void   cfg__npc__init
                     (CFGNPC         *message)
{
  static CFGNPC init_value = CFG__NPC__INIT;
  *message = init_value;
}
size_t cfg__npc__get_packed_size
                     (const CFGNPC *message)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__npc__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t cfg__npc__pack
                     (const CFGNPC *message,
                      uint8_t       *out)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__npc__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t cfg__npc__pack_to_buffer
                     (const CFGNPC *message,
                      ProtobufCBuffer *buffer)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__npc__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
CFGNPC *
       cfg__npc__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (CFGNPC *)
     protobuf_c_message_unpack (&cfg__npc__descriptor,
                                allocator, len, data);
}
void   cfg__npc__free_unpacked
                     (CFGNPC *message,
                      ProtobufCAllocator *allocator)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__npc__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   cfg__npcs__init
                     (CFGNPCS         *message)
{
  static CFGNPCS init_value = CFG__NPCS__INIT;
  *message = init_value;
}
size_t cfg__npcs__get_packed_size
                     (const CFGNPCS *message)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__npcs__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t cfg__npcs__pack
                     (const CFGNPCS *message,
                      uint8_t       *out)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__npcs__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t cfg__npcs__pack_to_buffer
                     (const CFGNPCS *message,
                      ProtobufCBuffer *buffer)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__npcs__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
CFGNPCS *
       cfg__npcs__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (CFGNPCS *)
     protobuf_c_message_unpack (&cfg__npcs__descriptor,
                                allocator, len, data);
}
void   cfg__npcs__free_unpacked
                     (CFGNPCS *message,
                      ProtobufCAllocator *allocator)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__npcs__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor cfg__npc__field_descriptors[3] =
{
  {
    "id",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGNPC, id),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "name",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGNPC, name),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "func",
    3,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_INT32,
    PROTOBUF_C_OFFSETOF(CFGNPC, n_func),
    PROTOBUF_C_OFFSETOF(CFGNPC, func),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned cfg__npc__field_indices_by_name[] = {
  2,   /* field[2] = func */
  0,   /* field[0] = id */
  1,   /* field[1] = name */
};
static const ProtobufCIntRange cfg__npc__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor cfg__npc__descriptor =
{
  PROTOBUF_C_MESSAGE_DESCRIPTOR_MAGIC,
  "CFG_NPC",
  "CFGNPC",
  "CFGNPC",
  "",
  sizeof(CFGNPC),
  3,
  cfg__npc__field_descriptors,
  cfg__npc__field_indices_by_name,
  1,  cfg__npc__number_ranges,
  (ProtobufCMessageInit) cfg__npc__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor cfg__npcs__field_descriptors[1] =
{
  {
    "npc",
    1,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    PROTOBUF_C_OFFSETOF(CFGNPCS, n_npc),
    PROTOBUF_C_OFFSETOF(CFGNPCS, npc),
    &cfg__npc__descriptor,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned cfg__npcs__field_indices_by_name[] = {
  0,   /* field[0] = npc */
};
static const ProtobufCIntRange cfg__npcs__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor cfg__npcs__descriptor =
{
  PROTOBUF_C_MESSAGE_DESCRIPTOR_MAGIC,
  "CFG_NPCS",
  "CFGNPCS",
  "CFGNPCS",
  "",
  sizeof(CFGNPCS),
  1,
  cfg__npcs__field_descriptors,
  cfg__npcs__field_indices_by_name,
  1,  cfg__npcs__number_ranges,
  (ProtobufCMessageInit) cfg__npcs__init,
  NULL,NULL,NULL    /* reserved[123] */
};
