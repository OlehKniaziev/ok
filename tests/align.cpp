#define OK_IMPLEMENTATION
#include "../ok.hpp"

using namespace ok;

int main() {
    OK_ASSERT(align_up(10, 8) == 16);
    OK_ASSERT(align_up(10, 16) == 16);
    OK_ASSERT(align_up(4, 8) == 8);
    OK_ASSERT(align_up(8, 8) == 8);
    OK_ASSERT(align_up(32, 4096) == 4096);

    OK_ASSERT(align_down(10, 8) == 8);
    OK_ASSERT(align_down(31, 16) == 16);
    OK_ASSERT(align_down(16, 8) == 16);
    OK_ASSERT(align_down(8, 4) == 8);
    OK_ASSERT(align_down(8, 8) == 8);
}
