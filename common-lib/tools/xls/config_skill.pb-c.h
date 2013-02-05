/* Generated by the protocol buffer compiler.  DO NOT EDIT! */

#ifndef PROTOBUF_C_config_5fskill_2eproto__INCLUDED
#define PROTOBUF_C_config_5fskill_2eproto__INCLUDED

#include <google/protobuf-c/protobuf-c.h>

PROTOBUF_C_BEGIN_DECLS


typedef struct _CFGSKILL CFGSKILL;
typedef struct _CFGSKILLS CFGSKILLS;


/* --- enums --- */


/* --- messages --- */

struct  _CFGSKILL
{
  ProtobufCMessage base;
  int32_t id;
  int32_t type;
  char *name;
  int32_t level;
  char *desc;
  int32_t icon;
  int32_t action;
  int32_t effect;
  int32_t music;
  int32_t consume;
  int32_t learn_level;
  int32_t learn_npc;
  int32_t learn_friendly;
  int32_t learn_item_id;
  int32_t learn_item_num;
  int32_t learn_money;
  int32_t next_level;
  int32_t next_friendly;
  int32_t next_item_id;
  int32_t next_item_num;
  int32_t next_money;
  int32_t continue_time;
  int32_t script;
  int32_t hp_max;
  int32_t hp_max_percent;
  int32_t hp_cur;
  int32_t hp_cur_percent;
  int32_t phy_attack_percent;
  int32_t phy_attack;
  int32_t magic_attack_percent;
  int32_t magic_attack;
  int32_t phy_defence_percent;
  int32_t phy_defence;
  int32_t magic_defence_percent;
  int32_t magic_defence;
  int32_t charm;
  int32_t hit_rate;
  int32_t jink_rate;
  int32_t hit_rate_percent;
  int32_t countattack_rate;
  int32_t countattack_rate_percent;
  int32_t parry_rate;
  int32_t parry_rate_percent;
  int32_t crit_rate;
  int32_t crit_rate_percent;
  int32_t speed_rate;
  int32_t speed_percent;
};
#define CFG__SKILL__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&cfg__skill__descriptor) \
    , 0, 0, NULL, 0, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }


struct  _CFGSKILLS
{
  ProtobufCMessage base;
  size_t n_skill;
  CFGSKILL **skill;
};
#define CFG__SKILLS__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&cfg__skills__descriptor) \
    , 0,NULL }


/* CFGSKILL methods */
void   cfg__skill__init
                     (CFGSKILL         *message);
size_t cfg__skill__get_packed_size
                     (const CFGSKILL   *message);
size_t cfg__skill__pack
                     (const CFGSKILL   *message,
                      uint8_t             *out);
size_t cfg__skill__pack_to_buffer
                     (const CFGSKILL   *message,
                      ProtobufCBuffer     *buffer);
CFGSKILL *
       cfg__skill__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   cfg__skill__free_unpacked
                     (CFGSKILL *message,
                      ProtobufCAllocator *allocator);
/* CFGSKILLS methods */
void   cfg__skills__init
                     (CFGSKILLS         *message);
size_t cfg__skills__get_packed_size
                     (const CFGSKILLS   *message);
size_t cfg__skills__pack
                     (const CFGSKILLS   *message,
                      uint8_t             *out);
size_t cfg__skills__pack_to_buffer
                     (const CFGSKILLS   *message,
                      ProtobufCBuffer     *buffer);
CFGSKILLS *
       cfg__skills__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   cfg__skills__free_unpacked
                     (CFGSKILLS *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*CFGSKILL_Closure)
                 (const CFGSKILL *message,
                  void *closure_data);
typedef void (*CFGSKILLS_Closure)
                 (const CFGSKILLS *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor cfg__skill__descriptor;
extern const ProtobufCMessageDescriptor cfg__skills__descriptor;

PROTOBUF_C_END_DECLS


#endif  /* PROTOBUF_config_5fskill_2eproto__INCLUDED */
