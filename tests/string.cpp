#define COMPARTMENT_IMPLEMENTATION
#include "../compartment.hpp"

using namespace comt;

int main() {
    Allocator* a = temp_allocator;

    const char* hello = "hello";

    String s = String::alloc(a, hello);

    COMT_ASSERT(strcmp(s.cstr(), hello) == 0);

#define hello_world "hello world!"

    String message = String::format(a, "%s world!", hello);

    COMT_ASSERT(strcmp(message.cstr(), hello_world) == 0);

    message.append(" and friends"_sv);

    COMT_ASSERT(strcmp(message.cstr(), hello_world " and friends") == 0);

    const char* obviously_the_best_language = "C++";

    message.format_append(" from %s", obviously_the_best_language);

    COMT_ASSERT(strcmp(message.cstr(), hello_world " and friends from C++") == 0);
}
