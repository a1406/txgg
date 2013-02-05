#ifndef _SYM_PARSER_H__
#define _SYM_PARSER_H__

int parse_file_name(char *name, uint8_t *type, uint32_t *param_type);
int parse_function_name(char *name, uint8_t *action, uint32_t *param_action);

#endif
