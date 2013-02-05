#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "rbtree.h"
#include "sortarray.h"

struct data_node {
	struct rb_node node;
	int key;
	int index;
	char name[32];
};

struct data_head {
	struct rb_root head;
//	struct data_node *next;
	comp_func comp;	
};

static struct data_head root_head;

static inline void data_init(struct data_node *node)
{
	rb_init_node(&node->node);
}

inline void data_init_head(struct data_head *head, comp_func comp)
{
	head->head = RB_ROOT;
//	head->next = NULL;
	head->comp = comp;
}

struct data_node *data_search(struct data_head *head, struct data_node *data)
{
	struct rb_node **p = &head->head.rb_node;
	struct rb_node *parent = NULL;
	struct data_node  *ptr;
	int ret;

	/* Make sure we don't add nodes that are already added */
//	WARN_ON_ONCE(!RB_EMPTY_NODE(&node->node));

	while (*p) {
		parent = *p;
		ptr = rb_entry(parent, struct data_node, node);
		ret = head->comp((void *)data, (void *)ptr);
		if (ret == 0)
			return (ptr);
		else if (ret < 0)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}
	return (NULL);
}

/**
 * timerqueue_add - Adds timer to timerqueue.
 *
 * @head: head of timerqueue
 * @node: timer node to be added
 *
 * Adds the timer node to the timerqueue, sorted by the
 * node's expires value.
 */
int data_add(struct data_head *head, struct data_node *node)
{
	struct rb_node **p = &head->head.rb_node;
	struct rb_node *parent = NULL;
	struct data_node  *ptr;
	int ret;
	/* Make sure we don't add nodes that are already added */
//	WARN_ON_ONCE(!RB_EMPTY_NODE(&node->node));

	while (*p) {
		parent = *p;
		ptr = rb_entry(parent, struct data_node, node);
		ret = head->comp(node, ptr);
		if (ret == 0)
			return (-1);
		else if (ret < 0)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}
	rb_link_node(&node->node, parent, p);
	rb_insert_color(&node->node, &head->head);
/*
	if (!head->next || node->key < head->next->key)
		head->next = node;
*/		
	return (0);
}

/**
 * timerqueue_del - Removes a timer from the timerqueue.
 *
 * @head: head of timerqueue
 * @node: timer node to be removed
 *
 * Removes the timer node from the timerqueue.
 */
void data_del(struct data_head *head, struct data_node *node)
{
//	WARN_ON_ONCE(RB_EMPTY_NODE(&node->node));

	/* update next pointer */
/*	
	if (head->next == node) {
		struct rb_node *rbn = rb_next(&node->node);

		head->next = rbn ?
			rb_entry(rbn, struct data_node, node) : NULL;
	}
*/	
	rb_erase(&node->node, &head->head);
	RB_CLEAR_NODE(&node->node);
}

int test_comp(const void *a, const void *b)
{
	if (((struct data_node *)a)->key == ((struct data_node *)b)->key)
		return (0);
	if (((struct data_node *)a)->key < ((struct data_node *)b)->key)
		return (-1);
	return (1);
}

int test_comp_v2(const void *a, const void *b)
{
	char *name1 = ((struct data_node *)a)->name;
	char *name2 = ((struct data_node *)b)->name;	

	return strcmp(name1, name2);
}


int main(int argc, char *argv[])
{
	int i, ret;
	struct data_node datas[20];
	struct data_node *p;
	struct data_node tmp;	
	srand(getpid());
	data_init_head(&root_head, test_comp_v2);
	for (i = 0; i < 20; ++i) {
		datas[i].key = rand() % 20;
		sprintf(datas[i].name, "name_%d", datas[i].key);
		datas[i].index = i;
		ret = data_add(&root_head, &datas[i]);
		printf("add %d, ret %d\n", datas[i].key, ret);
	}

	for (i = 0; i < 20; ++i) {
		sprintf(tmp.name, "name_%d", i);		
		p = data_search(&root_head, &tmp);
		printf("search %d, ret %p", i, p);
		if (p)
			printf("  p's index = %d, key = %d", p->index, p->key);
		printf("\n");
	}

	printf("=================\n");
	for (i = 0; i < 20; ++i) {
		sprintf(tmp.name, "name_%d", i);		
		p = data_search(&root_head, &tmp);
		if (p)
			data_del(&root_head, p);
	}

	for (i = 0; i < 20; ++i) {
		sprintf(tmp.name, "name_%d", i);		
		p = data_search(&root_head, &tmp);
		printf("search %d, ret %p", i, p);
		if (p)
			printf("  p's index = %d, key = %d", p->index, p->key);
		printf("\n");
	}
	
	return (0);
}
