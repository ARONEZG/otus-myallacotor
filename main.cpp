#include <iostream>
#include <vector>

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

    myAlloc() noexcept : _cur_ptr(nullptr), _end_ptr(nullptr) {}

    template <typename U>
    myAlloc(const myAlloc<U, ChunkSize, Expandable>&) noexcept 
        : _cur_ptr(nullptr), _end_ptr(nullptr) {}

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

        if (!_cur_ptr || _cur_ptr + n > _end_ptr) {
            if (Expandable && _chunks.empty()) {
                allocate_new_chunk();
            }

            throw std::bad_alloc();
        }


        auto result = _cur_ptr;
        _cur_ptr += n;
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
        pointer new_poiner = ::operator new(ChunkSize * sizeof(T));
        _cur_ptr = new_poiner;
        _end_ptr = new_poiner + ChunkSize;
        _chunks.push_back(new_poiner);
    }

    pointer _cur_ptr;
    pointer _end_ptr;
    std::vector<pointer> _chunks;
};

int main() {

    std::vector<int, myAlloc<int, 50>> v;

}