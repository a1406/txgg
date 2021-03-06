/* Generated by the protocol buffer compiler.  DO NOT EDIT! */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C_NO_DEPRECATED
#define PROTOBUF_C_NO_DEPRECATED
#endif

#include "config_drop.pb-c.h"
void   drop_item__init
                     (DropItem         *message)
{
  static DropItem init_value = DROP_ITEM__INIT;
  *message = init_value;
}
size_t drop_item__get_packed_size
                     (const DropItem *message)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &drop_item__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t drop_item__pack
                     (const DropItem *message,
                      uint8_t       *out)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &drop_item__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t drop_item__pack_to_buffer
                     (const DropItem *message,
                      ProtobufCBuffer *buffer)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &drop_item__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
DropItem *
       drop_item__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (DropItem *)
     protobuf_c_message_unpack (&drop_item__descriptor,
                                allocator, len, data);
}
void   drop_item__free_unpacked
                     (DropItem *message,
                      ProtobufCAllocator *allocator)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &drop_item__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   cfg__drop__init
                     (CFGDROP         *message)
{
  static CFGDROP init_value = CFG__DROP__INIT;
  *message = init_value;
}
size_t cfg__drop__get_packed_size
                     (const CFGDROP *message)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__drop__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t cfg__drop__pack
                     (const CFGDROP *message,
                      uint8_t       *out)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__drop__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t cfg__drop__pack_to_buffer
                     (const CFGDROP *message,
                      ProtobufCBuffer *buffer)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__drop__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
CFGDROP *
       cfg__drop__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (CFGDROP *)
     protobuf_c_message_unpack (&cfg__drop__descriptor,
                                allocator, len, data);
}
void   cfg__drop__free_unpacked
                     (CFGDROP *message,
                      ProtobufCAllocator *allocator)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__drop__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   cfg__drops__init
                     (CFGDROPS         *message)
{
  static CFGDROPS init_value = CFG__DROPS__INIT;
  *message = init_value;
}
size_t cfg__drops__get_packed_size
                     (const CFGDROPS *message)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__drops__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t cfg__drops__pack
                     (const CFGDROPS *message,
                      uint8_t       *out)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__drops__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t cfg__drops__pack_to_buffer
                     (const CFGDROPS *message,
                      ProtobufCBuffer *buffer)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__drops__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
CFGDROPS *
       cfg__drops__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (CFGDROPS *)
     protobuf_c_message_unpack (&cfg__drops__descriptor,
                                allocator, len, data);
}
void   cfg__drops__free_unpacked
                     (CFGDROPS *message,
                      ProtobufCAllocator *allocator)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__drops__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor drop_item__field_descriptors[3] =
{
  {
    "id",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(DropItem, id),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "num",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(DropItem, num),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "rate",
    3,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(DropItem, rate),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned drop_item__field_indices_by_name[] = {
  0,   /* field[0] = id */
  1,   /* field[1] = num */
  2,   /* field[2] = rate */
};
static const ProtobufCIntRange drop_item__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor drop_item__descriptor =
{
  PROTOBUF_C_MESSAGE_DESCRIPTOR_MAGIC,
  "drop_item",
  "DropItem",
  "DropItem",
  "",
  sizeof(DropItem),
  3,
  drop_item__field_descriptors,
  drop_item__field_indices_by_name,
  1,  drop_item__number_ranges,
  (ProtobufCMessageInit) drop_item__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor cfg__drop__field_descriptors[7] =
{
  {
    "id",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGDROP, id),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "exp",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGDROP, exp),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "money",
    3,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGDROP, money),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "energy",
    4,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGDROP, energy),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "energy_rate",
    5,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGDROP, energy_rate),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "exclude",
    6,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGDROP, exclude),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "item",
    7,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    PROTOBUF_C_OFFSETOF(CFGDROP, n_item),
    PROTOBUF_C_OFFSETOF(CFGDROP, item),
    &drop_item__descriptor,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned cfg__drop__field_indices_by_name[] = {
  3,   /* field[3] = energy */
  4,   /* field[4] = energy_rate */
  5,   /* field[5] = exclude */
  1,   /* field[1] = exp */
  0,   /* field[0] = id */
  6,   /* field[6] = item */
  2,   /* field[2] = money */
};
static const ProtobufCIntRange cfg__drop__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 7 }
};
const ProtobufCMessageDescriptor cfg__drop__descriptor =
{
  PROTOBUF_C_MESSAGE_DESCRIPTOR_MAGIC,
  "CFG_DROP",
  "CFGDROP",
  "CFGDROP",
  "",
  sizeof(CFGDROP),
  7,
  cfg__drop__field_descriptors,
  cfg__drop__field_indices_by_name,
  1,  cfg__drop__number_ranges,
  (ProtobufCMessageInit) cfg__drop__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor cfg__drops__field_descriptors[1] =
{
  {
    "drop",
    1,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    PROTOBUF_C_OFFSETOF(CFGDROPS, n_drop),
    PROTOBUF_C_OFFSETOF(CFGDROPS, drop),
    &cfg__drop__descriptor,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned cfg__drops__field_indices_by_name[] = {
  0,   /* field[0] = drop */
};
static const ProtobufCIntRange cfg__drops__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor cfg__drops__descriptor =
{
  PROTOBUF_C_MESSAGE_DESCRIPTOR_MAGIC,
  "CFG_DROPS",
  "CFGDROPS",
  "CFGDROPS",
  "",
  sizeof(CFGDROPS),
  1,
  cfg__drops__field_descriptors,
  cfg__drops__field_indices_by_name,
  1,  cfg__drops__number_ranges,
  (ProtobufCMessageInit) cfg__drops__init,
  NULL,NULL,NULL    /* reserved[123] */
};
