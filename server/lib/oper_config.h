/*
 * author: dengzhaoqun
 * date	 : 2011/07/23
 * modifed jack   2012/06
 */

#ifndef OPER_CONFIG_H_
#define OPER_CONFIG_H_

#define MAX_LINE_LEN 210

char *get_first_key(FILE *fp, char *key);
char *get_next_key(FILE *fp, char *key);
char *get_value(char *line);

#endif

/*
说明:
1)  配置文件的数据格式  key = value

2) 支持 '#' 开头注释

3) key, value 前后可有空格, tab.
*/
