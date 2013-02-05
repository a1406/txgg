#define SERVER_IP "192.168.1.119"

//Definet user state 
#define USER_UNEXIT             0
#define USER_INIT               1
#define USER_LOGIN              2
#define USER_IN_INSTANCE         3
#define USER_IN_SCENE            4

//Define test_function_id
#define TID_NOT_START                       0
#define TID_LOGIN_REQUEST                    1
#define TID_MOVE_REQUEST                     2
#define TID_ENTER_INSTANCE_REQUEST            3
#define TID_LEAVE_INSTANCE_REQUEST            4
#define TID_SLEEP                           5
#define TID_CHAT_REQUEST                     6

uint32_t Id_Base;
uint32_t Id_Count;

typedef struct
{
    uint32_t   userid;
    uint32_t   userstate;
    CLIENT_MAP client_maps;
    uint32_t   testid;
    uint32_t   pos_x;
    uint32_t   pos_y;
}testuser;


 
