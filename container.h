#include "allocator.h"
#include <memory>

template <typename T, typename Allocator = std::allocator<T>>
class SimpleContainer {
public:
    using value_type = T;
    using allocator_type = Allocator;

private:
    struct Node {
        T data;
        Node* next;

        template <typename... Args>
        Node(Args&&... args) : data(std::forward<Args>(args)...), next(nullptr) {}
    };

    using NodeAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    NodeAllocator allocator;

    Node* _head;
    Node* _tail;
    size_t _size;

public:
    SimpleContainer() : _head(nullptr), _tail(nullptr), _size(0), allocator(NodeAllocator()) {}

    explicit SimpleContainer(const Allocator& alloc) 
        : allocator(alloc), _head(nullptr), _tail(nullptr), _size(0) {}

    ~SimpleContainer() {
        clear();
    }

    void push_back(const T& value) {
        Node* new_node = std::allocator_traits<NodeAllocator>::allocate(allocator, 1);
        std::allocator_traits<NodeAllocator>::construct(allocator, new_node, value);

        if (!_head) {
            _head = _tail = new_node;
        } else {
            _tail->next = new_node;
            _tail = new_node;
        }
        _size++;
    }

    void clear() {
        Node* current = _head;
        while (current) {
            Node* next = current->next;
            std::allocator_traits<NodeAllocator>::destroy(allocator, current);
            std::allocator_traits<NodeAllocator>::deallocate(allocator, current, 1);
            current = next;
        }
        _head = _tail = nullptr;
        _size = 0;
    }

    size_t size() const { return _size; }
    bool empty() const { return _size == 0; }

    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator(Node* node = nullptr) : current_node(node) {}

        iterator& operator++() {
            current_node = current_node->next;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        reference operator*() { return current_node->data; }
        pointer operator->() { return &current_node->data; }

        bool operator==(const iterator& other) const { return current_node == other.current_node; }
        bool operator!=(const iterator& other) const { return !(*this == other); }

    private:
        Node* current_node;
    };

    iterator begin() { return iterator(_head); }
    iterator end() { return iterator(nullptr); }
};