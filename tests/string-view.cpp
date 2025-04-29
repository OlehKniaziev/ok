#define OK_IMPLEMENTATION
#include "../ok.hpp"

using namespace ok;

int main() {
#define CSTR_LITERAL "hello"
    StringView sv = "hello"_sv;
    OK_ASSERT(sv.count = strlen(CSTR_LITERAL));
    OK_ASSERT(strcmp((const char*)sv.data, CSTR_LITERAL) == 0);

    OK_ASSERT("1"_sv < "2"_sv);
    OK_ASSERT("3"_sv > "2"_sv);
    OK_ASSERT("123"_sv != "122"_sv);

    OK_ASSERT("hello"_sv.ends_with("lo"));
    OK_ASSERT("hello"_sv.starts_with("hel"));
}
