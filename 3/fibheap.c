#include "fibheap.h"
#include <math.h>
#include <string.h>
#include <stdlib.h> 


struct Fibheap *build_heap(void) {
    struct Fibheap *heap = (struct Fibheap*)malloc(sizeof(struct Fibheap));
    if (heap) {
        heap->size = 0;
        heap->min = NULL;
    }
    return heap;
}

void Fibheap_add_node_to_root_list(struct Node *new_node, struct Node *node) {
    if (node == NULL) return; 
    
    
    new_node->left = node;
    new_node->right = node->right;
    node->right->left = new_node;
    node->right = new_node;
}

struct Fibheap *Fibheap_insert(struct Fibheap *heap, int key, char *value) {
    struct Node *new_node = (struct Node*)malloc(sizeof(struct Node));
    if (!new_node) return heap;
    
    new_node->key = key;
    if (value) {
        new_node->value = strdup(value);
    } else {
        new_node->value = NULL;
    }
    
    new_node->parent = NULL;
    new_node->child = NULL;
    new_node->degree = 0;
    new_node->mark = false;
    new_node->left = new_node;
    new_node->right = new_node;
    
    if (heap->min == NULL) {
        heap->min = new_node;
    } else {
        Fibheap_add_node_to_root_list(new_node, heap->min);
        if (key < heap->min->key) {
            heap->min = new_node;
        }
    }
    
    heap->size++;
    return heap;
}

struct Node *Fibheap_min(struct Fibheap *heap) {
    return heap->min;
}

struct Node *FibHeapLinkList(struct Node *h1, struct Node *h2) {
    if (!h1) return h2;
    if (!h2) return h1;

    struct Node *h1_right = h1->right;
    struct Node *h2_left  = h2->left;

    h1->right = h2;
    h2->left  = h1;

    h2_left->right = h1_right;
    h1_right->left = h2_left;

    return h1;
}

struct Fibheap *Fibheap_union(struct Fibheap *heap1, struct Fibheap *heap2) {
    struct Fibheap *heap = build_heap();
    if (!heap1->min) {
        heap->min = heap2->min;
        heap->size = heap2->size;
    } else if (!heap2->min) {
        heap->min = heap1->min;
        heap->size = heap1->size;
    } else {
        heap->min = FibHeapLinkList(heap1->min, heap2->min);
        if (heap2->min->key < heap1->min->key) {
            heap->min = heap2->min;
        }
        heap->size = heap1->size + heap2->size;
    }
    
    free(heap1);
    free(heap2);
    return heap;
}

void Fibheap_remove_node_from_root_list(struct Node *node) {
    node->left->right = node->right;
    node->right->left = node->left;
}

void Fibheap_link(struct Fibheap *heap, struct Node *y, struct Node *x) {
    Fibheap_remove_node_from_root_list(y);
    
    y->parent = x;
    
    if (x->child == NULL) {
        x->child = y;
        y->left = y;
        y->right = y;
    } else {
        Fibheap_add_node_to_root_list(y, x->child);
    }
    
    x->degree++;
    y->mark = false;
}

int D(int size){
    return floor(log(size));
}

void Fibheap_consolidate(struct Fibheap *heap) {
    const int MAX_DEGREE = D(heap->size);
    struct Node *A[64];
    
    for (int i = 0; i < MAX_DEGREE; i++) {
        A[i] = NULL;
    }
    
    struct Node *start = heap->min;
    if (start == NULL) return;

    int num_roots = 0;
    struct Node *curr = start;
    do {
        num_roots++;
        curr = curr->right;
    } while (curr != start);

    struct Node **root_nodes = (struct Node**)malloc(sizeof(struct Node*) * num_roots);
    curr = start;
    for (int i = 0; i < num_roots; i++) {
        root_nodes[i] = curr;
        curr = curr->right;
    }

    for (int i = 0; i < num_roots; i++) {
        struct Node *x = root_nodes[i];
        int d = x->degree;
        
        while (d < MAX_DEGREE && A[d] != NULL) {
            struct Node *y = A[d];
            if (x->key > y->key) {
                struct Node *temp = x;
                x = y;
                y = temp;
            }
            Fibheap_link(heap, y, x);
            A[d] = NULL;
            d++;
        }
        if (d < MAX_DEGREE) {
            A[d] = x;
        }
    }
    free(root_nodes);

    heap->min = NULL;
    for (int i = 0; i < MAX_DEGREE; i++) {
        if (A[i] != NULL) {
            A[i]->left = A[i];
            A[i]->right = A[i];
            
            if (heap->min == NULL) {
                heap->min = A[i];
            } else {
                Fibheap_add_node_to_root_list(A[i], heap->min);
                if (A[i]->key < heap->min->key) {
                    heap->min = A[i];
                }
            }
        }
    }
}

struct Fibheap *Fibheap_extractmin(struct Fibheap *heap) {
    struct Node *z = heap->min;
    if (z != NULL) {
        if (z->child != NULL) {
            struct Node *child = z->child;
            struct Node *ptr = child;
            
            do {
                ptr->parent = NULL;
                ptr->mark = false; 
                ptr = ptr->right;
            } while (ptr != child);
            
            struct Node *min_left = z->left;
            struct Node *child_left = child->left;
            
            z->left->right = child;
            child->left = z->left;
            z->left = child_left;
            child_left->right = z;
        }
        
        if (z == z->right && z->child == NULL) {
            heap->min = NULL;
        } else {
            z->left->right = z->right;
            z->right->left = z->left;
            
            heap->min = z->right; 
            
            Fibheap_consolidate(heap);
        }
        
        heap->size--;
        
        if (z->value) free(z->value);
        free(z);
    }
    return heap;
}


void Fibheap_cut(struct Fibheap *heap, struct Node *x, struct Node *y) {
    if (x->right == x) {
        y->child = NULL;
    } else {
        x->left->right = x->right;
        x->right->left = x->left;
        if (y->child == x) {
            y->child = x->right;
        }
    }
    
    y->degree--;
    
    Fibheap_add_node_to_root_list(x, heap->min);
    x->parent = NULL;
    x->mark = false;
}

void Fibheap_cascading_cut(struct Fibheap *heap, struct Node *y) {
    struct Node *z = y->parent;
    if (z != NULL) {
        if (y->mark == false) {
            y->mark = true;
        } else {
            Fibheap_cut(heap, y, z);
            Fibheap_cascading_cut(heap, z);
        }
    }
}

struct Fibheap *Fibheap_decrease_key(struct Fibheap *heap, struct Node *x, int newkey) {
    if (newkey > x->key) return heap;
    
    x->key = newkey;
    struct Node *y = x->parent;
    
    if (y != NULL && x->key < y->key) {
        Fibheap_cut(heap, x, y);
        Fibheap_cascading_cut(heap, y);
    }
    
    if (x->key < heap->min->key) {
        heap->min = x;
    }
    return heap;
}

struct Fibheap *Fibheap_delete(struct Fibheap *heap, int key) {
    struct Node *node_to_delete = NULL;
    struct Node *start = heap->min;
    
    if (start != NULL) {
        struct Node *curr = start;
        do {
            if (curr->key == key) {
                node_to_delete = curr;
                break;
            }
            curr = curr->right;
        } while (curr != start);
    }
    
    if (node_to_delete) {
        Fibheap_decrease_key(heap, node_to_delete, -2147483647);
        Fibheap_extractmin(heap);
    }
    return heap;
}