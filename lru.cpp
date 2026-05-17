#include <assert.h>
#include <concepts>
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>

template <typename T>
struct Node
{
    T value;
    std::shared_ptr<Node> next;
    std::weak_ptr<Node>   prev;

    explicit Node(T v) : value(std::move(v)), next(nullptr) {}
};

template<typename T>
concept Hashable = requires(T a)
{
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

template <Hashable K, typename V>
class LRU
{
    int limit;
    int size = 0;
    std::shared_ptr<Node<V>> head;
    std::shared_ptr<Node<V>> tail;
    std::unordered_map<K, std::shared_ptr<Node<V>>> lookupNode;
    std::unordered_map<std::shared_ptr<Node<V>>, K>  lookupKey;

    void detach(std::shared_ptr<Node<V>> node)
    {
        auto prev = node->prev.lock();
        auto next = node->next;

        if (prev) prev->next   = next;
        else      head         = next;

        if (next) next->prev   = prev;
        else      tail         = prev;

        node->prev.reset();
        node->next = nullptr;
    }

    void push_front(std::shared_ptr<Node<V>> node)
    {
        node->next = head;
        node->prev.reset();
        if (head) head->prev = node;
        head = node;
        if (!tail) tail = node;
    }

    void evict_tail()
    {
        if (!tail) return;
        auto key = lookupKey[tail];
        lookupKey.erase(tail);
        lookupNode.erase(key);
        detach(tail);
        --size;
    }

public:
    explicit LRU(int limit = 1024) : limit{limit} {}

    void insert(K key, V value)
    {
        if (lookupNode.contains(key))
        {
            auto node = lookupNode[key];
            node->value = value;
            detach(node);
            push_front(node);
            return;
        }

        if (size >= limit)
            evict_tail();

        auto node = std::make_shared<Node<V>>(std::move(value));
        lookupNode.emplace(key,  node);
        lookupKey.emplace(node, std::move(key));
        push_front(node);
        ++size;
    }

    std::optional<V> lookup(K key)
    {
        if (!lookupNode.contains(key)) return std::nullopt;
        auto node = lookupNode[key];
        detach(node);
        push_front(node);
        return node->value;
    }
};

// int main()
// {
//     LRU<int, int> lru{3};
//
//     lru.insert(1, 10);
//     lru.insert(2, 20);
//     lru.insert(3, 30);
//
//     assert(lru.lookup(1) == 10);
//     lru.insert(4, 40);
//     assert(!lru.lookup(2));
//     assert(lru.lookup(4) == 40);
//     assert(lru.lookup(3) == 30);
//
//     lru.insert(3, 99);
//     assert(lru.lookup(3) == 99);
//
// }
