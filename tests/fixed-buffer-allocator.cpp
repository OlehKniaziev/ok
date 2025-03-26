#define OK_IMPLEMENTATION
#include "../ok.hpp"

using namespace ok;

int main() {
    const char* hello = "hello";
    const char* not_hello = "not hello";

    FixedBufferAllocator a{};

    String hello_string = String::alloc(&a, hello);

    OK_ASSERT(strcmp(hello_string.cstr(), hello) == 0);

    String not_hello_string = String::alloc(&a, not_hello);

    OK_ASSERT(strcmp(hello_string.cstr(), hello) == 0);
    OK_ASSERT(strcmp(not_hello_string.cstr(), not_hello) == 0);

    String* very_big_allocation = a.alloc<String>(1'000'000);
    OK_ASSERT(very_big_allocation == nullptr);

    a.buffer_off = 0;

    size_t bytes_count = align_up(FixedBufferAllocator::DEFAULT_PAGE_COUNT * OK_PAGE_SIZE, OK_PAGE_ALIGN);

    uint8_t* bytes = a.alloc<uint8_t>(bytes_count);
    OK_ASSERT(bytes != nullptr);
    OK_ASSERT(a.buffer_off == bytes_count);

    int* smol_int = a.alloc<int>();
    OK_ASSERT(smol_int != nullptr);
    OK_ASSERT(a.buffer_off == sizeof(void*));

    return 0;
}
