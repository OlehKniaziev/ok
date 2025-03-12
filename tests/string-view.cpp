#define COMPARTMENT_IMPLEMENTATION
#include "../compartment.hpp"

using namespace comt;

int main() {
#define CSTR_LITERAL "hello"
    StringView sv = "hello"_sv;
    COMT_ASSERT(sv.count = strlen(CSTR_LITERAL));
    COMT_ASSERT(strcmp((const char*)sv.data, CSTR_LITERAL) == 0);
}
