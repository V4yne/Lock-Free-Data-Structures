#include "lock_free_memory_pool.h"
int main() {
    LockFreeDataStructure::LockFreeMemoryPool<int> int_memory_pool(100);
    int* ptrs[10];
    for (int i = 0; i < 10; i++) {
        ptrs[i] = int_memory_pool.allocate();
        *ptrs[i] = i;
    }
    for (int i = 0; i < 10; i++) {
        printf("%d\n", *ptrs[i]);
    }
    int_memory_pool.deallocate(ptrs[3]);
    printf("debug: here\n");
    int_memory_pool.deallocate(ptrs[3]);
    return 0;
}