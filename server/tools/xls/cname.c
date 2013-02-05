#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "config_name.pb-c.h"
#include "freexl.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

uint8_t outbuf[1024 * 1024 * 5];
char * data[3][1024 * 1024 ];
char tmpvalue[32];
int max_columns;
int max_rows;
int fam_name;
int male_name;
int female_name;


CFGNAME result;

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
    int count=0;
    unsigned short worksheet_index;
    
	memset(data, 0, sizeof(data));	

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

    for(worksheet_index = 0 ; worksheet_index < 3; worksheet_index++)
    {
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
        
        max_columns = columns;
        max_rows = rows;
        count=0;
        
        for (row = 0; row < rows; row++)
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
					data[worksheet_index][count++] = strdup(tmpvalue);
					break;
				case FREEXL_CELL_DOUBLE:
//					printf (", %1.12f", cell.value.double_value);
					sprintf(tmpvalue, "%d", (int)cell.value.double_value);
					data[worksheet_index][count++] = strdup(tmpvalue);					
					break;
				case FREEXL_CELL_TEXT:
				case FREEXL_CELL_SST_TEXT:
//					print_sql_string (cell.value.text_value);
//					printf (", '%s'", cell.value.text_value);
					data[worksheet_index][count++] = strdup(cell.value.text_value);							
					break;
				case FREEXL_CELL_DATE:
				case FREEXL_CELL_DATETIME:
				case FREEXL_CELL_TIME:
//					printf (", '%s'", cell.value.text_value);
					data[worksheet_index][count++] = strdup(cell.value.text_value);					
					break;
				case FREEXL_CELL_NULL:
				default:
//					printf (", NULL");
					break;
                };
            }
        }
        switch( worksheet_index )
            {
            case ( 0 ):
                fam_name = count;
                break;
            case ( 1):
                male_name = count;
                break;
            case ( 2 ):
                female_name = count;
                break;
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

void init_result()
{
	int i;
    cfg__name__init( &result );
	result.n_fam_name = fam_name;
	result.fam_name = malloc(sizeof(char*) * fam_name);
	for (i = 0; i < fam_name; ++i) {
		assert(data[0][i]);
		result.fam_name[i] = data[0][i];
	}
			
	result.n_male_name = male_name;
	result.male_name = malloc(sizeof(char*) * male_name);
	for (i = 0; i < male_name; ++i) {
		assert(data[1][i]);
		result.male_name[i] = data[1][i];
	}
	result.n_female_name = female_name;
	result.female_name = malloc(sizeof(char*) * female_name);
	for (i = 0; i < female_name; ++i) {
		assert(data[2][i]);
		result.female_name[i] = data[2][i];
	}

}

int main(int argc, char *argv[])
{
	size_t size;
	if (argc < 2) {
		printf("please input xls file name\n");
		return (0);
	}
	if (load_excel(argv[1]) != 0)
	{
		printf("load excel fail\n");
		return (0);		
	}
	init_result();
    /*
	size = generate_pb_data(get_data_base);

	size = cfg__npcs__pack(&result, outbuf);	
	*/
	size = cfg__name__pack(&result, outbuf);
	
	int fd = open("name.output", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (write(fd, outbuf, size) != size) {
		perror("write fail\n");
	}
	close(fd);
}

