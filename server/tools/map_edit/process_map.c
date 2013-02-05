#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <assert.h>
#include "../xls/config_instance_map.pb-c.h"

#define STARTPOINTVALUE 2
#define ENDPOINTVALUE   3
#define MAPROW 25
#define MAPCOL 25

int TestMap[MAPROW][MAPCOL]=
{
//	 0 1 2 3 4 5 6 7 8 9 10 11
	{2,1,0,0,0,0,0,0,0,0,0,0}, // 0
	{0,1,1,0,0,0,0,0,0,0,0,0}, // 1
	{0,0,1,1,1,1,0,0,0,0,0,0}, // 2
	{0,0,1,0,0,1,1,0,0,0,0,0}, // 3
	{0,0,1,0,0,0,1,1,0,0,0,0}, // 4
	{0,0,1,0,0,0,0,1,1,0,0,0}, // 5
	{0,0,1,0,0,0,0,0,1,1,0,0}, // 6
	{0,0,1,0,0,0,0,0,0,1,1,0}, // 7
	{0,0,1,0,1,1,1,0,0,0,1,0}, // 8
	{0,0,1,0,1,0,1,0,0,0,1,0}, // 9
	{0,0,1,1,1,0,1,1,1,1,1,0}, // 10
	{0,0,1,0,0,0,0,0,0,0,1,0}, // 11
	{1,0,1,0,0,0,0,0,0,0,1,1}, // 12
	{1,0,1,0,0,0,0,0,0,0,1,1}, // 13
	{1,1,1,0,0,0,0,0,0,0,0,1}, // 14
	{0,0,0,0,0,0,0,0,0,0,0,1}, // 15
	{0,0,0,0,0,0,0,0,0,0,0,3}  // 16
};

/*
int TestMap[MAPROW][MAPCOL]=
{
//	 0 1 2 3 4 5 6 7 8 9 10 11
	{2,1,1,1,1,1,1,3,0,0,0,0}, // 0
	{0,1,0,0,0,0,0,0,0,0,0,0}, // 1
	{0,1,0,0,1,1,0,0,0,0,0,0}, // 2
	{0,0,1,0,0,1,1,0,0,0,0,0}, // 3
	{0,0,1,0,0,0,1,1,0,0,0,0}, // 4
	{0,0,1,0,0,0,0,1,1,0,0,0}, // 5
	{0,0,1,0,0,0,0,0,1,1,0,0}, // 6
	{0,0,1,0,0,0,0,0,0,1,1,0}, // 7
	{0,0,1,0,1,1,1,0,0,0,1,0}, // 8
	{0,0,1,0,1,0,1,0,0,0,1,0}, // 9
	{0,0,1,1,1,0,1,1,1,1,1,0}, // 10
	{0,0,0,0,0,0,0,0,0,0,1,0}, // 11
	{0,0,0,0,0,0,0,0,0,0,1,1}, // 12
	{0,0,0,0,0,0,0,0,0,0,1,1}, // 13
	{0,0,0,0,0,0,0,0,0,0,0,1}, // 14
	{0,0,0,0,0,0,0,0,0,0,0,1}, // 15
	{0,0,0,0,0,0,0,0,0,0,0,0}  // 16
};
*/
typedef struct point
{
	int x;
	int y;
} POINT;
#define MAX_RESULT_PATH (1024)
#define MAX_RESULT_NUM (128)

int GetEndPoint(int map[MAPROW][MAPCOL], POINT *start, POINT *end, int maprow, int mapcol)
{
	int i, j;
	for(i = 0; i < maprow; i++) {
		for(j = 0; j < mapcol; j++)
		{
			if ((map[i][j] & 0xff0000) == ENDPOINTVALUE)
			{
				end->x = i;
				end->y = j;
			}
			if ((map[i][j] & 0xff0000) == STARTPOINTVALUE)
			{
				start->x = i;
				start->y = j;
			}
			
		}
	}
	return 0;
}

//int  GetEndPoint(int *map,int *row,int *col, int maprow, int mapcol);
//int  FillResult(int **map,int *row,int *col, int maprow, int mapcol,int **result);
/*   
int Result[MAPROW][MAPCOL];
int EndPointR;
int EndPointC;
int StartPointR;
int StartPointC;

int check_left(int map[MAPROW][MAPCOL],int row,int col, int distance,int result[MAPROW][MAPCOL]);
int check_right(int map[MAPROW][MAPCOL],int row,int col, int distance,int result[MAPROW][MAPCOL]);
int check_up(int map[MAPROW][MAPCOL],int row,int col, int distance,int result[MAPROW][MAPCOL]);
int check_down(int map[MAPROW][MAPCOL],int row,int col, int distance,int result[MAPROW][MAPCOL]);	

int check_up(int map[MAPROW][MAPCOL],int row,int col, int distance,int result[MAPROW][MAPCOL])
{
	if (row == 0)
		return (0);
	--row;
	if (map[row][col] == 0)
		return (0);
	if (result[row][col] <= distance)
		return (0);
	result[row][col] = distance;
	++distance;
	
	check_left(map, row, col, distance, result);
	check_down(map, row, col, distance, result);
	check_right(map, row, col, distance, result);
	check_up(map, row, col, distance, result);		
	return (0);
}

int check_down(int map[MAPROW][MAPCOL],int row,int col, int distance,int result[MAPROW][MAPCOL])
{
	if (row == MAPROW - 1)
		return (0);
	++row;
	if (map[row][col] == 0)
		return (0);
	if (result[row][col] <= distance)
		return (0);
	result[row][col] = distance;
	++distance;
	
	check_left(map, row, col, distance, result);
	check_down(map, row, col, distance, result);
	check_right(map, row, col, distance, result);
	check_up(map, row, col, distance, result);		
	return (0);
}

int check_right(int map[MAPROW][MAPCOL],int row,int col, int distance,int result[MAPROW][MAPCOL])
{
	if (col == MAPCOL - 1)
		return (0);	
	++col;

	if (map[row][col] == 0)
		return (0);	
	if (result[row][col] <= distance)
		return (0);
	result[row][col] = distance;
	++distance;
	
	check_left(map, row, col, distance, result);
	check_down(map, row, col, distance, result);
	check_right(map, row, col, distance, result);
	check_up(map, row, col, distance, result);		
	return (0);
}

int check_left(int map[MAPROW][MAPCOL],int row,int col, int distance,int result[MAPROW][MAPCOL])
{
	if (col == 0)
		return (0);

	--col;
	if (map[row][col] == 0)
		return (0);
	
	if (result[row][col] <= distance)
		return (0);
	result[row][col] = distance;
	++distance;
	
	check_left(map, row, col, distance, result);
	check_down(map, row, col, distance, result);
	check_right(map, row, col, distance, result);
	check_up(map, row, col, distance, result);		
	return (0);
}

int FillResult(int map[MAPROW][MAPCOL], POINT *start, POINT *end, int result[MAPROW][MAPCOL])
{
	int distance = 1;
	int i, j;

	for (i = 0; i < MAPROW; ++i) {
		for (j = 0; j < MAPCOL; ++j) {
			result[i][j] = INT_MAX;
		}
	}

//	result[target_row][target_col] = 0;
	
	check_left(map, target_row, target_col, distance, result);
	check_down(map, target_row, target_col, distance, result);
	check_right(map, target_row, target_col, distance, result);
	check_up(map, target_row, target_col, distance, result);

	for (i = 0; i < MAPROW; ++i) {
		for (j = 0; j < MAPCOL; ++j) {
			if (result[i][j] == INT_MAX)
				printf("#####");
			else
				printf("%04d ", result[i][j]);
		}
		printf("\n");
	}
	printf("\n");
		
	return 0;
}   
*/

int check_can_through(int map[MAPROW][MAPCOL], POINT *point, POINT result[MAX_RESULT_PATH], int n)
{
	int i = 0;
//	if (point->x == INT_MAX)
//		return (0);

	assert(point);
	assert(point->x != INT_MAX);

	if (point->x < 0 || point->x >= MAPCOL)
		return (0);

	if (point->y < 0 || point->y >= MAPROW)
		return (0);	
	
	if (map[point->x][point->y] == 0)
		return (0);

	for (i = 0; i < n; ++i) {
		assert(result[i].x != INT_MAX);
		if (result[i].x == point->x && result[i].y == point->y)
			return (0);
	}
	
	return (1);
}

void create_new_path(POINT result[MAX_RESULT_NUM][MAX_RESULT_PATH], int *num, POINT *point, int n)
{
	assert(*num < MAX_RESULT_NUM - 1);
	assert(n < MAX_RESULT_PATH);
	++(*num);
	memcpy(result[*num], result[*num - 1], sizeof(result[0][0]) * n);
	result[*num][n].x = point->x;
	result[*num][n].y = point->y;	
}

int check_point(int map[MAPROW][MAPCOL], POINT result[MAX_RESULT_NUM][MAX_RESULT_PATH], int *num, POINT *point, int n)
{
	int find = 0;
	POINT left, right, up, down;

	if (map[point->x][point->y] == ENDPOINTVALUE) {
		result[*num][n].x = INT_MAX;
		result[*num][n].y = INT_MAX;		
		return (0);
	}
	
	left.x = point->x;
	left.y = point->y - 1;
	right.x = point->x;
	right.y = point->y + 1;
	up.x = point->x - 1;
	up.y = point->y;
	down.x = point->x + 1;
	down.y = point->y;

	if (check_can_through(map, &left, result[*num], n)) {
		result[*num][n].x = left.x;
		result[*num][n].y = left.y;
		if (check_point(map, result, num, &left, n+1) == 0) {
			++find;
		}
	}
	
	if (check_can_through(map, &right, result[*num], n)) {
		if (!find) {
			result[*num][n].x = right.x;
			result[*num][n].y = right.y;
		} else {
			create_new_path(result, num, &right, n);
		}
		++find;
		if (check_point(map, result, num, &right, n+1) != 0) {
			--find;
			if (find)
				--(*num);
		}
	}

	
	if (check_can_through(map, &up, result[*num], n)) {
		if (!find) {
			result[*num][n].x = up.x;
			result[*num][n].y = up.y;
		} else {
			create_new_path(result, num, &up, n);
		}
		++find;
		if (check_point(map, result, num, &up, n+1) != 0) {
			--find;
			if (find)
				--(*num);
		}
	}


	if (check_can_through(map, &down, result[*num], n)) {
		if (!find) {
			result[*num][n].x = down.x;
			result[*num][n].y = down.y;
		} else {
			create_new_path(result, num, &down, n);
		}
		++find;
		if (check_point(map, result, num, &down, n+1) != 0) {
			--find;
			if (find)
				--(*num);
		}
	}

	if (!find) {
//		printf("err, no way\n");
		return (-1);
	}
	return (0);
}

void print_result(POINT result[MAX_RESULT_PATH], CFGINSTANCEMAPNODE *node)
{
	int i;
	for (i = 0; i < MAX_RESULT_PATH; ++i) {
		if (result[i].x == INT_MAX) {
			printf("\n");
			return;
		}
		printf("%02d|%02d ", result[i].x, result[i].y);
	}
}

static CFGINSTANCEMAPNODE node_pool[MAX_RESULT_NUM];
static CFGINSTANCEMAPNODE *point_pool[MAX_RESULT_NUM];

CFGINSTANCEMAPNODE *get_new_node(CFGINSTANCEMAP *map)
{
	int i = map->n_node;
	if (i >= MAX_RESULT_NUM - 1)
		return (NULL);
	point_pool[i] = &node_pool[i];
	++map->n_node;
	return (point_pool[i]);
}

CFGINSTANCEMAPNODE *get_node(int id, CFGINSTANCEMAP *map)
{
	int i;
	for (i = 0; i < map->n_node; ++i) {
		if (map->node[i]->id == id)
			return (map->node[i]);
	}
	return get_new_node(map);
}

void create_map_data(CFGINSTANCEMAP *map, POINT result[MAX_RESULT_PATH])
{
	int i;
	int id;
	CFGINSTANCEMAPNODE *node;
	
	for (i = 0; i < MAX_RESULT_PATH; ++i) {
		if (result[i].x == INT_MAX) {
//			printf("\n");
			return;
		}
		id = (result[i].x << 8 | result[i].y);
		node = get_node(id, map);
		node->id = id;
//		printf("%02d|%02d ", result[i].x, result[i].y);
	}
}

int main(int argc, char *argv[])
{
	CFGINSTANCEMAP map;
	int i, j;
	POINT start, end;
	int num_result = 0;
	POINT result[MAX_RESULT_NUM][MAX_RESULT_PATH];

	for (i = 0; i < MAX_RESULT_NUM; ++i) {
		for (j = 0; j < MAX_RESULT_PATH; ++j) {
			result[i][j].x = INT_MAX;
			result[i][j].y = INT_MAX;
		}
	}

	GetEndPoint(TestMap, &start, &end, MAPROW, MAPCOL); 

	printf("EndPointR is %d %d,EndPointC is %d %d \n", start.x, start.y, end.x, end.y);

	result[0][0].x = start.x;
	result[0][0].y = start.y;

	check_point(TestMap, result, &num_result, &start, 1);

	cfg__instance__map__init(&map);
	map.node = point_pool;

	for (i = 0; i < num_result; ++i) {
		create_map_data(&map, result[i]);
	}
/*	
	for (i = 0; i <= num_result; ++i) {
		point[i] = &node[i];
		cfg__instance__map__node__init(&node[i]);
		print_result(result[i], &node[i]);
	}
*/	
//	FillResult(TestMap, &start, &end, Result);
	
		//  FillResult((char *)&TestMap,(int *)&EndPointR,(int *)&EndPointC,(char *)&Result);
	return 0;
}
