#include <lib/list.h>
#include <kernel/heap.h>

node_t* create_list(void* data) {
    node_t* new_node = malloc(sizeof(node_t));
    if(new_node == NULL) return NULL;
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

void push_back(node_t** head, void* data) {
    node_t* new_node = malloc(sizeof(node_t));
    if(new_node == NULL) return;
    
    new_node->data = data;
    new_node->next = NULL;
    
    if(*head == NULL) {
        *head = new_node;
        return;
    }
    
    node_t* current = *head;
    while(current->next != NULL) {
        current = current->next;
    }
    
    current->next = new_node;
}

void free_list(node_t** head) {
    if(*head == NULL) return;
    
    node_t* current = *head;
    node_t* next_node;
    
    while(current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    
    *head = NULL;
}