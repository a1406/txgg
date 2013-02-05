/* Generated by the protocol buffer compiler.  DO NOT EDIT! */

#ifndef PROTOBUF_C_scene_2eproto__INCLUDED
#define PROTOBUF_C_scene_2eproto__INCLUDED

#include <google/protobuf-c/protobuf-c.h>

PROTOBUF_C_BEGIN_DECLS


typedef struct _TestMsg TestMsg;
typedef struct _SCENE SCENE;
typedef struct _SCENES SCENES;


/* --- enums --- */


/* --- messages --- */

struct  _TestMsg
{
  ProtobufCMessage base;
  int32_t pos_x;
  int32_t pos_y;
};
#define TEST_MSG__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&test_msg__descriptor) \
    , 0, 0 }


struct  _SCENE
{
  ProtobufCMessage base;
  int32_t id;
  char *name;
  int32_t width;
  int32_t height;
  int32_t valid_height;
  size_t n_msg;
  TestMsg **msg;
};
#define SCENE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&scene__descriptor) \
    , 0, NULL, 0, 0, 0, 0,NULL }


struct  _SCENES
{
  ProtobufCMessage base;
  size_t n_scene;
  SCENE **scene;
};
#define SCENES__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&scenes__descriptor) \
    , 0,NULL }


/* TestMsg methods */
void   test_msg__init
                     (TestMsg         *message);
size_t test_msg__get_packed_size
                     (const TestMsg   *message);
size_t test_msg__pack
                     (const TestMsg   *message,
                      uint8_t             *out);
size_t test_msg__pack_to_buffer
                     (const TestMsg   *message,
                      ProtobufCBuffer     *buffer);
TestMsg *
       test_msg__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   test_msg__free_unpacked
                     (TestMsg *message,
                      ProtobufCAllocator *allocator);
/* SCENE methods */
void   scene__init
                     (SCENE         *message);
size_t scene__get_packed_size
                     (const SCENE   *message);
size_t scene__pack
                     (const SCENE   *message,
                      uint8_t             *out);
size_t scene__pack_to_buffer
                     (const SCENE   *message,
                      ProtobufCBuffer     *buffer);
SCENE *
       scene__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   scene__free_unpacked
                     (SCENE *message,
                      ProtobufCAllocator *allocator);
/* SCENES methods */
void   scenes__init
                     (SCENES         *message);
size_t scenes__get_packed_size
                     (const SCENES   *message);
size_t scenes__pack
                     (const SCENES   *message,
                      uint8_t             *out);
size_t scenes__pack_to_buffer
                     (const SCENES   *message,
                      ProtobufCBuffer     *buffer);
SCENES *
       scenes__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   scenes__free_unpacked
                     (SCENES *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*TestMsg_Closure)
                 (const TestMsg *message,
                  void *closure_data);
typedef void (*SCENE_Closure)
                 (const SCENE *message,
                  void *closure_data);
typedef void (*SCENES_Closure)
                 (const SCENES *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor test_msg__descriptor;
extern const ProtobufCMessageDescriptor scene__descriptor;
extern const ProtobufCMessageDescriptor scenes__descriptor;

PROTOBUF_C_END_DECLS


#endif  /* PROTOBUF_scene_2eproto__INCLUDED */
