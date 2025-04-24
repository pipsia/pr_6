#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    const int* array;
    size_t size;
    int target;
    int result_index;
    pthread_mutex_t mutex;
} SearchData;

void* search_thread(void* arg) {
    SearchData* data = (SearchData*)arg;
    
    for (size_t i = 0; i < data->size; ++i) {
        if (data->array[i] == data->target) {
            pthread_mutex_lock(&data->mutex);
            if (data->result_index == -1 || i < (size_t)data->result_index) {
                data->result_index = i;
            }
            pthread_mutex_unlock(&data->mutex);
            break;
        }
    }
    
    return NULL;
}

int parallel_search(const int* array, size_t size, int target, int num_threads) {
    SearchData data = {array, size, target, -1, PTHREAD_MUTEX_INITIALIZER};
    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    
    for (int i = 0; i < num_threads; ++i) {
        pthread_create(&threads[i], NULL, search_thread, &data);
    }
    
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
    
    free(threads);
    pthread_mutex_destroy(&data.mutex);
    return data.result_index;
}

int main() {
    int array[] = {1, 5, 3, 7, 5, 9, 2, 5, 8};
    size_t size = sizeof(array) / sizeof(array[0]);
    int target = 5;
    int num_threads = 4;
    
    int index = parallel_search(array, size, target, num_threads);
    
    if (index != -1) {
        printf("Found at index: %d\n", index);
    } else {
        printf("Not found\n");
    }
    
    return 0;
}
