#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    const int* array;
    size_t start;
    size_t end;
    int target;
    int* result_indices;
    size_t* result_count;
    pthread_mutex_t* mutex;
} ThreadData;

void* search_all_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    for (size_t i = data->start; i < data->end; ++i) {
        if (data->array[i] == data->target) {
            pthread_mutex_lock(data->mutex);
            data->result_indices[(*data->result_count)++] = i;
            pthread_mutex_unlock(data->mutex);
        }
    }
    return NULL;
}

void sort_indices(int* indices, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        for (size_t j = i + 1; j < count; ++j) {
            if (indices[i] > indices[j]) {
                int tmp = indices[i];
                indices[i] = indices[j];
                indices[j] = tmp;
            }
        }
    }
}

int* parallel_search_all(const int* array, size_t size, int target, int num_threads, size_t* result_count) {
    int* result_indices = malloc(size * sizeof(int));
    size_t count = 0;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    
    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    ThreadData* thread_data = malloc(num_threads * sizeof(ThreadData));
    
    size_t chunk_size = size / num_threads;
    for (int i = 0; i < num_threads; ++i) {
        thread_data[i] = (ThreadData){
            .array = array,
            .start = i * chunk_size,
            .end = (i == num_threads - 1) ? size : (i + 1) * chunk_size,
            .target = target,
            .result_indices = result_indices,
            .result_count = &count,
            .mutex = &mutex
        };
        pthread_create(&threads[i], NULL, search_all_thread, &thread_data[i]);
    }
    
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
    
    sort_indices(result_indices, count);
    
    *result_count = count;
    free(threads);
    free(thread_data);
    pthread_mutex_destroy(&mutex);
    return result_indices;
}

int main() {
    int array[] = {1, 5, 3, 7, 5, 9, 2, 5, 8};
    size_t size = sizeof(array) / sizeof(array[0]);
    int target = 5;
    int num_threads = 4;
    size_t count;
    
    int* indices = parallel_search_all(array, size, target, num_threads, &count);
    
    if (count > 0) {
        printf("Found at indices:");
        for (size_t i = 0; i < count; ++i) {
            printf(" %d", indices[i]);
        }
        printf("\n");
    } else {
        printf("Not found\n");
    }
    
    free(indices);
    return 0;
}
