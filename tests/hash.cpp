#define COMPARTMENT_IMPLEMENTATION
#include "../compartment.hpp"

using namespace comt;

int main() {
    COMT_ASSERT(hash::fnv1(""_sv) == 0xCBF29CE484222325);
    COMT_ASSERT(hash::fnv1("123"_sv) == 0xD97FFA186C3A60BB);
    COMT_ASSERT(hash::fnv1("Compartment!"_sv) == 0xCCC956328581F308);

    return 0;
}
