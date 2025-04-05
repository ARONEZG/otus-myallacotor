#include <iostream>
#include <vector>

#include <memory>
#include <vector>
#include <stdexcept>

template <typename T, size_t ChunkSize = 1024, bool Expandable = false>
class myAlloc {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <typename U>
    struct rebind {
        using other = myAlloc<U, ChunkSize, Expandable>;
    };

    myAlloc() noexcept : _current_ptr(nullptr), _end_ptr(nullptr) {}

    template <typename U>
    myAlloc(const myAlloc<U, ChunkSize, Expandable>&) noexcept 
        : _current_ptr(nullptr), _end_ptr(nullptr) {}

    ~myAlloc() {
        for (auto chunk : _chunks) {
            ::operator delete(chunk);
        }
    }

    pointer allocate(size_type n) {
        if (n == 0) {
            return nullptr;
        }
        if (n > ChunkSize) {
            throw std::bad_alloc();
        }

        if (!_current_ptr || _current_ptr + n > _end_ptr) {
            if (!Expandable && !_chunks.empty()) {
                throw std::bad_alloc();
            }
            allocate_new_chunk();
        }

        pointer result = _current_ptr;
        _current_ptr += n;
        return result;
    }

    void deallocate(pointer p, size_type n) noexcept {
        // No-op; memory is released when the allocator is destroyed
    }

    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::false_type;
    using propagate_on_container_swap = std::false_type;

private:
    void allocate_new_chunk() {
        pointer new_chunk = static_cast<pointer>(::operator new(ChunkSize * sizeof(value_type)));
        _chunks.push_back(new_chunk);
        _current_ptr = new_chunk;
        _end_ptr = new_chunk + ChunkSize;
    }

    std::vector<pointer> _chunks;
    pointer _current_ptr;
    pointer _end_ptr;
};

template <typename T, typename U, size_t ChunkSize, bool Expandable>
bool operator==(const myAlloc<T, ChunkSize, Expandable>&, 
                const myAlloc<U, ChunkSize, Expandable>&) {
    return false; // Allocators are never equal (stateful)
}

template <typename T, typename U, size_t ChunkSize, bool Expandable>
bool operator!=(const myAlloc<T, ChunkSize, Expandable>& a, 
                const myAlloc<U, ChunkSize, Expandable>& b) {
    return !(a == b);
}