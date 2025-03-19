#define OK_IMPLEMENTATION
#include "../ok.hpp"

#include <cmath>

using namespace ok;

int main() {
    ArenaAllocator arena{};

    int* one_int = arena.alloc<int>(1);
    OK_ASSERT(one_int != nullptr);

    const int funny_int = 123;
    *one_int = funny_int;

    OK_ASSERT(*one_int == funny_int);

    arena.reset();

    int* one_other_int = arena.alloc<int>(1);

    OK_ASSERT(one_int == one_other_int);
    OK_ASSERT(*one_other_int == funny_int);

    return 0;
}
