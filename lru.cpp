#include <assert.h>
#include <iostream>
#include <optional>
#include <unordered_map>

template <typename T>
struct Node
{
    T value;
    Node *next = nullptr;
    Node *prev = nullptr;
};

template <typename K, typename V>
class LRU
{
    int limit;
    int size = 0;
    Node<V> *head;
    Node<V> *tail;
    std::unordered_map<K, Node<V>*> lookupNode;
    std::unordered_map<Node<V>*, K> lookupKey;
public:
    LRU(int limit = 1024): limit{limit}
    {
        head = tail = nullptr;
    }

    void insert(K key, V value)
    {
        if (lookupNode.contains(key))
        {
            auto node = lookupNode[key];
            node->value = value;
            head->prev = node;
            node->next = head;
            head = node;
            return;
        }

        auto node = new Node<V>{value};
        size += 1;
        if (head == nullptr)
        {
            assert(tail == nullptr);
            head = tail = node;
            lookupNode.insert({key, node});
            lookupKey.insert({node, key});
            return;
        }


        if(size > limit)
        {
            auto delete_key  = lookupKey[tail];
            auto delete_node = tail;
            tail = tail->prev;
            lookupKey.erase(delete_node);
            lookupNode.erase(delete_key);
            delete delete_node;
        }

        lookupNode.insert({key, node});
        lookupKey.insert({node, key});
        head->prev = node;
        node->next = head;
        head = node;

    }

    std::optional<V> lookup(K key)
    {
        if (!lookupNode.contains(key)) return {};

        auto node = lookupNode[key];

        if (node->prev)
        {
            node->prev->next = node->next;
        }

        if (node->next)
        {
            node->next->prev = node->prev;
        }

        node->prev = head->prev;
        node->next = head;
        head = node;

        return node->value;
    }
};
//
// int main(void)
// {
//     LRU<int, int> lru{};
//     lru.insert(69, 420);
//     lru.insert(79, 430);
//     lru.insert(89, 440);
//     lru.insert(99, 450);
//     return 0;
// }
