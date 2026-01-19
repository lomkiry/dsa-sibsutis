#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h> 
#include <dirent.h>
#include <locale.h>

#define TABLE_SIZE 100003 
#define MAX_TERM_LENGTH 256 

typedef struct PostingNode {
    int document_id;
    struct PostingNode *next;
} PostingNode;

typedef struct HashNode {
    char *term;              
    PostingNode *postings; 
    struct HashNode *next;  
} HashNode;

extern HashNode *inverted_index[TABLE_SIZE];

void init_index();                  
void cleanup_index();               
unsigned int hash_func(const char *str); 

void index_term(const char *raw_term, int doc_id);
void read_and_index_file(const char *filepath, int doc_id); 

PostingNode *get_postings(const char *term); 
PostingNode *intersect_postings(PostingNode *p1, PostingNode *p2); 
PostingNode *union_postings(PostingNode *p1, PostingNode *p2);     
PostingNode *difference_postings(PostingNode *p1, PostingNode *p2);

void print_postings(PostingNode *head); 
void free_postings_list(PostingNode *head); 
void print_index_content(); 
void index_document_collection(const char *directory_path);

#endif