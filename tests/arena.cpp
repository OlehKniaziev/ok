#define COMPARTMENT_IMPLEMENTATION
#include "../compartment.hpp"

#include <cmath>

using namespace comt;

int main() {
    ArenaAllocator arena{};

    int* one_int = arena.alloc<int>(1);
    COMT_ASSERT(one_int != nullptr);

    const int funny_int = 123;
    *one_int = funny_int;

    COMT_ASSERT(*one_int == funny_int);

    arena.reset();

    int* one_other_int = arena.alloc<int>(1);

    COMT_ASSERT(one_int == one_other_int);
    COMT_ASSERT(*one_other_int == funny_int);

    return 0;
}
