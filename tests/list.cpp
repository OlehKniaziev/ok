#define OK_IMPLEMENTATION
#include "../ok.hpp"

using namespace ok;

int main() {
    size_t ints_cap = 90;

    List<int> ints = List<int>::alloc(temp_allocator, ints_cap);

    for (size_t i = 0; i < ints_cap; ++i) {
        ints.push(i);
    }

    OK_ASSERT(ints.capacity == ints_cap);
    OK_ASSERT(ints.count == ints_cap);

    ints.push(100);

    OK_ASSERT(ints.capacity == OK_LIST_GROW_FACTOR(ints_cap));

    return 0;
}
