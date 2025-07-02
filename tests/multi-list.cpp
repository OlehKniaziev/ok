#define OK_IMPLEMENTATION
#include "../ok.hpp"

using namespace ok;
using namespace ok::literals;

int main() {
    ArenaAllocator arena{};
    auto list = MultiList<U8, S16, StringView, F32>::alloc(&arena);
    list.push(1, -2, "HELLO"_sv, 1.5);

    U8 *bytes = list.get_items<U8>();
    S16 *shorts = list.get_items<S16>();
    StringView *strings = list.get_items<StringView>();
    F32 *floats = list.get_items<F32>();

    OK_ASSERT(list.count == 1);

    OK_ASSERT(bytes[0] == 1);
    OK_ASSERT(shorts[0] == -2);
    OK_ASSERT(strings[0] == "HELLO"_sv);
    OK_ASSERT(floats[0] == 1.5);
}
