//
// Created by zhoupenghui on 2021/1/31.
//

#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <cstring>

namespace memory_pool {
static constexpr size_t kMaxBlockLen = 512;

struct MemoryNode
{
    MemoryNode() {
        memset(block, 0x00, sizeof(block));
    }

    ~MemoryNode() = default;

    char block[kMaxBlockLen];
    bool is_use = false;
    MemoryNode *next = nullptr;
};

class MemoryPool
{
public:
    MemoryPool() = default;
    ~MemoryPool() {
        MemoryNode* node = this->next;
        while (node) {
            auto tmp = node;
            node = node->next;
            delete tmp;
            tmp = nullptr;
        }
    }

    void init(size_t count = 100) {
        MemoryNode** cur = &(this->next);
        size_t index = 0;
        while (index++ < count) {
            *cur = new MemoryNode();
            cur = &((*cur)->next);
        }
    }

    template <typename T, typename... Args>
    T* find_node(Args&&... args) {
        size_t len = sizeof(T);
        if (len > kMaxBlockLen) {
            return nullptr;
        }

        MemoryNode* node = this->next;
        while(node) {
            if (!node->is_use) {
                break;
            }
            node = node->next;
        }

        if (node) {
            node->is_use = true;
            return new(node->block)T(std::forward<Args>(args)...);
        }
        return nullptr;
    }

    template<typename T>
    bool free_node(T* t) {
        MemoryNode* node = (MemoryNode* )t;
        if (node) {
            node->is_use = false;
            memset(node->block, 0x00, sizeof(node->block));
            return true;
        }
        return false;
    }

    void clear() {
        MemoryNode* node = this->next;
        while(node) {
            memset(node->block, 0x00, sizeof(node->block));
            node->is_use = false;
        }
    }

private:
    MemoryNode *next;
};
}

#endif // MEMORY_POOL_H