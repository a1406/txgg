#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct dataconvert
{
    char configname[ 20 ];
    int ( * handle_func)(int rows, int columns);
}dataconvert__t;

