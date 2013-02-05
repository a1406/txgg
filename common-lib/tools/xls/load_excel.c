#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "google/protobuf-c/protobuf-c.h"
#include "freexl.h"
#include "load_excel.h"

int max_columns;
int max_rows;
static char *name[100];
static char *data[100][100];
char tmpvalue[32];

extern int generate_pb(void *base, const ProtobufCMessageDescriptor *desc, const ProtobufCFieldDescriptor *field, char *base_name, char *value, char *data_name);

void copy_data(ProtobufCType type, unsigned offset, char *base, char *data);
void copy_repeat_data(ProtobufCType type, unsigned offset, char *base, char *data, int *index);
void malloc_repeat_count(ProtobufCType type, unsigned offset, char *base, int count);
int check_repeat_field(void *msg, const ProtobufCFieldDescriptor *field, char *name, char *value);

int load_excel(char *filename)
{
	FreeXL_CellValue cell;
	const void *handle;
    unsigned int info;
    unsigned int rows;
    unsigned short columns;
    unsigned int row;
    unsigned short col;
	int ret;
	int worksheet_index = 0;

    ret = freexl_open (filename, &handle);
    if (ret != FREEXL_OK)
	{
		fprintf (stderr, "OPEN ERROR: %d\n", ret);
		return -1;
	}

    ret = freexl_get_info (handle, FREEXL_BIFF_PASSWORD, &info);
    if (ret != FREEXL_OK)
	{
		fprintf (stderr, "GET-INFO [FREEXL_BIFF_PASSWORD] Error: %d\n", ret);
		goto stop;
	}
    switch (info)
	{
		case FREEXL_BIFF_PLAIN:
			break;
		case FREEXL_BIFF_OBFUSCATED:
		default:
			fprintf (stderr, "Password protected: (not accessible)\n");
			goto stop;
	};

	ret = freexl_select_active_worksheet (handle, worksheet_index);
	if (ret != FREEXL_OK)
	{
		fprintf (stderr, "SELECT-ACTIVE_WORKSHEET Error: %d\n", ret);
		goto stop;
	}
			/* dimensions */
	ret = freexl_worksheet_dimensions (handle, &rows, &columns);
	if (ret != FREEXL_OK)
	{
		fprintf (stderr, "WORKSHEET-DIMENSIONS Error: %d\n", ret);
		goto stop;
	}

	if (rows < 3)
	{
		fprintf (stderr, "至少需要三行: %d\n", ret);
		goto stop;		
	}

	max_columns = columns;
	max_rows = rows;

	row = 1;

	for (col = 0; col < columns; col++)
	{
		ret = freexl_get_cell_value (handle, row, col, &cell);
		if (ret != FREEXL_OK)
		{
			fprintf (stderr,
				"CELL-VALUE-ERROR (r=%u c=%u): %d\n", row,
				col, ret);
			goto stop;
		}
		name[col] = strdup(cell.value.text_value);
	}
	
	for (row = 2; row < rows; row++)
	{
		for (col = 0; col < columns; col++)
		{
			ret = freexl_get_cell_value (handle, row, col, &cell);
			if (ret != FREEXL_OK)
			{
				fprintf (stderr,
					"CELL-VALUE-ERROR (r=%u c=%u): %d\n", row,
					col, ret);
				goto stop;
			}
			switch (cell.type)
			{
				case FREEXL_CELL_INT:
//					printf (", %d", cell.value.int_value);
					sprintf(tmpvalue, "%d", cell.value.int_value);
					data[row - 2][col] = strdup(tmpvalue);
					break;
				case FREEXL_CELL_DOUBLE:
//					printf (", %1.12f", cell.value.double_value);
					sprintf(tmpvalue, "%d", (int)cell.value.double_value);
					data[row - 2][col] = strdup(tmpvalue);					
					break;
				case FREEXL_CELL_TEXT:
				case FREEXL_CELL_SST_TEXT:
//					print_sql_string (cell.value.text_value);
//					printf (", '%s'", cell.value.text_value);
					data[row - 2][col] = strdup(cell.value.text_value);							
					break;
				case FREEXL_CELL_DATE:
				case FREEXL_CELL_DATETIME:
				case FREEXL_CELL_TIME:
//					printf (", '%s'", cell.value.text_value);
					data[row - 2][col] = strdup(cell.value.text_value);					
					break;
				case FREEXL_CELL_NULL:
				default:
//					printf (", NULL");
					data[row - 2][col] = NULL;
					break;
			};
		}
	}
	return (0);
stop:
/* closing the .XLS file [Workbook] */
    ret = freexl_close (handle);
    if (ret != FREEXL_OK)
	{
		fprintf (stderr, "CLOSE ERROR: %d\n", ret);
		return -1;
	}

	return (0);
}

int generate_pb_data(func_get_data_base get_data_base)
{
	size_t size;
	int i;
	int line, j;
	ProtobufCMessage *base;

	for (line = 0; line < max_rows; ++line)
	{
		base = get_data_base(line);
		for (j = 0; j < max_columns; ++j)
		{
			if (!data[line][j])
				continue;
			for (i = 0; i < base->descriptor->n_fields; ++i)
			{
				if (base->descriptor->fields[i].label == PROTOBUF_C_LABEL_REPEATED) {
					if (*(void **)((char *)base + base->descriptor->fields[i].offset) == NULL) {
//						*(void **)((char *)base + base->descriptor->fields[i].offset) = malloc(10240);
						assert(0);
					}
					
				}
				
				if (0 == generate_pb(&base->descriptor, base->descriptor, &base->descriptor->fields[i], NULL, data[line][j], name[j])) {
//					if (i == 0 && base->descriptor->fields[i].label == PROTOBUF_C_LABEL_REPEATED)
//						*(int *)((char *)base + base->descriptor->fields[i].quantifier_offset) += 1;
					break;
				}
/*
				if (base->descriptor->fields[i].label == PROTOBUF_C_LABEL_REPEATED) {
					if (check_repeat_field(base, &base->descriptor->fields[i], name[j], data[line][j]) == 0)
						break;
				} else {
					if (check_required_field(base, &base->descriptor->fields[i], name[j], data[line][j]) == 0)
						break;					
				}
*/				
/*

				else if (strcmp(name[j], base->descriptor->fields[i].name) == 0) {
					printf("find %s, value = %s\n", name[j], data[line][j]);
					copy_data(base->descriptor->fields[i].type,
						base->descriptor->fields[i].offset,
						(char *)base,
						data[line][j]);
					break;
				}
*/				
			}
		}
	}
	return (0);
//	size = npcs__pack(&result, outbuf);
//	npc = npcs__unpack(NULL, size, outbuf);
/*
	int fd = open("demo.output", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
*/	
}

void copy_data(ProtobufCType type, unsigned offset, char *base, char *data)
{
	int i;
	switch (type)
	{
		case PROTOBUF_C_TYPE_INT32:
		case PROTOBUF_C_TYPE_SINT32:
		case PROTOBUF_C_TYPE_SFIXED32:
			i = atoi(data);
			memcpy(base + offset, &i, 4);
			break;
		case PROTOBUF_C_TYPE_STRING:
			*(char **)(base + offset) = strdup(data);
//			strcpy(base + offset, data);
			break;
		default:
			printf("invalid type\n");
			return;
	}
	
}

void copy_repeat_data(ProtobufCType type, unsigned offset, char *base, char *data, int *index)
{
	int i;
	char **addr;
	char *p;

	addr = (char **)(base + offset);

	switch (type)
	{
		case PROTOBUF_C_TYPE_INT32:
		case PROTOBUF_C_TYPE_SINT32:
		case PROTOBUF_C_TYPE_SFIXED32:
			p = (*addr + 4 * (*index));
			i = atoi(data);
			memcpy(p, &i, 4);			
			break;
		case PROTOBUF_C_TYPE_STRING:
			p = (*addr + sizeof(void *) * (*index));			
			*(char **)(p) = strdup(data);
			break;
		default:
			printf("invalid type\n");
			return;
	}
	++(*index);
}

void malloc_repeat_count(ProtobufCType type, unsigned offset, char *base, int count)
{
	switch (type)
	{
		case PROTOBUF_C_TYPE_INT32:
		case PROTOBUF_C_TYPE_SINT32:
		case PROTOBUF_C_TYPE_SFIXED32:
			*(char **)(base + offset) = malloc(4 * count);
			break;
		case PROTOBUF_C_TYPE_STRING:
			*(char **)(base + offset) = malloc(sizeof(void *) * count);
			break;
		default:
			printf("invalid type\n");
			return;
	}
}

int check_repeat_field(void *msg, const ProtobufCFieldDescriptor *field, char *name, char *value)
{
	int count;
	assert(field->label == PROTOBUF_C_LABEL_REPEATED);
	if (strncmp(name, "n_", 2) == 0 &&
		strcmp(&name[2], field->name) == 0) {
		assert(*(char **)((char *)msg + field->offset) == NULL);
		printf("find %s, value = %s\n", name, value);
		count = atoi(value);
		malloc_repeat_count(field->type,
			field->offset,
			(char *)msg,
			count);
		*(int *)((char *)msg + field->quantifier_offset) = 0;
		return (0);
	} else if (strcmp(name, field->name) == 0) {

		// repeat 使用的内存，默认只给4096， 如果比较多的元素的话请指定数量
		if (*(char **)((char *)msg + field->offset) == NULL)
			*(char **)((char *)msg + field->offset) = malloc(4096);
		
		printf("find repeated[%d] %s, value = %s\n",
			*(int *)((char *)msg + field->quantifier_offset),
			name, value);
		copy_repeat_data(field->type,
						field->offset,
						(char *)msg,
						value,
						(int *)((char *)msg + field->quantifier_offset));
		return (0);
	}
	return (1);
}

int check_required_field(void *msg, const ProtobufCFieldDescriptor *field, char *name, char *value)
{
	if (field->type == PROTOBUF_C_TYPE_MESSAGE) {
		return (0);
	}
	return (1);
}
