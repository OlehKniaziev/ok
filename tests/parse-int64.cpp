#define OK_IMPLEMENTATION
#include "../ok.hpp"

using namespace ok;

int main() {
    int64_t i123, i_empty, i_neg;
    OK_ASSERT(parse_int64("123"_sv, &i123));
    OK_ASSERT(!parse_int64(""_sv, &i_empty));
    OK_ASSERT(parse_int64("-999"_sv, &i_neg));

    OK_ASSERT(i123 == 123);
    OK_ASSERT(i_neg == -999);
}
