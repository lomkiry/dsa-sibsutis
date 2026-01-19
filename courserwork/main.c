#include "header.h"
#include <locale.h>
#include <time.h> 
#include <stdio.h>

#define NUM_ITERATIONS 100000 

void measure_query_time(const char *term1, const char *term2, const char *op_name, 
                        PostingNode *(*operation_func)(PostingNode *, PostingNode *)) {
    
    PostingNode *p1 = get_postings(term1);
    PostingNode *p2 = get_postings(term2);
    
    if (!p1 || !p2) {
        printf("\n--- Запрос: %s ('%s' %s '%s') ---\n", op_name, term1, op_name, term2);
        printf("Ошибка: Терм(ы) '%s' или '%s' не найдены в индексе.\n", term1, term2);
        return;
    }

    clock_t start = clock();
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        PostingNode *result = operation_func(p1, p2);
        free_postings_list(result); 
    }
    
    clock_t end = clock();
    double total_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000; // в мс

    printf("\n--- ТЕСТ БУЛЕВА: %s ('%s' %s '%s') ---\n", op_name, term1, op_name, term2);
    
    PostingNode *final_result = operation_func(p1, p2);
    printf("-> Результат (Doc IDs): ");
    print_postings(final_result);
    free_postings_list(final_result);
    
    printf("-> Общее время (%d итераций): %f мс\n", NUM_ITERATIONS, total_time);
    printf("-> Среднее время на один запрос: %f мкс (микросекунд)\n", 
           (total_time * 1000.0) / NUM_ITERATIONS); // Переводим в микросекунды
}


void run_indexing_test(const char *dir_path, int num_docs) {
    init_index();
    clock_t start = clock();
    index_document_collection(dir_path); 
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\n[РЕЗУЛЬТАТ] Индексация %d документов (%s): %f секунд.\n", 
           num_docs, dir_path, time_taken);
    cleanup_index();
}


int main(void) {
    setlocale(LC_ALL, "C"); 

    printf("--- ТЕСТИРОВАНИЕ МАСШТАБИРУЕМОСТИ ИНДЕКСАЦИИ ---\n");
    run_indexing_test("test_docs10", 10); 
    run_indexing_test("test_docs100", 100); 
    run_indexing_test("test_docs1000", 1000);
    run_indexing_test("test_docs5000", 5000);
    run_indexing_test("test_docs10000", 10000);
    printf("\n--- ИНДЕКСАЦИЯ КОЛЛЕКЦИИ 'test_docs10' ДЛЯ ТЕСТИРОВАНИЯ ЗАПРОСОВ ---\n");
    init_index();
    index_document_collection("test_docs10"); // <--- Используем 10 файлов
    printf("Индексация завершена.\n");

    measure_query_time("algorithm", "data", "AND", intersect_postings); 
    measure_query_time("hash", "encryption", "AND", intersect_postings); 

    measure_query_time("system", "database", "OR", union_postings); 

    measure_query_time("processor", "query", "NOT", difference_postings); 
    
    cleanup_index();
    
    return 0;
}