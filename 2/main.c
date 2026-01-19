#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include "rtree.h"
#include <x86intrin.h>

void generate_words_file() {
    const char* filename = "words.txt";
    int n = 100000;
    int min_length = 6;
    int max_length = 30;
    
    FILE *file = fopen(filename, "w");
    if (file == NULL) return;
    
    for (int i = 0; i < n; i++) {
        int word_len = rand() % (max_length - min_length + 1) + min_length;
        for (int j = 0; j < word_len; j++) {
            char c = 'a' + (rand() % 26);
            fputc(c, file);
        }
        fputc('\n', file);
    }
    fclose(file);
}

char** read_words_from_file(const char* filename, int* word_count) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) return NULL;
    
    int count = 0;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        count++;
    }
    rewind(file);
    
    char** words = malloc(count * sizeof(char*));
    if (words == NULL) {
        fclose(file);
        return NULL;
    }
    
    int index = 0;
    while (fgets(buffer, sizeof(buffer), file) && index < count) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        if (len > 1) {
            words[index] = malloc(strlen(buffer) + 1);
            strcpy(words[index], buffer);
            index++;
        }
    }
    fclose(file);
    *word_count = index;
    return words;
}

void rtree_free(struct rtree *root) {
    if (root == NULL) return;
    rtree_free(root->child);
    rtree_free(root->sibling);
    free(root);
}

void experimental_research() {
    printf("|  # | Количество элементов | Время rtree_lookup, такты |\n");  
    printf("|----|----------------------|---------------------------|\n");
    
    FILE* test_file = fopen("words.txt", "r");
    if (test_file == NULL) { 
        generate_words_file();
    } else {
        fclose(test_file);
    }
    
    int word_count;
    char** all_keys = read_words_from_file("words.txt", &word_count);
    if (all_keys == NULL || word_count < 100000) {
        return;
    }
    
    struct rtree *root = NULL;
    
    for (int experiment = 1; experiment <= 10; experiment++) {
        int num_elements = experiment * 10000;
        
        for (int i = 0; i < num_elements; i++) {
            root = rtree_insert(root, all_keys[i], i + 1);
        }
        
        unsigned long long time = 0;
        int cycle = 1000;
        
        for (int j = 0; j < cycle; j++) {
            int random_index = rand() % num_elements;
            
            unsigned long long start = __rdtsc();
            struct rtree *result = rtree_lookup(root, all_keys[random_index]);
            unsigned long long end = __rdtsc();
            time += (end - start);
        }
        
        printf("| %2d | %19d | %24llu |\n", experiment, num_elements, time / cycle);

        for (int i = 0; i < num_elements; i++) {
            root = rtree_delete(root, all_keys[i]);
        }
    }
    
    rtree_free(root);
    
    for (int i = 0; i < word_count; i++) {
        free(all_keys[i]);
    }
    free(all_keys);
}

void interactive_tree_building() {
    struct rtree *root = NULL;
    char input[256];
    char word[256];
    int value = 1;
    
    printf("Команды: add, del, search, show, clear, exit\n");
    
    while (1) {
        printf("> ");
        if (scanf("%255s", input) != 1) {
            break;
        }
        
        if (strcmp(input, "exit") == 0) {
            break;
        }
        
        if (strcmp(input, "show") == 0) {
            rtree_print(root, 0);
            continue;
        }
        
        if (strcmp(input, "clear") == 0) {
            rtree_free(root);
            root = NULL;
            value = 1;
            continue;
        }
        
        if (strcmp(input, "add") == 0 || strcmp(input, "del") == 0 || strcmp(input, "search") == 0) {
            if (scanf("%255s", word) != 1) {
                continue;
            }
            
            if (strcmp(input, "add") == 0) {
                root = rtree_insert(root, word, value);
                printf("Добавлено [%d]\n", value);
                value++;
            }
            else if (strcmp(input, "del") == 0) {
                struct rtree *found = rtree_lookup(root, word);
                if (found != NULL) {
                    root = rtree_delete(root, word);
                    printf("Удалено\n");
                } else {
                    printf("Не найдено\n");
                }
            }
            else if (strcmp(input, "search") == 0) {
                unsigned long long start = __rdtsc();
                struct rtree *found = rtree_lookup(root, word);
                unsigned long long end = __rdtsc();
                
                if (found != NULL) {
                    printf("Найдено [%u] за %llu тактов\n", found->value, end - start);
                } else {
                    printf("Не найдено (поиск занял %llu тактов)\n", end - start);
                }
            }
        }
    }
    
    rtree_free(root);
}

int main() {
    srand(time(NULL));
    
    int choice;
    printf("1 - таблица\n");
    printf("2 - построить дерево\n");
    printf("Выбор: ");
    scanf("%d", &choice);
    
    switch(choice) {
        case 1:
            experimental_research();
            break;
        case 2:
            interactive_tree_building();
            break;
    }
    
    return 0;
}