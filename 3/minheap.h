#pragma once

#include <stdlib.h>

struct heapnode {
    int key; 
    char *value; 
};

struct heap {
    int maxsize; 
    int nnodes; 
    struct heapnode *nodes; 
};

struct heap *heap_create(int maxsize);
void heap_free(struct heap *h);
struct heapnode *heap_min(struct heap *h);
int heap_insert(struct heap *h, int key, char *value);
struct heapnode heap_extract_min(struct heap *h);
int heap_decrease_key(struct heap *h, int index, int newkey);
int heap_delete(struct heap *h, int index);