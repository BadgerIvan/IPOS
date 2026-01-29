#ifndef _LIST_H
#define _LIST_H

typedef struct node node_t;

typedef struct node {
    void* data;
    node_t* next;
};

node_t* create_list(void* data);
void push_back(node_t** head, void* data);
void free_list(node_t** head);

#endif