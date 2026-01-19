#include "rbtree.h"
#include "bstree.h"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <x86intrin.h>
#include <string.h>

int getrand(int min, int max) {
    return (double)rand() / (RAND_MAX + 1.0) * (max - min) + min;
}

unsigned long long search_cycles_bs(struct bstree *tree, int key) {
    unsigned long long start = __rdtsc();
    bstree_lookup(tree, key);
    unsigned long long end = __rdtsc();
    return end - start;
}

unsigned long long search_cycles_rb(struct rbtree *tree, int key) {
    unsigned long long start = __rdtsc();
    rbtree_lookup(tree, key);
    unsigned long long end = __rdtsc();
    return end - start;
}

unsigned long long max_cycles_bs(struct bstree *tree) {
    unsigned long long start = __rdtsc();
    bstree_max(tree);
    unsigned long long end = __rdtsc();
    return end - start;
}

unsigned long long max_cycles_rb(struct rbtree *tree) {
    unsigned long long start = __rdtsc();
    rbtree_max(tree);
    unsigned long long end = __rdtsc();
    return end - start;
}

// Функция для интерактивного построения красно-черного дерева
void interactive_rbtree() {
    struct rbtree *root = NULL;
    char command[20];
    int key;
    int value_counter = 1;
    
    printf("\n=== ИНТЕРАКТИВНОЕ ПОСТРОЕНИЕ КРАСНО-ЧЕРНОГО ДЕРЕВА ===\n");
    printf("Команды:\n");
    printf("  add <key>    - добавить элемент (целое число)\n");
    printf("  del <key>    - удалить элемент\n");
    printf("  show         - показать дерево с цветами узлов\n");
    printf("  exit         - выход\n\n");
    
    while (1) {
        printf("rbtree> ");
        if (scanf("%s", command) != 1) {
            break;
        }
        
        if (strcmp(command, "exit") == 0) {
            break;
        }
        
        if (strcmp(command, "show") == 0) {
            printf("\nКрасно-черное дерево:\n");
            printf("(R - красный узел, B - черный узел)\n");
            printf("-----------------------------------\n");
            if (root == NULL) {
                printf("Дерево пустое\n");
            } else {
                rbtree_print_dfs(root, 0);
            }
            printf("-----------------------------------\n\n");
            continue;
        }
        
        if (strcmp(command, "add") == 0 || strcmp(command, "del") == 0) {
            if (scanf("%d", &key) != 1) {
                printf("Ошибка: требуется целое число\n");
                continue;
            }
            
            if (strcmp(command, "add") == 0) {
                char value[50];
                sprintf(value, "val%d", value_counter);
                root = rbtree_add(root, key, value);
                printf("Добавлен ключ %d: %s\n", key, value);
                value_counter++;
            }
            else if (strcmp(command, "del") == 0) {
                root = rbtree_delete(root, key);
                printf("Удален ключ %d\n", key);
            }
        } else {
            printf("Неизвестная команда. Используйте: add, del, show, exit\n");
        }
    }
    
    rbtree_free(root);
    printf("Выход\n");
}

int main(void) {
    srand(time(NULL));
    printf("1. таблицы\n 2. строить\n");
    int res; 
    scanf("%d", &res);
    if (res == 1) {
        int average_case[200000];
    int worst_case[200000];

    for (int i = 0; i < 200000; i++) {
        average_case[i] = getrand(10000, 1000000);
    }

    for (int i = 0; i < 200000; i++) {
        worst_case[i] = i;
    }
    int cycle = 15;
    
    // Ваши тесты
    printf("BSTree lookup average case:\n");
    for (int i = 0; i < 10; i++) {
        struct bstree *tree = NULL;
        int j = 0;
        for (j = 0; j < (i+1) * 20000; j++) {
            tree = bstree_add(tree, average_case[j], "");
        }
        unsigned long long time = 0;
        for (int i = 0; i < cycle; i++) {
            time += search_cycles_bs(tree, average_case[j-1]);
        }
        printf("%llu\n", time/cycle);
        bstree_free(tree);
    }
    printf("BSTree lookup worst case:\n");
    for (int i = 0; i < 10; i++) {
        struct bstree *tree = NULL;
        int j = 0;
        for (j = 0; j < (i+1) * 20000; j++) {
            tree = bstree_add(tree, worst_case[j], "");
        }
        unsigned long long time = 0;
        for (int i = 0; i < cycle; i++) {
            time += search_cycles_bs(tree, worst_case[j-1]);
        }
        printf("%llu\n", time/cycle);
        bstree_free(tree);
    }
    printf("RBTree lookup average case:\n");
    for (int i = 0; i < 10; i++) {
        struct rbtree *tree = NULL;
        int j = 0;
        for (j = 0; j < (i+1) * 20000; j++) {
            tree = rbtree_add(tree, average_case[j], "");
        }
        unsigned long long time = 0;
        for (int i = 0; i < cycle; i++) {
            time += search_cycles_rb(tree, average_case[j-1]);
        }
        printf("%llu\n", time/cycle);
        rbtree_free(tree);
    }
    printf("RBTree lookup worst case:\n");
    for (int i = 0; i < 10; i++) {
        struct rbtree *tree = NULL;
        int j = 0;
        for (j = 0; j < (i+1) * 20000; j++) {
            tree = rbtree_add(tree, worst_case[j], "");
        }
        unsigned long long time = 0;
        for (int i = 0; i < cycle; i++) {
            time += search_cycles_rb(tree, worst_case[j-1]);
        }
        printf("%llu\n", time/cycle);
        rbtree_free(tree);
    }
    printf("BSTree max average case:\n");
    for (int i = 0; i < 10; i++) {
        struct bstree *tree = NULL;
        int j = 0;
        for (j = 0; j < (i+1) * 20000; j++) {
            tree = bstree_add(tree, average_case[j], "");
        }
        unsigned long long time = 0;
        for (int i = 0; i < cycle; i++) {
            time += max_cycles_bs(tree);
        }
        printf("%llu\n", time/cycle);
        bstree_free(tree);
    }

    printf("RBTree max average case:\n");
    for (int i = 0; i < 10; i++) {
        struct rbtree *tree = NULL;
        int j = 0;
        for (j = 0; j < (i+1) * 20000; j++) {
            tree = rbtree_add(tree, average_case[j], "");
        }
        unsigned long long time = 0;
        for (int i = 0; i < cycle; i++) {
            time += max_cycles_rb(tree);
        }
        printf("%llu\n", time/cycle);
        rbtree_free(tree);
    }
    printf("BSTree max worst case:\n");
    for (int i = 0; i < 10; i++) {
        struct bstree *tree = NULL;
        int j = 0;
        for (j = 0; j < (i+1) * 20000; j++) {
            tree = bstree_add(tree, worst_case[j], "");
        }
        unsigned long long time = 0;
        for (int i = 0; i < cycle; i++) {
            time += max_cycles_bs(tree);
        }
        printf("%llu\n", time/cycle);
        bstree_free(tree);
    }

    printf("RBTree max worst case:\n");
    for (int i = 0; i < 10; i++) {
        struct rbtree *tree = NULL;
        int j = 0;
        for (j = 0; j < (i+1) * 20000; j++) {
            tree = rbtree_add(tree, worst_case[j], "");
        }
        unsigned long long time = 0;
        for (int i = 0; i < cycle; i++) {
            time += max_cycles_rb(tree);
        }
        printf("%llu\n", time/cycle);
        rbtree_free(tree);
    }
    } else if (res == 2) {
        interactive_rbtree();
    
    }

    
    
    return 0;
}