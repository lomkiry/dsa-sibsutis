#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <x86intrin.h>
#include <string.h> 

#include "minheap.h"
#include "fibheap.h"

int getrand(int min, int max) {
    return (double)rand() / (RAND_MAX + 1.0) * (max - min) + min;
}

int main(void) {
    srand(time(NULL));

    int data_size = 200000;
    int *data = (int*)malloc(sizeof(int) * data_size);
    for (int i = 0; i < data_size; i++) {
        data[i] = getrand(10000, 200000);
    }

    int sizes[] = {50000, 100000, 150000, 200000};
    int num_sizes = 4;
    int cycle = 1000;

    printf("Table 1: MinHeap extract-min:\n");
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        struct heap *h = heap_create(n + cycle + 10);
        for (int i = 0; i < n; i++) heap_insert(h, data[i], "");

        unsigned long long time = 0;
        for (int j = 0; j < cycle; j++) {
            unsigned long long start = __rdtsc();
            heap_extract_min(h);
            unsigned long long end = __rdtsc();
            time += (end - start);
            
            heap_insert(h, data[0], ""); 
        }
        printf("%llu\n", time / cycle);
        heap_free(h);
    }

    printf("Table 1: FibHeap extract-min:\n");
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        struct Fibheap *fh = build_heap();
        for (int i = 0; i < n; i++) fh = Fibheap_insert(fh, data[i], "");

        unsigned long long time = 0;
        for (int j = 0; j < cycle; j++) {
            unsigned long long start = __rdtsc();
            fh = Fibheap_extractmin(fh);
            unsigned long long end = __rdtsc();
            time += (end - start);
            
            fh = Fibheap_insert(fh, data[0], "");
        }
        printf("%llu\n", time / cycle);
    }

    printf("Table 1: MinHeap decrease-key:\n");
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        struct heap *h = heap_create(n + 10);
        for (int i = 0; i < n; i++) heap_insert(h, data[i], "");

        unsigned long long time = 0;
        for (int j = 0; j < cycle; j++) {
            int idx = h->nnodes - j; 
            if (idx < 1) idx = 1;
            
            int new_val = h->nodes[idx].key - 1;
            
            unsigned long long start = __rdtsc();
            heap_decrease_key(h, idx, new_val);
            unsigned long long end = __rdtsc();
            time += (end - start);
        }
        printf("%llu\n", time / cycle);
        heap_free(h);
    }

    printf("Table 1: FibHeap decrease-key:\n");
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        struct Fibheap *fh = build_heap();
        for (int i = 0; i < n; i++) fh = Fibheap_insert(fh, data[i], "");

        struct Node **nodes_to_test = (struct Node**)malloc(sizeof(struct Node*) * n);
        struct Node *curr = fh->min;
        int count = 0;
        if (curr != NULL) {
            do {
                nodes_to_test[count++] = curr;
                curr = curr->right;
            } while (curr != fh->min && count < n);
        }

        unsigned long long time = 0;
        for (int j = 0; j < cycle; j++) {
            struct Node *target = nodes_to_test[j % count];
            
            int new_val = target->key - 1;

            unsigned long long start = __rdtsc();
            fh = Fibheap_decrease_key(fh, target, new_val);
            unsigned long long end = __rdtsc();
            time += (end - start);
        }
        printf("%llu\n", time / cycle);
        free(nodes_to_test);
    }

    printf("Table 2: MinHeap extract-all:\n");
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        struct heap *h = heap_create(n + 10);
        for (int i = 0; i < n; i++) heap_insert(h, data[i], "");

        unsigned long long start = __rdtsc();
        while (h->nnodes > 0) {
            heap_extract_min(h);
        }
        unsigned long long end = __rdtsc();
        
        printf("%llu\n", end - start);
        heap_free(h);
    }

    printf("Table 2: FibHeap extract-all:\n");
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        struct Fibheap *fh = build_heap();
        for (int i = 0; i < n; i++) fh = Fibheap_insert(fh, data[i], "");

        unsigned long long start = __rdtsc();
        while (fh->min != NULL) {
            fh = Fibheap_extractmin(fh);
        }
        unsigned long long end = __rdtsc();

        printf("%llu\n", end - start);
    }

    printf("Table 2: MinHeap decrease-all:\n");
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        struct heap *h = heap_create(n + 10);
        for (int i = 0; i < n; i++) heap_insert(h, data[i], "");

        unsigned long long start = __rdtsc();
        for (int i = h->nnodes; i >= 1; i--) {
            heap_decrease_key(h, i, h->nodes[i].key - 1);
        }
        unsigned long long end = __rdtsc();

        printf("%llu\n", end - start);
        heap_free(h);
    }

    printf("Table 2: FibHeap decrease-all:\n");
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        struct Fibheap *fh = build_heap();        
        for (int i = 0; i < n; i++) fh = Fibheap_insert(fh, data[i], "");

        struct Node **all_nodes = (struct Node**)malloc(sizeof(struct Node*) * n);
        struct Node *curr = fh->min;
        int idx = 0;
        if (curr != NULL) {
            do {
                all_nodes[idx++] = curr;
                curr = curr->right;
            } while (curr != fh->min && idx < n);
        }

        unsigned long long start = __rdtsc();
        for (int i = 0; i < idx; i++) {
            fh = Fibheap_decrease_key(fh, all_nodes[i], all_nodes[i]->key - 1);
        }
        unsigned long long end = __rdtsc();

        printf("%llu\n", end - start);
        free(all_nodes);
    }

    free(data);
    return 0;
}