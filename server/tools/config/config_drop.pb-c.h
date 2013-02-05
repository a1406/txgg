/* Generated by the protocol buffer compiler.  DO NOT EDIT! */

#ifndef PROTOBUF_C_config_5fdrop_2eproto__INCLUDED
#define PROTOBUF_C_config_5fdrop_2eproto__INCLUDED

#include <google/protobuf-c/protobuf-c.h>

PROTOBUF_C_BEGIN_DECLS


typedef struct _DropItem DropItem;
typedef struct _CFGDROP CFGDROP;
typedef struct _CFGDROPS CFGDROPS;


/* --- enums --- */


/* --- messages --- */

struct  _DropItem
{
  ProtobufCMessage base;
  int32_t id;
  int32_t num;
  int32_t rate;
};
#define DROP_ITEM__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&drop_item__descriptor) \
    , 0, 0, 0 }


struct  _CFGDROP
{
  ProtobufCMessage base;
  int32_t id;
  int32_t exp;
  int32_t money;
  int32_t gold;
  int32_t gold_rate;
  int32_t exclude;
  size_t n_item;
  DropItem **item;
};
#define CFG__DROP__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&cfg__drop__descriptor) \
    , 0, 0, 0, 0, 0, 0, 0,NULL }


struct  _CFGDROPS
{
  ProtobufCMessage base;
  size_t n_drop;
  CFGDROP **drop;
};
#define CFG__DROPS__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&cfg__drops__descriptor) \
    , 0,NULL }


/* DropItem methods */
void   drop_item__init
                     (DropItem         *message);
size_t drop_item__get_packed_size
                     (const DropItem   *message);
size_t drop_item__pack
                     (const DropItem   *message,
                      uint8_t             *out);
size_t drop_item__pack_to_buffer
                     (const DropItem   *message,
                      ProtobufCBuffer     *buffer);
DropItem *
       drop_item__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   drop_item__free_unpacked
                     (DropItem *message,
                      ProtobufCAllocator *allocator);
/* CFGDROP methods */
void   cfg__drop__init
                     (CFGDROP         *message);
size_t cfg__drop__get_packed_size
                     (const CFGDROP   *message);
size_t cfg__drop__pack
                     (const CFGDROP   *message,
                      uint8_t             *out);
size_t cfg__drop__pack_to_buffer
                     (const CFGDROP   *message,
                      ProtobufCBuffer     *buffer);
CFGDROP *
       cfg__drop__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   cfg__drop__free_unpacked
                     (CFGDROP *message,
                      ProtobufCAllocator *allocator);
/* CFGDROPS methods */
void   cfg__drops__init
                     (CFGDROPS         *message);
size_t cfg__drops__get_packed_size
                     (const CFGDROPS   *message);
size_t cfg__drops__pack
                     (const CFGDROPS   *message,
                      uint8_t             *out);
size_t cfg__drops__pack_to_buffer
                     (const CFGDROPS   *message,
                      ProtobufCBuffer     *buffer);
CFGDROPS *
       cfg__drops__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   cfg__drops__free_unpacked
                     (CFGDROPS *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*DropItem_Closure)
                 (const DropItem *message,
                  void *closure_data);
typedef void (*CFGDROP_Closure)
                 (const CFGDROP *message,
                  void *closure_data);
typedef void (*CFGDROPS_Closure)
                 (const CFGDROPS *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor drop_item__descriptor;
extern const ProtobufCMessageDescriptor cfg__drop__descriptor;
extern const ProtobufCMessageDescriptor cfg__drops__descriptor;

PROTOBUF_C_END_DECLS


#endif  /* PROTOBUF_config_5fdrop_2eproto__INCLUDED */