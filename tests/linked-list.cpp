#define OK_IMPLEMENTATION
#include "../ok.hpp"

using namespace ok;

int main() {
    ArenaAllocator arena{};

    LinkedList<U32> ints = LinkedList<U32>::alloc(&arena);

    for (U32 i = 0; i < 1000; ++i) {
        ints.append(i);
    }

    OK_ASSERT(ints.tail->value == 999);
    OK_ASSERT(ints.tail->prev->value == 998);
    OK_ASSERT(ints.tail->next == nullptr);

    OK_ASSERT(ints.head->value == 0);
    OK_ASSERT(ints.head->next->value == 1);
    OK_ASSERT(ints.head->prev == nullptr);

    ints.prepend(1337);
    OK_ASSERT(ints.head->value == 1337);
    OK_ASSERT(ints.head->next->value == 0);

    return 0;
}
