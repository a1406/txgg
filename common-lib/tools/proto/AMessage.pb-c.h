/* Generated by the protocol buffer compiler.  DO NOT EDIT! */

#ifndef PROTOBUF_C_AMessage_2eproto__INCLUDED
#define PROTOBUF_C_AMessage_2eproto__INCLUDED

#include <google/protobuf-c/protobuf-c.h>

PROTOBUF_C_BEGIN_DECLS


typedef struct _AMessage AMessage;
typedef struct _Ttt Ttt;
typedef struct _BMessage BMessage;
typedef struct _RoleSummary RoleSummary;
typedef struct _RolePos RolePos;
typedef struct _LoginReq LoginReq;
typedef struct _LoginResp LoginResp;
typedef struct _ListroleReq ListroleReq;
typedef struct _ListroleResp ListroleResp;
typedef struct _CreateroleReq CreateroleReq;
typedef struct _CreateroleResp CreateroleResp;
typedef struct _MoveReq MoveReq;
typedef struct _MoveResp MoveResp;
typedef struct _MoveNotify MoveNotify;
typedef struct _TransportReq TransportReq;
typedef struct _TransportResp TransportResp;
typedef struct _LeaveSceneNotify LeaveSceneNotify;
typedef struct _JoinSceneNotify JoinSceneNotify;
typedef struct _LogoutNotify LogoutNotify;
typedef struct _RivalData RivalData;
typedef struct _RoundData RoundData;
typedef struct _FightNotify FightNotify;


/* --- enums --- */

typedef enum _TYPES {
  TYPES__INT_TYPE = 1,
  TYPES__CHAR_TYPE = 2,
  TYPES__STRING_TYPE = 3,
  TYPES__FLOAT_TYPE = 4,
  TYPES__STRUCT_TYPE = 5
} TYPES;

/* --- messages --- */

struct  _AMessage
{
  ProtobufCMessage base;
  TYPES a;
  char *b;
  size_t n_c;
  int32_t *c;
  size_t n_d;
  char **d;
};
#define AMESSAGE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&amessage__descriptor) \
    , 0, NULL, 0,NULL, 0,NULL }


struct  _Ttt
{
  ProtobufCMessage base;
  int32_t a;
  int32_t b;
  int32_t c;
};
#define TTT__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ttt__descriptor) \
    , 0, 0, 0 }


struct  _BMessage
{
  ProtobufCMessage base;
  size_t n_msg;
  Ttt **msg;
};
#define BMESSAGE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&bmessage__descriptor) \
    , 0,NULL }


struct  _RoleSummary
{
  ProtobufCMessage base;
  uint32_t seq;
  uint32_t profession;
  uint32_t level;
  char *name;
};
#define ROLE_SUMMARY__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&role_summary__descriptor) \
    , 0, 0, 0, NULL }


struct  _RolePos
{
  ProtobufCMessage base;
  uint32_t pos_x;
  uint32_t pos_y;
};
#define ROLE_POS__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&role_pos__descriptor) \
    , 0, 0 }


struct  _LoginReq
{
  ProtobufCMessage base;
  uint32_t role_id;
};
#define LOGIN_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&login_req__descriptor) \
    , 0 }


struct  _LoginResp
{
  ProtobufCMessage base;
  uint32_t role_id;
  RoleSummary *role_summary;
  uint32_t scene_id;
  RolePos *role_pos;
};
#define LOGIN_RESP__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&login_resp__descriptor) \
    , 0, NULL, 0, NULL }


struct  _ListroleReq
{
  ProtobufCMessage base;
  uint32_t role_id;
};
#define LISTROLE_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&listrole_req__descriptor) \
    , 0 }


struct  _ListroleResp
{
  ProtobufCMessage base;
  uint32_t role_id;
  size_t n_role_summary;
  RoleSummary **role_summary;
};
#define LISTROLE_RESP__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&listrole_resp__descriptor) \
    , 0, 0,NULL }


struct  _CreateroleReq
{
  ProtobufCMessage base;
  uint32_t role_id;
  uint32_t profession;
  char *name;
};
#define CREATEROLE_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&createrole_req__descriptor) \
    , 0, 0, NULL }


struct  _CreateroleResp
{
  ProtobufCMessage base;
  uint32_t role_id;
  RoleSummary *role_summary;
};
#define CREATEROLE_RESP__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&createrole_resp__descriptor) \
    , 0, NULL }


struct  _MoveReq
{
  ProtobufCMessage base;
  size_t n_path;
  RolePos **path;
};
#define MOVE_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&move_req__descriptor) \
    , 0,NULL }


struct  _MoveResp
{
  ProtobufCMessage base;
  int32_t result;
  size_t n_path;
  RolePos **path;
};
#define MOVE_RESP__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&move_resp__descriptor) \
    , 0, 0,NULL }


struct  _MoveNotify
{
  ProtobufCMessage base;
  uint32_t role_id;
  size_t n_path;
  RolePos **path;
};
#define MOVE_NOTIFY__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&move_notify__descriptor) \
    , 0, 0,NULL }


struct  _TransportReq
{
  ProtobufCMessage base;
  uint32_t target_scene_id;
};
#define TRANSPORT_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&transport_req__descriptor) \
    , 0 }


struct  _TransportResp
{
  ProtobufCMessage base;
  int32_t result;
};
#define TRANSPORT_RESP__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&transport_resp__descriptor) \
    , 0 }


struct  _LeaveSceneNotify
{
  ProtobufCMessage base;
  uint32_t role_id;
};
#define LEAVE_SCENE_NOTIFY__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&leave_scene_notify__descriptor) \
    , 0 }


struct  _JoinSceneNotify
{
  ProtobufCMessage base;
  uint32_t role_id;
};
#define JOIN_SCENE_NOTIFY__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&join_scene_notify__descriptor) \
    , 0 }


struct  _LogoutNotify
{
  ProtobufCMessage base;
  uint32_t role_id;
};
#define LOGOUT_NOTIFY__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&logout_notify__descriptor) \
    , 0 }


struct  _RivalData
{
  ProtobufCMessage base;
  char *name;
  uint32_t res_id;
  uint32_t hp;
  size_t n_buff;
  uint32_t *buff;
};
#define RIVAL_DATA__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&rival_data__descriptor) \
    , NULL, 0, 0, 0,NULL }


struct  _RoundData
{
  ProtobufCMessage base;
  protobuf_c_boolean left;
  uint32_t lhp_delta;
  uint32_t rhp_delta;
  size_t n_lbuff;
  uint32_t *lbuff;
  size_t n_rbuff;
  uint32_t *rbuff;
};
#define ROUND_DATA__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&round_data__descriptor) \
    , 0, 0, 0, 0,NULL, 0,NULL }


struct  _FightNotify
{
  ProtobufCMessage base;
  RivalData *rival_data;
  int32_t result;
  size_t n_round_data;
  RoundData **round_data;
};
#define FIGHT_NOTIFY__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&fight_notify__descriptor) \
    , NULL, 0, 0,NULL }


/* AMessage methods */
void   amessage__init
                     (AMessage         *message);
size_t amessage__get_packed_size
                     (const AMessage   *message);
size_t amessage__pack
                     (const AMessage   *message,
                      uint8_t             *out);
size_t amessage__pack_to_buffer
                     (const AMessage   *message,
                      ProtobufCBuffer     *buffer);
AMessage *
       amessage__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   amessage__free_unpacked
                     (AMessage *message,
                      ProtobufCAllocator *allocator);
/* Ttt methods */
void   ttt__init
                     (Ttt         *message);
size_t ttt__get_packed_size
                     (const Ttt   *message);
size_t ttt__pack
                     (const Ttt   *message,
                      uint8_t             *out);
size_t ttt__pack_to_buffer
                     (const Ttt   *message,
                      ProtobufCBuffer     *buffer);
Ttt *
       ttt__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ttt__free_unpacked
                     (Ttt *message,
                      ProtobufCAllocator *allocator);
/* BMessage methods */
void   bmessage__init
                     (BMessage         *message);
size_t bmessage__get_packed_size
                     (const BMessage   *message);
size_t bmessage__pack
                     (const BMessage   *message,
                      uint8_t             *out);
size_t bmessage__pack_to_buffer
                     (const BMessage   *message,
                      ProtobufCBuffer     *buffer);
BMessage *
       bmessage__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   bmessage__free_unpacked
                     (BMessage *message,
                      ProtobufCAllocator *allocator);
/* RoleSummary methods */
void   role_summary__init
                     (RoleSummary         *message);
size_t role_summary__get_packed_size
                     (const RoleSummary   *message);
size_t role_summary__pack
                     (const RoleSummary   *message,
                      uint8_t             *out);
size_t role_summary__pack_to_buffer
                     (const RoleSummary   *message,
                      ProtobufCBuffer     *buffer);
RoleSummary *
       role_summary__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   role_summary__free_unpacked
                     (RoleSummary *message,
                      ProtobufCAllocator *allocator);
/* RolePos methods */
void   role_pos__init
                     (RolePos         *message);
size_t role_pos__get_packed_size
                     (const RolePos   *message);
size_t role_pos__pack
                     (const RolePos   *message,
                      uint8_t             *out);
size_t role_pos__pack_to_buffer
                     (const RolePos   *message,
                      ProtobufCBuffer     *buffer);
RolePos *
       role_pos__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   role_pos__free_unpacked
                     (RolePos *message,
                      ProtobufCAllocator *allocator);
/* LoginReq methods */
void   login_req__init
                     (LoginReq         *message);
size_t login_req__get_packed_size
                     (const LoginReq   *message);
size_t login_req__pack
                     (const LoginReq   *message,
                      uint8_t             *out);
size_t login_req__pack_to_buffer
                     (const LoginReq   *message,
                      ProtobufCBuffer     *buffer);
LoginReq *
       login_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   login_req__free_unpacked
                     (LoginReq *message,
                      ProtobufCAllocator *allocator);
/* LoginResp methods */
void   login_resp__init
                     (LoginResp         *message);
size_t login_resp__get_packed_size
                     (const LoginResp   *message);
size_t login_resp__pack
                     (const LoginResp   *message,
                      uint8_t             *out);
size_t login_resp__pack_to_buffer
                     (const LoginResp   *message,
                      ProtobufCBuffer     *buffer);
LoginResp *
       login_resp__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   login_resp__free_unpacked
                     (LoginResp *message,
                      ProtobufCAllocator *allocator);
/* ListroleReq methods */
void   listrole_req__init
                     (ListroleReq         *message);
size_t listrole_req__get_packed_size
                     (const ListroleReq   *message);
size_t listrole_req__pack
                     (const ListroleReq   *message,
                      uint8_t             *out);
size_t listrole_req__pack_to_buffer
                     (const ListroleReq   *message,
                      ProtobufCBuffer     *buffer);
ListroleReq *
       listrole_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   listrole_req__free_unpacked
                     (ListroleReq *message,
                      ProtobufCAllocator *allocator);
/* ListroleResp methods */
void   listrole_resp__init
                     (ListroleResp         *message);
size_t listrole_resp__get_packed_size
                     (const ListroleResp   *message);
size_t listrole_resp__pack
                     (const ListroleResp   *message,
                      uint8_t             *out);
size_t listrole_resp__pack_to_buffer
                     (const ListroleResp   *message,
                      ProtobufCBuffer     *buffer);
ListroleResp *
       listrole_resp__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   listrole_resp__free_unpacked
                     (ListroleResp *message,
                      ProtobufCAllocator *allocator);
/* CreateroleReq methods */
void   createrole_req__init
                     (CreateroleReq         *message);
size_t createrole_req__get_packed_size
                     (const CreateroleReq   *message);
size_t createrole_req__pack
                     (const CreateroleReq   *message,
                      uint8_t             *out);
size_t createrole_req__pack_to_buffer
                     (const CreateroleReq   *message,
                      ProtobufCBuffer     *buffer);
CreateroleReq *
       createrole_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   createrole_req__free_unpacked
                     (CreateroleReq *message,
                      ProtobufCAllocator *allocator);
/* CreateroleResp methods */
void   createrole_resp__init
                     (CreateroleResp         *message);
size_t createrole_resp__get_packed_size
                     (const CreateroleResp   *message);
size_t createrole_resp__pack
                     (const CreateroleResp   *message,
                      uint8_t             *out);
size_t createrole_resp__pack_to_buffer
                     (const CreateroleResp   *message,
                      ProtobufCBuffer     *buffer);
CreateroleResp *
       createrole_resp__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   createrole_resp__free_unpacked
                     (CreateroleResp *message,
                      ProtobufCAllocator *allocator);
/* MoveReq methods */
void   move_req__init
                     (MoveReq         *message);
size_t move_req__get_packed_size
                     (const MoveReq   *message);
size_t move_req__pack
                     (const MoveReq   *message,
                      uint8_t             *out);
size_t move_req__pack_to_buffer
                     (const MoveReq   *message,
                      ProtobufCBuffer     *buffer);
MoveReq *
       move_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   move_req__free_unpacked
                     (MoveReq *message,
                      ProtobufCAllocator *allocator);
/* MoveResp methods */
void   move_resp__init
                     (MoveResp         *message);
size_t move_resp__get_packed_size
                     (const MoveResp   *message);
size_t move_resp__pack
                     (const MoveResp   *message,
                      uint8_t             *out);
size_t move_resp__pack_to_buffer
                     (const MoveResp   *message,
                      ProtobufCBuffer     *buffer);
MoveResp *
       move_resp__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   move_resp__free_unpacked
                     (MoveResp *message,
                      ProtobufCAllocator *allocator);
/* MoveNotify methods */
void   move_notify__init
                     (MoveNotify         *message);
size_t move_notify__get_packed_size
                     (const MoveNotify   *message);
size_t move_notify__pack
                     (const MoveNotify   *message,
                      uint8_t             *out);
size_t move_notify__pack_to_buffer
                     (const MoveNotify   *message,
                      ProtobufCBuffer     *buffer);
MoveNotify *
       move_notify__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   move_notify__free_unpacked
                     (MoveNotify *message,
                      ProtobufCAllocator *allocator);
/* TransportReq methods */
void   transport_req__init
                     (TransportReq         *message);
size_t transport_req__get_packed_size
                     (const TransportReq   *message);
size_t transport_req__pack
                     (const TransportReq   *message,
                      uint8_t             *out);
size_t transport_req__pack_to_buffer
                     (const TransportReq   *message,
                      ProtobufCBuffer     *buffer);
TransportReq *
       transport_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   transport_req__free_unpacked
                     (TransportReq *message,
                      ProtobufCAllocator *allocator);
/* TransportResp methods */
void   transport_resp__init
                     (TransportResp         *message);
size_t transport_resp__get_packed_size
                     (const TransportResp   *message);
size_t transport_resp__pack
                     (const TransportResp   *message,
                      uint8_t             *out);
size_t transport_resp__pack_to_buffer
                     (const TransportResp   *message,
                      ProtobufCBuffer     *buffer);
TransportResp *
       transport_resp__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   transport_resp__free_unpacked
                     (TransportResp *message,
                      ProtobufCAllocator *allocator);
/* LeaveSceneNotify methods */
void   leave_scene_notify__init
                     (LeaveSceneNotify         *message);
size_t leave_scene_notify__get_packed_size
                     (const LeaveSceneNotify   *message);
size_t leave_scene_notify__pack
                     (const LeaveSceneNotify   *message,
                      uint8_t             *out);
size_t leave_scene_notify__pack_to_buffer
                     (const LeaveSceneNotify   *message,
                      ProtobufCBuffer     *buffer);
LeaveSceneNotify *
       leave_scene_notify__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   leave_scene_notify__free_unpacked
                     (LeaveSceneNotify *message,
                      ProtobufCAllocator *allocator);
/* JoinSceneNotify methods */
void   join_scene_notify__init
                     (JoinSceneNotify         *message);
size_t join_scene_notify__get_packed_size
                     (const JoinSceneNotify   *message);
size_t join_scene_notify__pack
                     (const JoinSceneNotify   *message,
                      uint8_t             *out);
size_t join_scene_notify__pack_to_buffer
                     (const JoinSceneNotify   *message,
                      ProtobufCBuffer     *buffer);
JoinSceneNotify *
       join_scene_notify__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   join_scene_notify__free_unpacked
                     (JoinSceneNotify *message,
                      ProtobufCAllocator *allocator);
/* LogoutNotify methods */
void   logout_notify__init
                     (LogoutNotify         *message);
size_t logout_notify__get_packed_size
                     (const LogoutNotify   *message);
size_t logout_notify__pack
                     (const LogoutNotify   *message,
                      uint8_t             *out);
size_t logout_notify__pack_to_buffer
                     (const LogoutNotify   *message,
                      ProtobufCBuffer     *buffer);
LogoutNotify *
       logout_notify__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   logout_notify__free_unpacked
                     (LogoutNotify *message,
                      ProtobufCAllocator *allocator);
/* RivalData methods */
void   rival_data__init
                     (RivalData         *message);
size_t rival_data__get_packed_size
                     (const RivalData   *message);
size_t rival_data__pack
                     (const RivalData   *message,
                      uint8_t             *out);
size_t rival_data__pack_to_buffer
                     (const RivalData   *message,
                      ProtobufCBuffer     *buffer);
RivalData *
       rival_data__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   rival_data__free_unpacked
                     (RivalData *message,
                      ProtobufCAllocator *allocator);
/* RoundData methods */
void   round_data__init
                     (RoundData         *message);
size_t round_data__get_packed_size
                     (const RoundData   *message);
size_t round_data__pack
                     (const RoundData   *message,
                      uint8_t             *out);
size_t round_data__pack_to_buffer
                     (const RoundData   *message,
                      ProtobufCBuffer     *buffer);
RoundData *
       round_data__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   round_data__free_unpacked
                     (RoundData *message,
                      ProtobufCAllocator *allocator);
/* FightNotify methods */
void   fight_notify__init
                     (FightNotify         *message);
size_t fight_notify__get_packed_size
                     (const FightNotify   *message);
size_t fight_notify__pack
                     (const FightNotify   *message,
                      uint8_t             *out);
size_t fight_notify__pack_to_buffer
                     (const FightNotify   *message,
                      ProtobufCBuffer     *buffer);
FightNotify *
       fight_notify__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   fight_notify__free_unpacked
                     (FightNotify *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*AMessage_Closure)
                 (const AMessage *message,
                  void *closure_data);
typedef void (*Ttt_Closure)
                 (const Ttt *message,
                  void *closure_data);
typedef void (*BMessage_Closure)
                 (const BMessage *message,
                  void *closure_data);
typedef void (*RoleSummary_Closure)
                 (const RoleSummary *message,
                  void *closure_data);
typedef void (*RolePos_Closure)
                 (const RolePos *message,
                  void *closure_data);
typedef void (*LoginReq_Closure)
                 (const LoginReq *message,
                  void *closure_data);
typedef void (*LoginResp_Closure)
                 (const LoginResp *message,
                  void *closure_data);
typedef void (*ListroleReq_Closure)
                 (const ListroleReq *message,
                  void *closure_data);
typedef void (*ListroleResp_Closure)
                 (const ListroleResp *message,
                  void *closure_data);
typedef void (*CreateroleReq_Closure)
                 (const CreateroleReq *message,
                  void *closure_data);
typedef void (*CreateroleResp_Closure)
                 (const CreateroleResp *message,
                  void *closure_data);
typedef void (*MoveReq_Closure)
                 (const MoveReq *message,
                  void *closure_data);
typedef void (*MoveResp_Closure)
                 (const MoveResp *message,
                  void *closure_data);
typedef void (*MoveNotify_Closure)
                 (const MoveNotify *message,
                  void *closure_data);
typedef void (*TransportReq_Closure)
                 (const TransportReq *message,
                  void *closure_data);
typedef void (*TransportResp_Closure)
                 (const TransportResp *message,
                  void *closure_data);
typedef void (*LeaveSceneNotify_Closure)
                 (const LeaveSceneNotify *message,
                  void *closure_data);
typedef void (*JoinSceneNotify_Closure)
                 (const JoinSceneNotify *message,
                  void *closure_data);
typedef void (*LogoutNotify_Closure)
                 (const LogoutNotify *message,
                  void *closure_data);
typedef void (*RivalData_Closure)
                 (const RivalData *message,
                  void *closure_data);
typedef void (*RoundData_Closure)
                 (const RoundData *message,
                  void *closure_data);
typedef void (*FightNotify_Closure)
                 (const FightNotify *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCEnumDescriptor    types__descriptor;
extern const ProtobufCMessageDescriptor amessage__descriptor;
extern const ProtobufCMessageDescriptor ttt__descriptor;
extern const ProtobufCMessageDescriptor bmessage__descriptor;
extern const ProtobufCMessageDescriptor role_summary__descriptor;
extern const ProtobufCMessageDescriptor role_pos__descriptor;
extern const ProtobufCMessageDescriptor login_req__descriptor;
extern const ProtobufCMessageDescriptor login_resp__descriptor;
extern const ProtobufCMessageDescriptor listrole_req__descriptor;
extern const ProtobufCMessageDescriptor listrole_resp__descriptor;
extern const ProtobufCMessageDescriptor createrole_req__descriptor;
extern const ProtobufCMessageDescriptor createrole_resp__descriptor;
extern const ProtobufCMessageDescriptor move_req__descriptor;
extern const ProtobufCMessageDescriptor move_resp__descriptor;
extern const ProtobufCMessageDescriptor move_notify__descriptor;
extern const ProtobufCMessageDescriptor transport_req__descriptor;
extern const ProtobufCMessageDescriptor transport_resp__descriptor;
extern const ProtobufCMessageDescriptor leave_scene_notify__descriptor;
extern const ProtobufCMessageDescriptor join_scene_notify__descriptor;
extern const ProtobufCMessageDescriptor logout_notify__descriptor;
extern const ProtobufCMessageDescriptor rival_data__descriptor;
extern const ProtobufCMessageDescriptor round_data__descriptor;
extern const ProtobufCMessageDescriptor fight_notify__descriptor;

PROTOBUF_C_END_DECLS


#endif  /* PROTOBUF_AMessage_2eproto__INCLUDED */
