#include <assert.h>
#include <stdio.h>
#include "sortarray.h"
#include "client_map.h"

POOL_DEF(client_map)

int	max_client_map_in_game;

void init_game_client_map(int num)
{
	max_client_map_in_game = num;
}

void init_client_map_data(CLIENT_MAP *client)
{
	assert(client);
	rb_init_node(&client->node_fd);	
}
int resume_client_map_data(CLIENT_MAP *client)
{
	assert(client);
	return (0);
}

CLIENT_MAP *get_first_client_map(struct rb_root *head)
{
	if (unlikely(!head->rb_node))
		return NULL;
	return rb_entry(head->rb_node, CLIENT_MAP, node_fd);
}

CLIENT_MAP *get_next_client_map(CLIENT_MAP *client, int left)
{
	struct rb_node *p;
	if (unlikely(!client))
		return NULL;
	if (left)
		p = client->node_fd.rb_left;
	else
		p = client->node_fd.rb_right;
	if (!p)
		return NULL;
	
	return rb_entry(p, CLIENT_MAP, node_fd);
}

CLIENT_MAP *get_client_map_by_fd(uint16_t fd, struct rb_root *head)
{
	struct rb_node **p = &head->rb_node;
	struct rb_node *parent = NULL;
	CLIENT_MAP  *ptr;
	while (*p) {
		parent = *p;
		ptr = rb_entry(parent, CLIENT_MAP, node_fd);
		if (ptr->fd == fd)
			return (ptr);
		else if (fd < ptr->fd)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}
	return (NULL);
}

CLIENT_MAP *add_client_map(CLIENT_MAP *client, struct rb_root *head)
{
	assert(client);
	struct rb_node **p;
	struct rb_node *parent;
	CLIENT_MAP  *ptr;

	p = &head->rb_node;
	parent = NULL;

	while (*p) {
		parent = *p;
		ptr = rb_entry(parent, CLIENT_MAP, node_fd);
		if (client->fd == ptr->fd)
			return (NULL);
		else if (client->fd < ptr->fd)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}
	rb_link_node(&client->node_fd, parent, p);
	rb_insert_color(&client->node_fd, head);
	return client;
}

int del_client_map_by_fd(uint16_t fd, struct rb_root *head)
{
	CLIENT_MAP *client;
	client = get_client_map_by_fd(fd, head);
	if (!client)
		return (-1);
	rb_erase(&client->node_fd, head);
	RB_CLEAR_NODE(&client->node_fd);
	
	return (0);
}
