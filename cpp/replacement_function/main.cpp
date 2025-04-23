#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <new>

void *operator new(std::size_t size) {
    void* ptr = malloc(size);
    if (ptr == nullptr) 
        throw std::bad_alloc();
    printf("new: ptr = %p, size = %lu\n", ptr, size);
    return ptr;
}

void operator delete(void* ptr) noexcept {
    printf("delete: ptr = %p\n", ptr);
    free(ptr);
}

int main() {
    int* p1 = new int;
    delete p1;
    short* p2 = new short;
    // delete p2;
}