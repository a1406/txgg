#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "google/protobuf-c/protobuf-c.h"

int generate_pb(void *base, const ProtobufCMessageDescriptor *desc, const ProtobufCFieldDescriptor *field, char *base_name, char *value, char *data_name)
{
	char new_name[256];
	ProtobufCMessageDescriptor *new_desc;
	int i;

	if (field->type == PROTOBUF_C_TYPE_MESSAGE)
	{
		new_desc = (ProtobufCMessageDescriptor *)field->descriptor;
		if (!new_desc)
		{
			printf("parse err, maybe a bug\n");
			exit(0);
		}
		if (base_name)
			sprintf(new_name, "%s_%s", base_name, field->name);
		else
			sprintf(new_name, "%s", field->name);
		for (i = 0; i < new_desc->n_fields; ++i) {
			if (generate_pb(*(*(void ***)(base + field->offset) + (*(int *)(base + field->quantifier_offset))), new_desc, &new_desc->fields[i], new_name, value, data_name) == 0)
			{
				if (i == new_desc->n_fields - 1 && field->label == PROTOBUF_C_LABEL_REPEATED)
					*(int *)(base + field->quantifier_offset) += 1;
				return (0);
			}
		}
	}

	if (base_name)
		sprintf(new_name, "%s_%s", base_name, field->name);
	else
		sprintf(new_name, "%s", field->name);
	if (strcmp(data_name, new_name) != 0)
		return (1);


	if (field->label == PROTOBUF_C_LABEL_REPEATED) {
//	check_repeat_field(base, field, new_name
		copy_repeat_data(field->type,
						field->offset,
						(char *)base,
						value,
						(int *)((char *)base + field->quantifier_offset));
	}
	else
	{
	base = base + field->offset;
		
	switch (field->type)
	{
		case PROTOBUF_C_TYPE_INT32:
		case PROTOBUF_C_TYPE_SINT32:
		case PROTOBUF_C_TYPE_SFIXED32:
			i = atoi(value);
			memcpy(base, &i, 4);
			break;
		case PROTOBUF_C_TYPE_STRING:
			*(char **)(base) = strdup(value);
			break;
		default:
			printf("invalid type\n");
			return;
	}
	}
	printf("find %s %s\n", data_name, value);

	return (0);
}
