#include "minheap.h"

struct heap *heap_create(int maxsize) {
    struct heap *h;
    h = malloc(sizeof(*h));
    if (h != NULL) {
        h->maxsize = maxsize;
        h->nnodes = 0;
        h->nodes = malloc(sizeof(*h->nodes) * (maxsize + 1));
        if (h->nodes == NULL) {
            free(h);
            return NULL;
        }
    }
    return h;
}

void heap_free(struct heap *h) {
    free(h->nodes);
    free(h);
}

void heap_swap(struct heapnode *a, struct heapnode *b) {
    struct heapnode temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

struct heapnode *heap_min(struct heap *h) {
    if (h->nnodes == 0)
        return NULL;
    return &h->nodes[1];
}

int heap_insert(struct heap *h, int key, char *value) {
    if (h->nnodes >= h->maxsize) 
        return -1;
    
    h->nnodes++;
    h->nodes[h->nnodes].key = key;
    h->nodes[h->nnodes].value = value;
    
    for (int i = h->nnodes; i > 1 && h->nodes[i].key < h->nodes[i / 2].key; i = i / 2)
        heap_swap(&h->nodes[i], &h->nodes[i / 2]);
    
    return 0;
}

void heap_heapify(struct heap *h, int index) {
    while (1) {
        int left = 2 * index;
        int right = 2 * index + 1;
        int smallest = index;
        
        if (left <= h->nnodes && h->nodes[left].key < h->nodes[smallest].key)
            smallest = left;
        if (right <= h->nnodes && h->nodes[right].key < h->nodes[smallest].key)
            smallest = right;
        
        if (smallest == index)
            break;
            
        heap_swap(&h->nodes[index], &h->nodes[smallest]);
        index = smallest;
    }
}

struct heapnode heap_extract_min(struct heap *h) {
    if (h->nnodes == 0)
        return (struct heapnode){0, NULL};
    
    struct heapnode minnode = h->nodes[1];
    h->nodes[1] = h->nodes[h->nnodes];
    h->nnodes--;
    heap_heapify(h, 1);
    
    return minnode;
}

int heap_decrease_key(struct heap *h, int index, int newkey) {
    if (index < 1 || index > h->nnodes)
        return -1;
    
    if (h->nodes[index].key <= newkey) 
        return -1;
    
    h->nodes[index].key = newkey;
    
    while (index > 1 && h->nodes[index].key < h->nodes[index / 2].key) {
        heap_swap(&h->nodes[index], &h->nodes[index / 2]);
        index /= 2;
    }
    
    return index;
}

int heap_delete(struct heap *h, int index) {
    if (index < 1 || index > h->nnodes)
        return -1;
    
    h->nodes[index].key = -2147483647; 
    
    int i = index;
    while (i > 1 && h->nodes[i].key < h->nodes[i / 2].key) {
        heap_swap(&h->nodes[i], &h->nodes[i / 2]);
        i /= 2;
    }
    
    heap_extract_min(h);
    
    return 0;
}