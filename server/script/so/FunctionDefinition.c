#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdlib.h>
#include "script_callback.h"
#line 0 "INSTANCE_300005.script"
int script_func_INSTANCE_300005_OnMonsterDead_30019602(){
//    ATM_PinstancePopupMessage("恭喜，你战胜了晶核地宫领主！");
//    ATM_SetPinstanceCompleted();
//    ATM_SmashAllMonsters();
      printf("i am in %s %d\n", __FUNCTION__, __LINE__);
      printf("name = %s\n", sc_get_cur_player_name());
      return (0);
}
#line 0 "QUEST_1.script"
int script_func_QUEST_1_OnQuestAccept_0(){
//    ATM_PinstancePopupMessage("恭喜，你战胜了晶核地宫领主！");
//    ATM_SetPinstanceCompleted();
//    ATM_SmashAllMonsters();
      printf("i am in %s %d\n", __FUNCTION__, __LINE__);
      printf("name = %s\n", sc_get_cur_player_name());
      return (0);
}

int script_func_QUEST_1_OnQuestComplete_0(){
      printf("i am in %s %d\n", __FUNCTION__, __LINE__);
      printf("name = %s\n", sc_get_cur_player_name());
      return (0);
}

}
