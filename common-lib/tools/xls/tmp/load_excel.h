#ifndef __TOOL_LOAD_EXCEL_H__
#define __TOOL_LOAD_EXCEL_H__

#include "google/protobuf-c/protobuf-c.h"

typedef ProtobufCMessage *(*func_get_data_base)(int line);

extern int max_columns;
extern int max_rows;
int generate_pb_data(func_get_data_base get_data_base);
int load_excel(char *filename);


#endif
