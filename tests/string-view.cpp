#define OK_IMPLEMENTATION
#include "../ok.hpp"

using namespace ok;

int main() {
#define CSTR_LITERAL "hello"
    StringView sv = "hello"_sv;
    OK_ASSERT(sv.count = strlen(CSTR_LITERAL));
    OK_ASSERT(strcmp((const char*)sv.data, CSTR_LITERAL) == 0);
}
