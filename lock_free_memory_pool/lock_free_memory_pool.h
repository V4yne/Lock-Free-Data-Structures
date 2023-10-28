#include <atomic>
#include <iostream>

namespace LockFreeDataStructure {

template <typename T>
class LockFreeMemoryPool {
   public:
    LockFreeMemoryPool(const __uint32_t& pool_size) {
        pool_size_ = pool_size;
        pool_ = new Memory_Node[pool_size];
        is_allocated_ = new std::atomic<bool>[pool_size];
        for (int i = 0; i < pool_size_ - 1; i++) {
            pool_[i].next = &pool_[i + 1];
            is_allocated_[i].store(false);
        }
        pool_[pool_size_ - 1].next = nullptr;
        is_allocated_[pool_size_ - 1].store(false);
        free_node_head_.store(PackVersionPtr(0, pool_));
    }

    ~LockFreeMemoryPool() {
        delete[] pool_;
        delete[] is_allocated_;
    }

    // todo: allocate
    // todo: deallocate
    T* allocate() {
        __uint128_t old_head;
        __uint128_t new_head;
        do {
            old_head = free_node_head_.load();
            __uint128_t old_version = GetVersion(old_head);
            Memory_Node* old_ptr = GetPtr(old_head);
            new_head = PackVersionPtr(old_version + 1, old_ptr->next);
        } while (free_node_head_.compare_exchange_strong(old_head, new_head) ==
                 false);
        Memory_Node* old_ptr = GetPtr(old_head);
        is_allocated_[old_ptr - pool_].store(true);
        return &(old_ptr->data);
    }

    void deallocate(T* node) {
        Memory_Node* node_ptr = reinterpret_cast<Memory_Node*>(node);
        __int128_t index = node_ptr - pool_;
        if (node_ptr < pool_ || index > pool_size_) return;
        bool status = true;
        if (is_allocated_[index].compare_exchange_strong(status, false) ==
            false) {
            printf("This memory_node has been deallocated.\n");
            return;
        }
        // printf("%d\n", *node);
        __uint128_t old_head;
        __uint128_t new_head;
        do {
            old_head = free_node_head_.load();
            Memory_Node* old_head_ptr = GetPtr(old_head);
            __uint128_t version = GetVersion(old_head);
            node_ptr->next = old_head_ptr;
            new_head = PackVersionPtr(version + 1, node_ptr);
        } while (free_node_head_.compare_exchange_strong(old_head, new_head) ==
                 false);
    }

   private:
    struct Memory_Node {
        T data;
        Memory_Node* next;
    };

    __uint128_t pool_size_;
    Memory_Node* pool_;

    std::atomic<bool>* is_allocated_;

    std::atomic<__uint128_t> free_node_head_;

    const static __uint128_t ptr_mask = 0x0000000000000000FFFFFFFFFFFFFFFF;

    __uint128_t GetVersion(const __uint128_t& ver_ptr) { return ver_ptr >> 64; }

    Memory_Node* GetPtr(const __uint128_t& ver_ptr) {
        return reinterpret_cast<Memory_Node*>(ver_ptr & ptr_mask);
    }

    __uint128_t PackVersionPtr(const __uint128_t& version, Memory_Node* ptr) {
        __uint128_t ptr_val = reinterpret_cast<__uint128_t>(ptr);
        return (version << 64) | (ptr_val & ptr_mask);
    }
};
}  // namespace LockFreeDataStructure

// use version to avoid ABA case