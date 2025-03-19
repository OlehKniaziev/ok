#define OK_IMPLEMENTATION
#include "../ok.hpp"

using namespace ok;

int main() {
    OK_ASSERT(hash::fnv1(""_sv) == 0xCBF29CE484222325);
    OK_ASSERT(hash::fnv1("123"_sv) == 0xD97FFA186C3A60BB);
    OK_ASSERT(hash::fnv1("Ok!"_sv) == 0xD840C3186B2B5F00);

    return 0;
}
