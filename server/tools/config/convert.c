#include "google/protobuf-c/protobuf-c.h"
/*Follow header file need to update when need to deal new config sheet*/
#include "config_attribute.pb-c.h"
#include "config_drop.pb-c.h"
#include "config_instance_map.pb-c.h"
#include "config_instance.pb-c.h"
#include "config_item.pb-c.h"
#include "config_keyword.pb-c.h"
#include "config_map_function.pb-c.h"
#include "config_NPC.pb-c.h"
#include "config_player.pb-c.h"
#include "config_quest_main.pb-c.h"
#include "config_quest_branch.pb-c.h"
#include "config_scene.pb-c.h"
#include "config_skill.pb-c.h"
#include "config_map.pb-c.h"

#include "convert.h"
int max_rows;
int max_columns;
int row_num_config;

extern char **data_not_format;

typedef ProtobufCMessage *(*func_get_data_base)(int line, void * result);
void copy_repeat_data(ProtobufCType type, unsigned offset, char *base, char *data, int *index);

/*common part of code*/
uint8_t outbuf[1024 * 1024 * 5];
int initialize_conv_table(void);
int execute_data_conv(const char * sheet_name, int rows, int cols );
dataconvert__t * data_conver_table =0;

int generate_pb(void *base, const ProtobufCMessageDescriptor *desc,
                const ProtobufCFieldDescriptor *field,
                char *base_name, char *value, char *data_name);
void copy_repeat_data(ProtobufCType type, unsigned offset,
                      char *base, char *data, int *index);

int handle_npc_func( int rows, int columns );
int handle_skill_func( int rows, int columns );
int handle_attribute_func( int rows, int columns );
int handle_drop_func( int rows, int columns );
int handle_item_func( int rows, int columns );
int handle_map_function_func( int rows, int columns );
int handle_quest_elite_func( int rows, int columns );
int handle_quest_main_func( int rows, int columns );
int handle_quest_cycle_func( int rows, int columns );
int handle_quest_branch_func( int rows, int columns );
int handle_player_func( int rows, int columns );
int handle_map_func( int rows, int columns );

int initialize_conv_table(void)
{
    int len = 0;
/*Supported convert sheet type*/
    dataconvert__t  tmptable[]={
        /* "configname", init function, get data function,
           pack function,point to CFG**S, sizeof( CFG**S )*/
        {"npc", handle_npc_func},
        {"skill_active", handle_skill_func},
        {"attribute", handle_attribute_func},
        {"drop", handle_drop_func},
        {"item", handle_item_func},
        {"map_function", handle_map_function_func},
        {"quest_main", handle_quest_main_func},
        {"player", handle_player_func},
        {"map", handle_map_func},
        {"quest_branch", handle_quest_branch_func},
/*

        {"quest_cycle", handle_quest_cycle_func},
                {"quest_elite", handle_quest_elite_func},
*/
        {"", NULL}
    };
    len = sizeof( tmptable);
    if( !(data_conver_table = ( dataconvert__t * )malloc( len ) ))
    {
        printf( "Malloc space %d for data_conver_table fail!\n" ,len);
        return -1;
    }
    memmove( data_conver_table,&tmptable, len );

    return 0;
}

int execute_data_conv( const char * sheet_name, int rows, int cols )
{
    int ret;

    max_rows = rows - 1;
    max_columns = cols;
    
    dataconvert__t * tableptr =0 ;
    
    tableptr = data_conver_table;

    while ( ( strcmp ( tableptr->configname, sheet_name )  != 0 )
            && ( tableptr->handle_func != NULL ))
        tableptr ++;

    if(  tableptr->handle_func == NULL)
        return -10;
    
    memset ( outbuf, 0x0, sizeof( outbuf ) );
    ret = tableptr->handle_func(rows,cols);
    if (ret != 0 )
    {
        fprintf (stderr, " convert %s error: %d\n", tableptr->configname , ret);
        return -1;
    }
    return 0;
}

int generate_pb_data(func_get_data_base get_data_base, void * cfg_s)
{
	size_t size;
	int i, config_line;
	int line, j;
	ProtobufCMessage *base;
    char * dataptr, * nameptr;
    
	for (line = 1; line < max_rows; ++line)
	{
        config_line = line -1;
		base = get_data_base(config_line , cfg_s);
		for (j = 0; j < max_columns; ++j)
		{
            dataptr = *(data_not_format+j +max_columns*line);
            nameptr =  *(data_not_format+j);
			if (!dataptr)
				continue;
			for (i = 0; i < base->descriptor->n_fields; ++i)
			{
				if (base->descriptor->fields[i].label == PROTOBUF_C_LABEL_REPEATED)
                {
					if (*(void **)((char *)base + base->descriptor->fields[i].offset) == NULL)
                    {
						assert(0);
					}
					
				}
				
				if (0 == generate_pb(&base->descriptor, base->descriptor,
                                     &base->descriptor->fields[i], NULL, dataptr, nameptr))
                {
					break;
				}
			}
		}
	}
	return (0);
}

int generate_pb(void *base, const ProtobufCMessageDescriptor *desc,
                const ProtobufCFieldDescriptor *field, char *base_name,
                char *value, char *data_name)
{
	char new_name[256];
	ProtobufCMessageDescriptor *new_desc;
	int i;
	uint32_t ul;

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
			if (generate_pb(*(*(void ***)(base + field->offset) + (*(int *)(base + field->quantifier_offset))),
                            new_desc, &new_desc->fields[i], new_name, value, data_name) == 0)
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
		case PROTOBUF_C_TYPE_UINT32:
			ul = strtoul(value, NULL, 0);
			memcpy(base, &ul, 4);
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
void copy_repeat_data(ProtobufCType type, unsigned offset, char *base, char *data, int *index)
{
	int i;
	uint32_t ul;
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
		case PROTOBUF_C_TYPE_UINT32:
			p = (*addr + 4 * (*index));			
			ul = strtoul(data, NULL, 0);
			memcpy(p, &ul, 4);			
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

ProtobufCMessage *npc_get_data_base(int line, void * result)
{
	int i;
	CFGNPC *msg;
	msg = ((CFGNPCS *)result)->npc[line] = malloc(sizeof(CFGNPC));
	cfg__npc__init(msg);

	msg->func = malloc(sizeof(int32_t) * 50);
    msg->drop = malloc(sizeof(int32_t) * 50);
        
	return &msg->base;
}

int handle_npc_func( int rows, int columns )
{
    int ret =0;
    size_t size;
    CFGNPCS  result;

    row_num_config = rows -2;
    cfg__npcs__init(&result);
	result.n_npc = row_num_config;
	result.npc = malloc(sizeof(void *) *  row_num_config);

    size = generate_pb_data(npc_get_data_base,&result);

    printf( "Congratulations, you had reach here!\n" );

    size = cfg__npcs__pack(&result, outbuf);	

    CFGNPCS *check = cfg__npcs__unpack(NULL,size,outbuf);
    
    int fd = open("npc.output", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
   
    if (write(fd, outbuf, size) != size) {
        perror("write fail\n");
    }
    close(fd);

    return ret;
}

ProtobufCMessage *skill_get_data_base(int line, void * result)
{
	int i;
	CFGSKILL *msg;
	msg = ((CFGSKILLS *)result)->skill[line] = malloc(sizeof(CFGSKILL));
	cfg__skill__init(msg);
	return &msg->base;
}

int handle_skill_func( int rows, int columns )
{
    int ret =0;
    size_t size;
    CFGSKILLS  result;

    row_num_config = rows -2;
    cfg__skills__init(&result);
	result.n_skill = row_num_config;
	result.skill = malloc(sizeof(void *) *  row_num_config);

    size = generate_pb_data(skill_get_data_base,&result);

    printf( "Congratulations, you had reach here!\n" );

    size = cfg__skills__pack(&result, outbuf);	

    CFGSKILLS *check = cfg__skills__unpack(NULL,size,outbuf);
    
    int fd = open("skill_active.output", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
   
    if (write(fd, outbuf, size) != size) {
        perror("write fail\n");
    }
    close(fd);

    return ret;
}

ProtobufCMessage *attribute_get_data_base(int line, void * result)
{
	int i;
	CFGATTRIBUTE *msg;
	msg = ((CFGATTRIBUTES *)result)->attribute[line] = malloc(sizeof(CFGATTRIBUTE));
	cfg__attribute__init(msg);
	return &msg->base;
}

int handle_attribute_func( int rows, int columns )
{
	size_t size;
    CFGATTRIBUTES result;

    row_num_config = rows -2;
	cfg__attributes__init(&result);
	result.n_attribute =  row_num_config;
	result.attribute = malloc(sizeof(void *) * row_num_config);

	size = generate_pb_data(attribute_get_data_base,&result);

	size = cfg__attributes__pack(&result, outbuf);

	CFGATTRIBUTES *check = cfg__attributes__unpack(NULL, size, outbuf);	
	
	int fd = open("attribute.output", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
	return (0);
    
}

ProtobufCMessage *drop_get_data_base(int line, void * result)
{
	int i;
	CFGDROP *msg;
	msg = ((CFGDROPS *)result)->drop[line] = malloc(sizeof(CFGDROP));
	cfg__drop__init(msg);
	msg->item = malloc(sizeof(void *) * 50);
	for (i = 0; i < 30; ++i)
	{
		msg->item[i] = malloc(sizeof(DropItem));
		drop_item__init(msg->item[i]);
	}

	return &msg->base;
}

int handle_drop_func( int rows, int columns )
{
    CFGDROPS result;
    size_t size;

    row_num_config = rows -2;
    cfg__drops__init(&result);
	result.n_drop = row_num_config;
	result.drop = malloc(sizeof(void *) * row_num_config);
    
    size = generate_pb_data(drop_get_data_base, &result);
    
	size = cfg__drops__pack(&result, outbuf);
    
	CFGDROPS *check = cfg__drops__unpack(NULL, size, outbuf);	
	
	int fd = open("drop.output", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
	return (0);    
}

ProtobufCMessage *item_get_data_base(int line, void * result)
{
	int i;
	CFGITEM *msg;
	msg =(( CFGITEMS * ) result)->item[line] = malloc(sizeof(CFGITEM));
	cfg__item__init(msg);
	return &msg->base;
}

int handle_item_func( int rows, int columns )
{
    CFGITEMS result;
    size_t size;

    row_num_config = rows -2;
    cfg__items__init(&result);
	result.n_item = row_num_config;
	result.item = malloc(sizeof(void *) * row_num_config);

    size = generate_pb_data(item_get_data_base,&result);

	size = cfg__items__pack(&result, outbuf);

	CFGITEMS *check = cfg__items__unpack(NULL, size, outbuf);	
	
	int fd = open("item.output", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
	return (0);
    
}

ProtobufCMessage *map_function_get_data_base(int line, void * result)
{
	int i;
	CFGMAPFUNCTION *msg;
	msg = ((CFGMAPFUNCTIONS *)result)->map_function[line]
        = malloc(sizeof(CFGMAPFUNCTION));
	cfg__map__function__init(msg);

    msg->drop = malloc(sizeof(int32_t) * 50);
    
	return &msg->base;
}

int handle_map_function_func( int rows, int columns )
{
	size_t size;
    CFGMAPFUNCTIONS result;

    row_num_config = rows -2;
	cfg__map__functions__init(&result);
	result.n_map_function =     row_num_config ;
	result.map_function = malloc(sizeof(void *) * row_num_config );
    
    size = generate_pb_data(map_function_get_data_base, &result);
       
	size = cfg__map__functions__pack(&result, outbuf);

	CFGMAPFUNCTIONS *check = cfg__map__functions__unpack(NULL, size, outbuf);	
	
	int fd = open("mapfunction.output", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
	return (0);
    
}

ProtobufCMessage *quest_main_get_data_base(int line, void * result)
{
	int i;
	CFGMAINQUEST *msg;
	msg = ((CFGMAINQUESTS *)result)->quest[line] = malloc(sizeof(CFGMAINQUEST));
	cfg__main__quest__init(msg);

	return &msg->base;
}

int handle_quest_main_func( int rows, int columns )
{
	size_t size;
    CFGMAINQUESTS result;

    row_num_config = rows -2;
    cfg__main__quests__init(&result);
	result.n_quest =     row_num_config ;
	result.quest = malloc(sizeof(void *)  *    row_num_config );

    size = generate_pb_data(quest_main_get_data_base, &result);

	size = cfg__main__quests__pack(&result, outbuf);

	CFGMAINQUESTS *check = cfg__main__quests__unpack(NULL, size, outbuf);	
	
	int fd = open("quest_main.output", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
	return (0);

}

ProtobufCMessage *player_get_data_base(int line, void * result)
{
	int i;
	CFGPLAYER *msg;
	msg = ((CFGPLAYERS *)result)->player[line] = malloc(sizeof(CFGPLAYER));
	cfg__player__init(msg);
	return &msg->base;
}

int handle_player_func( int rows, int columns )
{
	size_t size;
    CFGPLAYERS result;

    row_num_config = rows -2;
    cfg__players__init(&result);
	result.n_player =  row_num_config;
	result.player = malloc(sizeof(void *) *  row_num_config);

    size = generate_pb_data(player_get_data_base, &result);

	size = cfg__players__pack(&result, outbuf);	
	
	int fd = open("player.output", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
}

ProtobufCMessage *map_get_data_base(int line, void * result)
{
	int i;
	CFGMAP *msg;
	msg = ((CFGMAPS *)result)->map[line] = malloc(sizeof(CFGMAP));
	cfg__map__init(msg);
	return &msg->base;
}

int handle_map_func( int rows, int columns )
{
	size_t size;
    CFGMAPS result;

    row_num_config = rows -2;
    cfg__maps__init(&result);
	result.n_map =  row_num_config;
	result.map = malloc(sizeof(void *) *  row_num_config);

    size = generate_pb_data(map_get_data_base, &result);

	size = cfg__maps__pack(&result, outbuf);	
	
	int fd = open("map.output", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
}

ProtobufCMessage *quest_branch_get_data_base(int line, void * result)
{
	int i;
	CFGBRANCHQUEST *msg;
	msg = ((CFGBRANCHQUESTS *)result)->quest[line] = malloc(sizeof(CFGBRANCHQUEST));
	cfg__branch__quest__init(msg);

	return &msg->base;
}

int handle_quest_branch_func( int rows, int columns )
{
	size_t size;
    CFGBRANCHQUESTS result;

    row_num_config = rows -2;
    cfg__branch__quests__init(&result);
	result.n_quest =     row_num_config ;
	result.quest = malloc(sizeof(void *)  *    row_num_config );

    size = generate_pb_data(quest_branch_get_data_base, &result);

	size = cfg__branch__quests__pack(&result, outbuf);

	CFGBRANCHQUESTS *check = cfg__branch__quests__unpack(NULL, size, outbuf);	
	
	int fd = open("quest_branch.output", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
	return (0);

}
