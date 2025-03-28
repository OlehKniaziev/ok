#define OK_IMPLEMENTATION
#include "../ok.hpp"

using namespace ok;

int main() {
    uint32_t u32 = 123;
    int32_t  i32 = -123;

    uint64_t u64 = 10'000'000'000;
    int64_t  i64 = -10'000'000'000;

    OK_ASSERT(strcmp(to_string(temp_allocator, u32).cstr(), "123") == 0);
    OK_ASSERT(strcmp(to_string(temp_allocator, i32).cstr(), "-123") == 0);
    OK_ASSERT(strcmp(to_string(temp_allocator, u64).cstr(), "10000000000") == 0);
    OK_ASSERT(strcmp(to_string(temp_allocator, i64).cstr(), "-10000000000") == 0);

    return 0;
}
