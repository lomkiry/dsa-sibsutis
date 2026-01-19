#include "header.h"

HashNode *inverted_index[TABLE_SIZE];

void init_index() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        inverted_index[i] = NULL;
    }
}

unsigned int hash_func(const char *str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; 
    }
    return hash % TABLE_SIZE;
}

bool normalize_token(char *token) {
    int write_idx = 0;
    for (int read_idx = 0; token[read_idx] != '\0'; read_idx++) {
        unsigned char c = (unsigned char)token[read_idx];
        
        if (isalnum(c)) { 
            token[write_idx++] = tolower(c); 
        }
    }
    token[write_idx] = '\0';
    return (write_idx > 0);
}

bool insert_posting_sorted(PostingNode **head_ref, int doc_id) {
    PostingNode *current = *head_ref;
    PostingNode *prev = NULL;

    while (current != NULL && current->document_id < doc_id) {
        prev = current;
        current = current->next;
    }

    if (current != NULL && current->document_id == doc_id) {
        return false;
    }

    PostingNode *new_node = (PostingNode *)malloc(sizeof(PostingNode));
    if (!new_node) { perror("Memory allocation error"); exit(1); }
    new_node->document_id = doc_id;

    if (prev == NULL) {
        new_node->next = *head_ref;
        *head_ref = new_node;
    } else {
        new_node->next = current;
        prev->next = new_node;
    }
    return true;
}

void index_term(const char *raw_term, int doc_id) {
    char term_buffer[MAX_TERM_LENGTH];
    strncpy(term_buffer, raw_term, MAX_TERM_LENGTH - 1);
    term_buffer[MAX_TERM_LENGTH - 1] = '\0';

    if (!normalize_token(term_buffer)) return;

    unsigned int index = hash_func(term_buffer);
    HashNode *node = inverted_index[index];

    while (node != NULL) {
        if (strcmp(node->term, term_buffer) == 0) {
            insert_posting_sorted(&(node->postings), doc_id);
            return;
        }
        node = node->next;
    }

    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    if (!new_node) { perror("Memory allocation error"); exit(1); }

    new_node->term = strdup(term_buffer); // Копируем само слово
    new_node->postings = NULL;
    insert_posting_sorted(&(new_node->postings), doc_id);

    new_node->next = inverted_index[index];
    inverted_index[index] = new_node;
}

void read_and_index_file(const char *filepath, int doc_id) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "Ошибка: не удалось открыть файл %s\n", filepath);
        return;
    }

    char buffer[MAX_TERM_LENGTH];
    int buf_idx = 0;
    int ch;

    while ((ch = fgetc(file)) != EOF) {
        if (isspace(ch) || ispunct(ch)) {
            if (buf_idx > 0) {
                buffer[buf_idx] = '\0';
                index_term(buffer, doc_id);
                buf_idx = 0; 
            }
        } else {
            if (buf_idx < MAX_TERM_LENGTH - 1) {
                buffer[buf_idx++] = (char)ch;
            }
        }
    }
    if (buf_idx > 0) {
        buffer[buf_idx] = '\0';
        index_term(buffer, doc_id);
    }

    fclose(file);
}

PostingNode *get_postings(const char *term) {
    char temp_term[MAX_TERM_LENGTH];
    strncpy(temp_term, term, MAX_TERM_LENGTH - 1);
    temp_term[MAX_TERM_LENGTH - 1] = '\0';
    
    if (!normalize_token(temp_term)) return NULL;

    unsigned int index = hash_func(temp_term);
    HashNode *node = inverted_index[index];

    while (node != NULL) {
        if (strcmp(node->term, temp_term) == 0) {
            return node->postings;
        }
        node = node->next;
    }
    return NULL;
}

PostingNode *intersect_postings(PostingNode *p1, PostingNode *p2) {
    PostingNode *result = NULL;
    PostingNode **tail = &result;

    while (p1 && p2) {
        if (p1->document_id == p2->document_id) {
            *tail = (PostingNode *)malloc(sizeof(PostingNode));
            (*tail)->document_id = p1->document_id;
            (*tail)->next = NULL;
            tail = &(*tail)->next;
            
            p1 = p1->next;
            p2 = p2->next;
        } else if (p1->document_id < p2->document_id) {
            p1 = p1->next;
        } else {
            p2 = p2->next;
        }
    }
    return result;
}

PostingNode *union_postings(PostingNode *p1, PostingNode *p2) {
    PostingNode *result = NULL;
    PostingNode **tail = &result;

    while (p1 || p2) {
        int id_to_add;
        
        if (!p1) {
            id_to_add = p2->document_id;
            p2 = p2->next;
        } else if (!p2) {
            id_to_add = p1->document_id;
            p1 = p1->next;
        } else if (p1->document_id < p2->document_id) {
            id_to_add = p1->document_id;
            p1 = p1->next;
        } else if (p2->document_id < p1->document_id) {
            id_to_add = p2->document_id;
            p2 = p2->next;
        } else { 
            id_to_add = p1->document_id;
            p1 = p1->next;
            p2 = p2->next;
        }

        *tail = (PostingNode *)malloc(sizeof(PostingNode));
        (*tail)->document_id = id_to_add;
        (*tail)->next = NULL;
        tail = &(*tail)->next;
    }
    return result;
}

PostingNode *difference_postings(PostingNode *p1, PostingNode *p2) {
    PostingNode *result = NULL;
    PostingNode **tail = &result;

    while (p1) {
        while (p2 && p2->document_id < p1->document_id) {
            p2 = p2->next;
        }

        if (!p2 || p2->document_id > p1->document_id) {
            *tail = (PostingNode *)malloc(sizeof(PostingNode));
            (*tail)->document_id = p1->document_id;
            (*tail)->next = NULL;
            tail = &(*tail)->next;
        }
        
        p1 = p1->next;
    }
    return result;
}

void print_postings(PostingNode *head) {
    if (!head) {
        printf("Empty result\n");
        return;
    }
    printf("[");
    while (head) {
        printf("%d", head->document_id);
        if (head->next) printf(", ");
        head = head->next;
    }
    printf("]\n");
}

void free_postings_list(PostingNode *head) {
    while (head) {
        PostingNode *temp = head;
        head = head->next;
        free(temp);
    }
}

void cleanup_index() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode *node = inverted_index[i];
        while (node) {
            HashNode *temp_node = node;
            node = node->next;

            free_postings_list(temp_node->postings);
            free(temp_node->term);
            free(temp_node);
        }
        inverted_index[i] = NULL;
    }
    printf("Index memory cleaned up.\n");
}

void print_index_content() {
    printf("\n--- ПОЛНОЕ СОДЕРЖИМОЕ ИНВЕРТИРОВАННОГО ИНДЕКСА ---\n");
    bool empty = true;
    int term_count = 0;

    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode *current = inverted_index[i];
        while (current != NULL) {
            empty = false;
            term_count++;
            
            printf("[Ячейка %d] -> Терм: %s, Постинг: ", i, current->term);
            
            printf("[");
            PostingNode *p = current->postings;
            while (p != NULL) {
                printf("%d", p->document_id);
                if (p->next != NULL) {
                    printf(", ");
                }
                p = p->next;
            }
            printf("]\n");
            
            current = current->next;
        }
    }
    
    if (empty) {
        printf("Индекс пуст.\n");
    } else {
        printf("---------------------------------------------------\n");
        printf("Всего проиндексировано уникальных термов: %d\n", term_count);
    }
}

void index_document_collection(const char *directory_path) {
    DIR *d;
    struct dirent *dir;
    char filepath[256];
    int doc_id_counter = 1;

    d = opendir(directory_path);
    if (!d) {
        fprintf(stderr, "Ошибка: Не удалось открыть директорию '%s'\n", directory_path);
        return;
    }

    while ((dir = readdir(d)) != NULL) {
        size_t len = strlen(dir->d_name);
        if (len > 4 && strcmp(dir->d_name + len - 4, ".txt") == 0) {
            
            snprintf(filepath, sizeof(filepath), "%s/%s", directory_path, dir->d_name);
            
            read_and_index_file(filepath, doc_id_counter);
            doc_id_counter++;
        }
    }
    closedir(d);
}