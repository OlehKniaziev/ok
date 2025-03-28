#define OK_IMPLEMENTATION
#include "../ok.hpp"

using namespace ok;

int main() {
    auto cmd = Command::alloc(temp_allocator, "echo");
    cmd.arg("hello").arg("world");

    auto err = cmd.exec();
    OK_ASSERT(!err.has_value());

    return 0;
}
