#include <math.h>
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
    int ch;
    printf("Выберите тест(1-табличка, 2-тест добавлений): ");
    while (scanf("%d", &ch) == 2);
    if (ch == 1) {
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

            while (fh->min != NULL) {
                fh = Fibheap_extractmin(fh);
            }
            free(fh);
        }

        printf("Table 1: MinHeap decrease-key:\n");
        for (int s = 0; s < num_sizes; s++) {
            int n = sizes[s];
            struct heap *h = heap_create(n + 10);
            for (int i = 0; i < n; i++) heap_insert(h, data[i], "");


            unsigned long long start = __rdtsc();
            int res = heap_decrease_key(h, n-1, -INFINITY);
            unsigned long long end = __rdtsc();
            /*unsigned long long time = 0;
            for (int j = 0; j < cycle; j++) {
                int idx = h->nnodes - j; 
                if (idx < 1) idx = 1;
                
                int new_val = h->nodes[idx].key - 1;
                
                unsigned long long start = __rdtsc();
                heap_decrease_key(h, idx, new_val);
                unsigned long long end = __rdtsc();
                time += (end - start);
            } */

            printf("%llu\n", end - start);
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
            
            while (fh->min != NULL) {
                fh = Fibheap_extractmin(fh);
            }
            free(fh);
        }

        printf("Table 2: MinHeap extract-all:\n");
        for (int s = 0; s < num_sizes; s++) {
            int n = sizes[s];
            struct heap *h = heap_create(n + 10);
            for (int i = 0; i < n; i++) heap_insert(h, data[i], "");
            unsigned long long alltime = 0;
            while (h->nnodes > 0) {
                unsigned long long start = __rdtsc();
                heap_extract_min(h);
                unsigned long long end = __rdtsc();
                alltime += end-start;
            }
            
            
            printf("%llu\n", alltime / n);
            heap_free(h);
        }

        printf("Table 2: FibHeap extract-all:\n");
        for (int s = 0; s < num_sizes; s++) {
            int n = sizes[s];
            struct Fibheap *fh = build_heap();
            for (int i = 0; i < n; i++) fh = Fibheap_insert(fh, data[i], "");
            unsigned long long alltime = 0;
            while (fh->min != NULL) {
                unsigned long long start = __rdtsc();
                fh = Fibheap_extractmin(fh);
                unsigned long long end = __rdtsc();
                alltime += end-start;
            }

            printf("%llu\n", alltime / n);
            free(fh); 
        }

        printf("Table 2: MinHeap decrease-all:\n");
        for (int s = 0; s < num_sizes; s++) {
            int n = sizes[s];
            struct heap *h = heap_create(n + 10);
            for (int i = 0; i < n; i++) heap_insert(h, data[i], "");
            unsigned long long alltime = 0;
            for (int i = h->nnodes; i >= 1; i--) {
                unsigned long long start = __rdtsc();
                int result = heap_decrease_key(h, i, h->nodes[1].key);
                unsigned long long end = __rdtsc();
                heap_extract_min(h);
                alltime += end-start;
            }

            printf("%llu\n", alltime / n);
            heap_free(h);
        }

        printf("Table 2: FibHeap decrease-all:\n");
        for (int s = 0; s < num_sizes; s++) {
            int n = sizes[s];
            struct Fibheap *fh = build_heap();
            struct Node **all_nodes = (struct Node**)malloc(sizeof(struct Node*) * n);

            for (int i = 0; i < n; i++) {
                fh = Fibheap_insert(fh, data[i], "");
            }

            fh = Fibheap_extractmin(fh);

            struct Node *curr = fh->min;
            int idx = 0;
            if (curr != NULL) {
                do {
                    all_nodes[idx++] = curr;
                    curr = curr->right;
                } while (curr != fh->min && idx < n);
            }
            long long alltime = 0;
            for (int i = 0; i < idx; i++) {
              unsigned long long start = __rdtsc();
                fh = Fibheap_decrease_key(fh, all_nodes[i], all_nodes[0]->key + 50);
                unsigned long long end = __rdtsc();
                alltime += end - start;

            }
            printf("%llu\n", alltime);

            free(all_nodes);
            
            while (fh->min != NULL) {
                fh = Fibheap_extractmin(fh);
            }
            free(fh);
        }

        free(data);
    } else if( ch == 2) {
        // Тест 1: Минимальный узел со степенью 0
        struct Fibheap *fh_0 = build_heap();
        fh_0 = Fibheap_insert(fh_0, 10, "");
        fh_0 = Fibheap_insert(fh_0, 20, ""); 
        fh_0 = Fibheap_insert(fh_0, 30, "");

        printf("Тест 0 детей: Размер до = %d, Мин = %d\n", fh_0->size, Fibheap_min(fh_0)->key);
        fh_0 = Fibheap_extractmin(fh_0);
        printf("Тест 0 детей: Размер после = %d, Мин = %d\n\n", fh_0->size, Fibheap_min(fh_0)->key);
        
        // Тест 1: Минимальный узел со степенью 1
        struct Fibheap *fh_1 = build_heap();
        fh_1 = Fibheap_insert(fh_1, 50, "A"); 
        fh_1 = Fibheap_insert(fh_1, 60, "B");
        fh_1 = Fibheap_insert(fh_1, 70, "C");
        fh_1 = Fibheap_insert(fh_1, 80, "D");
        fh_1 = Fibheap_insert(fh_1, 10, "MIN");

        fh_1 = Fibheap_extractmin(fh_1); 
        fh_1 = Fibheap_extractmin(fh_1); 
        
        printf("Тест 1 ребенок: Размер до = %d, Мин = %d (Ожидание: 1 ребенок)\n", fh_1->size, Fibheap_min(fh_1)->key);
        fh_1 = Fibheap_extractmin(fh_1);
        printf("Тест 1 ребенок: Размер после = %d, Мин = %d\n\n", fh_1->size, Fibheap_min(fh_1)->key);

        // Тест 3: Минимальный узел со степенью 2
        struct Fibheap *fh_2 = build_heap();

        struct Node *min_node = (struct Node*)malloc(sizeof(struct Node));
        struct Node *child1 = (struct Node*)malloc(sizeof(struct Node));
        struct Node *child2 = (struct Node*)malloc(sizeof(struct Node));
        struct Node *other_root = (struct Node*)malloc(sizeof(struct Node)); 

        min_node->key = 10; min_node->value = strdup("MIN"); min_node->degree = 2; min_node->mark = false;
        child1->key = 20; child1->value = strdup("C1"); child1->degree = 0; child1->mark = false;
        child2->key = 30; child2->value = strdup("C2"); child2->degree = 0; child2->mark = false;
        other_root->key = 100; other_root->value = strdup("Other"); other_root->degree = 0; other_root->mark = false;

        min_node->parent = NULL; min_node->child = NULL;
        other_root->parent = NULL; other_root->child = NULL;

        fh_2->min = min_node;
        min_node->left = other_root;
        min_node->right = other_root;
        other_root->left = min_node;
        other_root->right = min_node;

        min_node->child = child1;
        child1->parent = min_node;
        child2->parent = min_node;

        child1->left = child2; child1->right = child2;
        child2->left = child1; child2->right = child1;

        child1->child = NULL; child2->child = NULL;

        fh_2->size = 3;

        printf("Тест 2 ребенка: Размер до = %d, Мин = %d\n", fh_2->size, Fibheap_min(fh_2)->key);
        fh_2 = Fibheap_extractmin(fh_2);
        printf("Тест 2 ребенка: Размер после = %d, Мин = %d\n\n", fh_2->size, Fibheap_min(fh_2)->key);

        while (fh_2->min != NULL) fh_2 = Fibheap_extractmin(fh_2);
        free(fh_2);
        // Тест 4: Минимальный узел со степенью 3
        struct Fibheap *fh_3 = build_heap();

        struct Node *min_node_3 = (struct Node*)malloc(sizeof(struct Node));
        struct Node *c1 = (struct Node*)malloc(sizeof(struct Node));
        struct Node *c2 = (struct Node*)malloc(sizeof(struct Node));
        struct Node *c3 = (struct Node*)malloc(sizeof(struct Node));
        struct Node *other_root_3 = (struct Node*)malloc(sizeof(struct Node)); 

        min_node_3->key = 10; min_node_3->value = strdup("MIN3"); min_node_3->degree = 3; min_node_3->mark = false;
        c1->key = 20; c1->value = strdup("C1"); c1->degree = 0; c1->mark = false;
        c2->key = 30; c2->value = strdup("C2"); c2->degree = 0; c2->mark = false;
        c3->key = 40; c3->value = strdup("C3"); c3->degree = 0; c3->mark = false;
        other_root_3->key = 50; other_root_3->value = strdup("Other3"); other_root_3->degree = 0; other_root_3->mark = false;

        min_node_3->parent = NULL; min_node_3->child = NULL;
        other_root_3->parent = NULL; other_root_3->child = NULL;

        fh_3->min = min_node_3;
        min_node_3->left = other_root_3;
        min_node_3->right = other_root_3;
        other_root_3->left = min_node_3;
        other_root_3->right = min_node_3;

        min_node_3->child = c1;
        c1->parent = min_node_3;
        c2->parent = min_node_3;
        c3->parent = min_node_3;

        c1->left = c3; c1->right = c2; c1->child = NULL;
        c2->left = c1; c2->right = c3; c2->child = NULL;
        c3->left = c2; c3->right = c1; c3->child = NULL;

        fh_3->size = 4;

        printf("Тест 3 ребенка: Размер до = %d, Мин = %d\n", fh_3->size, Fibheap_min(fh_3)->key);
        fh_3 = Fibheap_extractmin(fh_3);
        printf("Тест 3 ребенка: Размер после = %d, Мин = %d\n\n", fh_3->size, Fibheap_min(fh_3)->key);

        while (fh_3->min != NULL) fh_3 = Fibheap_extractmin(fh_3);
        free(fh_3);
                
        struct Fibheap *fh_solo = build_heap();
        fh_solo = Fibheap_insert(fh_solo, 99, "SOLO");
        printf("Тест Один узел: Размер до = %d, Мин = %d\n", fh_solo->size, Fibheap_min(fh_solo)->key);
        fh_solo = Fibheap_extractmin(fh_solo);
        if (fh_solo->min == NULL && fh_solo->size == 0) {
            printf("Тест Один узел: (Куча пуста).\n\n");
        } else {
            printf("Тест Один узел: Ошибка.\n\n");
        }
    }
    return 0;
}