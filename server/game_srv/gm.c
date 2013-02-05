#include "gm.h"
#include "game_srv.h"

#define MAX_GAMEMASTER_HANDLER_NUMBER  (128)

static int  gm_param_num;
static char gm_param[MAX_GAMEMASTER_PARAMETER_NUMBER][MAX_GAMEMASTER_PER_PARAMETER_LENGTH];

typedef struct gm_handle
{
	int len;
	char cmd[MAX_GAMEMASTER_PER_PARAMETER_LENGTH];
	func_gm_handle handle;
} GM_HANDLE;

static int gm_handle_num;
static GM_HANDLE gm_handle_list[MAX_GAMEMASTER_HANDLER_NUMBER];

GM_HANDLE *get_gm_handle()
{
	int i;
	int len;
	len = strlen(gm_param[0]);
	for (i = 0; i < gm_handle_num; ++i) {
		if (len != gm_handle_list[i].len)
			continue;
		if (memcmp(gm_param[0], gm_handle_list[i].cmd, len) == 0)
			return &gm_handle_list[i];
	}
	return NULL;
}

int parse_gm_parameter(char *cmd)
{
	assert(cmd);
	unsigned int i;
	char *saved_cmd = cmd;
	
    // 这里从下标2开始是因为要跳过双划杠
//    char* pszCommand = &m_pRequestMsg->m_stMsgBody.m_stGameMaster_Request.m_szCommand[2];
//    int len = strlen(cmd);

    gm_param_num = 0;
//    memset(&m_aszCommand, 0, sizeof(m_aszCommand));

    // 双划杠之后必须紧跟命令字，如果是空格则返回错误。
//    if (*cmd == ' ')
//    {
//        return -1;
//    }

	while (*cmd) {
		if (*cmd == ' ') {
			++cmd;
			continue;
		}
        for (i = 0; i < sizeof(gm_param[0]); i++) {
			if (cmd[i] == ' ' || cmd[i] == '\0') {
				gm_param[gm_param_num][i] = '\0';
				++gm_param_num;
				break;
			}
            gm_param[gm_param_num][i] = cmd[i];
		}
		cmd += (i);
	}
		
/*
    // 识别参数，第一个参数为命令
    for (; len > 0; len--)
    {
        // skip ' '
        if (*cmd == ' ')
        {
            cmd++;
			len--;
            continue;
        }

        if (*cmd == '\0')
        {
			goto done;
        }

        // copy parameter
        for (i = 0; i < sizeof(gm_param[0]); i++)
        {
            gm_param[gm_param_num][i] = *cmd;
            if (*cmd == '\0') 
            {
				gm_param_num++;				
				goto done;				
            }

            if (*cmd == ' ') 
            {
				gm_param[gm_param_num][i] = '\0';
                break;
            }

            if (len <= 0)
            {
				gm_param[gm_param_num][i] = '\0';
				gm_param_num++;				
				goto done;
            }
            cmd++;
            len--;
        }
        gm_param_num++;

        // skip not null
        while (1)
        {
            if (*cmd == '\0') 
            {
                break;
            }

            if (*cmd == ' ') 
            {
                break;
            }

            if (len <= 0)
            {
				goto done;
            }

            cmd++;
            len--;
        }
    }

done:
*/
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "%s: get %d cmd, cmd is %s", __FUNCTION__, gm_param_num, saved_cmd);
	
    return 0;
}

int run_gm_command(ROLE *role, char *cmd)
{
	GM_HANDLE *handle;
	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "%s: role[%lu] send gm cmd %s", __FUNCTION__, role->role_id, cmd);
	if (parse_gm_parameter(cmd) != 0) {
			//todo
		return (-1);
	}
	handle = get_gm_handle();
	if (handle == NULL) {
			//todo
		return (-10);		
	}
	return handle->handle(role,  gm_param_num - 1, &gm_param[1]);
}

int register_gm_handle(func_gm_handle handle, char *cmd)
{
	if (gm_handle_num >= MAX_GAMEMASTER_HANDLER_NUMBER) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: gm handle full", __FUNCTION__);
		return (-1);
	}
	gm_handle_list[gm_handle_num].len = strlen(cmd);
	if (gm_handle_list[gm_handle_num].len >= MAX_GAMEMASTER_PER_PARAMETER_LENGTH) {
		log4c_category_log(mycat, LOG4C_PRIORITY_ERROR, "%s: gm handle name too long", __FUNCTION__, cmd);		
		return (-10);
	}
	strcpy(gm_handle_list[gm_handle_num].cmd, cmd);
	gm_handle_list[gm_handle_num].handle = handle;
	++gm_handle_num;

	log4c_category_log(mycat, LOG4C_PRIORITY_INFO, "%s: gm handle %s register success", __FUNCTION__, cmd);
	return (0);
}

