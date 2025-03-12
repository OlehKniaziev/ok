#define COMPARTMENT_IMPLEMENTATION
#include "../compartment.hpp"

using namespace comt;

int main() {
    ArenaAllocator a{};

    const char* str = "hello";

    String s = String::alloc(&a, str);

    COMT_ASSERT(strcmp(s.cstr(), str) == 0);
}
