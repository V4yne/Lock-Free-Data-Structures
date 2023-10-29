#include <thread>
#include <vector>

#include "lock_free_memory_pool.h"

const int NUM_THREADS = 10;
const int OPERATIONS_PER_THREAD = 100;

void ThreadTask(LockFreeDataStructure::LockFreeMemoryPool<int>& pool) {
    for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
        int* data = pool.allocate();
        *data = i;
        std::thread::id threadId = std::this_thread::get_id();

        printf("Current thread ID: %d     %d\n", threadId, *data);
        //    pool.deallocate(data);
    }
}

int main() {
    LockFreeDataStructure::LockFreeMemoryPool<int> pool(NUM_THREADS *
                                                        OPERATIONS_PER_THREAD);

    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.push_back(std::thread(ThreadTask, std::ref(pool)));
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Done!" << std::endl;
    pool.print_allocated_size();
    int* data = pool.allocate();
    if (data == nullptr)
        std::cout << "Yes, pool has no available node." << std::endl;
    else
        std::cout << "Oh no!" << std::endl;
    return 0;
}
// g++ -std=c++17 concurrent-test.cpp lock_free_memory_pool.h
