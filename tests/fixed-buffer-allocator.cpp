#define COMPARTMENT_IMPLEMENTATION
#include "../compartment.hpp"

using namespace comt;

int main() {
    FixedBufferAllocator a{};

    String s = String::alloc(&a, "hello");

    COMT_ASSERT(strcmp((const char*)s.data.items, "hello") == 0);

    String s1 = String::alloc(&a, "not hello");

    COMT_ASSERT(strcmp(s.cstr(), "hello") == 0);
    COMT_ASSERT(strcmp(s1.cstr(), "not hello") == 0);

    return 0;
}
