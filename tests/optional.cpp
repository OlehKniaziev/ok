#define OK_IMPLEMENTATION
#include "../ok.hpp"

using namespace ok;

int main() {
    int a = 1;
    Optional<int*> a_opt{&a};
    OK_ASSERT(a_opt.has_value());
    OK_ASSERT(a_opt);
}
