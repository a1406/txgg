#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "AMessage.pb-c.h"

struct data
{
	int id;
	char *name;
	char *value;
};

struct data datas[] = {
/*
	{1, "a", "2"},
	{2, "b", "tangpeilei"},
	{3, "n_c", "3"},
	{3, "c", "10"},
	{3, "c", "20"},
	{3, "c", "30"},
*/
	{4, "n_d", "4"},
	{4, "d", "d10"},
	{4, "d", "d20"},
	{4, "d", "d30"},	
	{4, "d", "d40"},	
};

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

int check_repeat_field(void *msg, const ProtobufCFieldDescriptor *field, struct data *data)
{
	int count;
	assert(field->label == PROTOBUF_C_LABEL_REPEATED);
	if (strncmp(datas->name, "n_", 2) == 0 &&
		strcmp(&data->name[2], field->name) == 0) {
		assert(*(char **)((char *)msg + field->offset) == NULL);
		printf("find %s, value = %s\n", data->name, data->value);
		count = atoi(data->value);
		malloc_repeat_count(field->type,
			field->offset,
			(char *)msg,
			count);
		*(int *)((char *)msg + field->quantifier_offset) = 0;
		return (0);
	} else if (strcmp(data->name, field->name) == 0) {
		printf("find repeated[%d] %s, value = %s\n",
			*(int *)((char *)msg + field->quantifier_offset),
			data->name, data->value);
		copy_repeat_data(field->type,
						field->offset,
						(char *)msg,
						data->value,
						(int *)((char *)msg + field->quantifier_offset));
		return (0);
	}
	return (1);
}

int main(int argc, char *argv[])
{
	int i, j;
	AMessage msg = AMESSAGE__INIT;

	for (j = 0; j < sizeof(datas) / sizeof(datas[0]); ++j)
	{
		for (i = 0; i < msg.base.descriptor->n_fields; ++i) {
//			printf("msg.base.descriptor.fields[i].name = %s, id = %d\n", msg.base.descriptor->fields[i].name, msg.base.descriptor->fields[i].id);
			if (msg.base.descriptor->fields[i].label == PROTOBUF_C_LABEL_REPEATED) {
				if (check_repeat_field(&msg, &msg.base.descriptor->fields[i], &datas[j]) == 0)
					break;
			} else if (strcmp(datas[j].name, msg.base.descriptor->fields[i].name) == 0) {
				printf("find %s, value = %s\n", datas[j].name, datas[j].value);
				copy_data(msg.base.descriptor->fields[i].type,
					msg.base.descriptor->fields[i].offset,
					(char *)&msg,
					datas[j].value);
				break;
			}
		}
	}

	return (0);
}
