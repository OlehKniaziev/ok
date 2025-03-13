#define COMPARTMENT_IMPLEMENTATION
#include "../compartment.hpp"

using namespace comt;

int main() {
    const char* hello = "hello";
    const char* not_hello = "not hello";

    FixedBufferAllocator a{};

    String hello_string = String::alloc(&a, hello);

    COMT_ASSERT(strcmp(hello_string.cstr(), hello) == 0);

    String not_hello_string = String::alloc(&a, not_hello);

    COMT_ASSERT(strcmp(hello_string.cstr(), hello) == 0);
    COMT_ASSERT(strcmp(not_hello_string.cstr(), not_hello) == 0);

    String* very_big_allocation = a.alloc<String>(1'000'000);
    COMT_ASSERT(very_big_allocation == nullptr);

    a.buffer_off = 0;

    size_t bytes_count = FixedBufferAllocator::DEFAULT_PAGE_COUNT * COMT_PAGE_SIZE;

    uint8_t* bytes = a.alloc<uint8_t>(bytes_count);
    COMT_ASSERT(bytes != nullptr);
    COMT_ASSERT(a.buffer_off == bytes_count);

    int* smol_int = a.alloc<int>();
    COMT_ASSERT(smol_int != nullptr);
    COMT_ASSERT(a.buffer_off == sizeof(void*));

    return 0;
}
