#ifndef _SYM_PARSER_H__
#define _SYM_PARSER_H__
#include <stdint.h>

int parse_file_name(char *name, uint8_t *type, uint32_t *param_type);
int parse_function_name(char *name, uint8_t *action, uint32_t *param_action);
int generate_register_sentence(char* sentence, const uint8_t type, const uint32_t param_type, const uint8_t action,
	const uint32_t param_action);
int generate_function_name_sentence(char *function_name, const uint8_t type, const uint32_t param_type, const uint8_t action,
	const uint32_t param_action);
#endif
