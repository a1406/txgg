/* Generated by the protocol buffer compiler.  DO NOT EDIT! */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C_NO_DEPRECATED
#define PROTOBUF_C_NO_DEPRECATED
#endif

#include "config_attribute.pb-c.h"
void   cfg__attribute__init
                     (CFGATTRIBUTE         *message)
{
  static CFGATTRIBUTE init_value = CFG__ATTRIBUTE__INIT;
  *message = init_value;
}
size_t cfg__attribute__get_packed_size
                     (const CFGATTRIBUTE *message)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__attribute__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t cfg__attribute__pack
                     (const CFGATTRIBUTE *message,
                      uint8_t       *out)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__attribute__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t cfg__attribute__pack_to_buffer
                     (const CFGATTRIBUTE *message,
                      ProtobufCBuffer *buffer)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__attribute__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
CFGATTRIBUTE *
       cfg__attribute__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (CFGATTRIBUTE *)
     protobuf_c_message_unpack (&cfg__attribute__descriptor,
                                allocator, len, data);
}
void   cfg__attribute__free_unpacked
                     (CFGATTRIBUTE *message,
                      ProtobufCAllocator *allocator)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__attribute__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   cfg__attributes__init
                     (CFGATTRIBUTES         *message)
{
  static CFGATTRIBUTES init_value = CFG__ATTRIBUTES__INIT;
  *message = init_value;
}
size_t cfg__attributes__get_packed_size
                     (const CFGATTRIBUTES *message)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__attributes__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t cfg__attributes__pack
                     (const CFGATTRIBUTES *message,
                      uint8_t       *out)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__attributes__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t cfg__attributes__pack_to_buffer
                     (const CFGATTRIBUTES *message,
                      ProtobufCBuffer *buffer)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__attributes__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
CFGATTRIBUTES *
       cfg__attributes__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (CFGATTRIBUTES *)
     protobuf_c_message_unpack (&cfg__attributes__descriptor,
                                allocator, len, data);
}
void   cfg__attributes__free_unpacked
                     (CFGATTRIBUTES *message,
                      ProtobufCAllocator *allocator)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &cfg__attributes__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor cfg__attribute__field_descriptors[37] =
{
  {
    "id",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, id),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "hp",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, hp),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "hp_percent",
    3,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, hp_percent),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "strength",
    4,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, strength),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "brain",
    5,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, brain),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "charm",
    6,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, charm),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "agile",
    7,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, agile),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "phy_attack",
    8,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, phy_attack),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "phy_attack_percent",
    9,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, phy_attack_percent),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "magic_attack",
    10,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, magic_attack),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "magic_attack_percent",
    11,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, magic_attack_percent),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "phy_defence",
    12,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, phy_defence),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "phy_defence_percent",
    13,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, phy_defence_percent),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "magic_defence",
    14,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, magic_defence),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "magic_defence_percent",
    15,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, magic_defence_percent),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "hit_rate",
    16,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, hit_rate),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "hit_rate_percent",
    17,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, hit_rate_percent),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "jink_rate",
    18,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, jink_rate),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "jink_rate_percent",
    19,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, jink_rate_percent),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "parry_rate",
    20,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, parry_rate),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "parry_rate_percent",
    21,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, parry_rate_percent),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "countattack_rate",
    22,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, countattack_rate),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "countattack_rate_percent",
    23,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, countattack_rate_percent),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "crit_rate",
    24,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, crit_rate),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "crit_rate_percent",
    25,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, crit_rate_percent),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "speed",
    26,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, speed),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "speed_percent",
    27,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, speed_percent),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "move_speed",
    28,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, move_speed),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "move_speed_percent",
    29,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, move_speed_percent),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "fight_hit_percent",
    30,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, fight_hit_percent),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "fight_parry_percent",
    31,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, fight_parry_percent),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "fight_countattack_percent",
    32,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, fight_countattack_percent),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "fight_crit_percent",
    33,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, fight_crit_percent),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "restore_hp",
    34,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, restore_hp),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "restore_hp_percent",
    35,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, restore_hp_percent),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "restore_magic",
    36,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, restore_magic),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "restore_magic_percent",
    37,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTE, restore_magic_percent),
    NULL,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned cfg__attribute__field_indices_by_name[] = {
  6,   /* field[6] = agile */
  4,   /* field[4] = brain */
  5,   /* field[5] = charm */
  21,   /* field[21] = countattack_rate */
  22,   /* field[22] = countattack_rate_percent */
  23,   /* field[23] = crit_rate */
  24,   /* field[24] = crit_rate_percent */
  31,   /* field[31] = fight_countattack_percent */
  32,   /* field[32] = fight_crit_percent */
  29,   /* field[29] = fight_hit_percent */
  30,   /* field[30] = fight_parry_percent */
  15,   /* field[15] = hit_rate */
  16,   /* field[16] = hit_rate_percent */
  1,   /* field[1] = hp */
  2,   /* field[2] = hp_percent */
  0,   /* field[0] = id */
  17,   /* field[17] = jink_rate */
  18,   /* field[18] = jink_rate_percent */
  9,   /* field[9] = magic_attack */
  10,   /* field[10] = magic_attack_percent */
  13,   /* field[13] = magic_defence */
  14,   /* field[14] = magic_defence_percent */
  27,   /* field[27] = move_speed */
  28,   /* field[28] = move_speed_percent */
  19,   /* field[19] = parry_rate */
  20,   /* field[20] = parry_rate_percent */
  7,   /* field[7] = phy_attack */
  8,   /* field[8] = phy_attack_percent */
  11,   /* field[11] = phy_defence */
  12,   /* field[12] = phy_defence_percent */
  33,   /* field[33] = restore_hp */
  34,   /* field[34] = restore_hp_percent */
  35,   /* field[35] = restore_magic */
  36,   /* field[36] = restore_magic_percent */
  25,   /* field[25] = speed */
  26,   /* field[26] = speed_percent */
  3,   /* field[3] = strength */
};
static const ProtobufCIntRange cfg__attribute__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 37 }
};
const ProtobufCMessageDescriptor cfg__attribute__descriptor =
{
  PROTOBUF_C_MESSAGE_DESCRIPTOR_MAGIC,
  "CFG_ATTRIBUTE",
  "CFGATTRIBUTE",
  "CFGATTRIBUTE",
  "",
  sizeof(CFGATTRIBUTE),
  37,
  cfg__attribute__field_descriptors,
  cfg__attribute__field_indices_by_name,
  1,  cfg__attribute__number_ranges,
  (ProtobufCMessageInit) cfg__attribute__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor cfg__attributes__field_descriptors[1] =
{
  {
    "attribute",
    1,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTES, n_attribute),
    PROTOBUF_C_OFFSETOF(CFGATTRIBUTES, attribute),
    &cfg__attribute__descriptor,
    NULL,
    0,            /* packed */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned cfg__attributes__field_indices_by_name[] = {
  0,   /* field[0] = attribute */
};
static const ProtobufCIntRange cfg__attributes__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor cfg__attributes__descriptor =
{
  PROTOBUF_C_MESSAGE_DESCRIPTOR_MAGIC,
  "CFG_ATTRIBUTES",
  "CFGATTRIBUTES",
  "CFGATTRIBUTES",
  "",
  sizeof(CFGATTRIBUTES),
  1,
  cfg__attributes__field_descriptors,
  cfg__attributes__field_indices_by_name,
  1,  cfg__attributes__number_ranges,
  (ProtobufCMessageInit) cfg__attributes__init,
  NULL,NULL,NULL    /* reserved[123] */
};
