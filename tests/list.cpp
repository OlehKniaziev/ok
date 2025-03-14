#define COMPARTMENT_IMPLEMENTATION
#include "../compartment.hpp"

using namespace comt;

int main() {
    size_t ints_cap = 90;

    List<int> ints = List<int>::alloc(temp_allocator, ints_cap);

    for (size_t i = 0; i < ints_cap; ++i) {
        ints.push(i);
    }

    COMT_ASSERT(ints.capacity == ints_cap);
    COMT_ASSERT(ints.count == ints_cap);

    ints.push(100);

    COMT_ASSERT(ints.capacity == COMT_LIST_GROW_FACTOR(ints_cap));

    return 0;
}
