#define OK_IMPLEMENTATION
#include "../ok.hpp"

using namespace ok;

int main() {
#if OK_UNIX
    auto cmd = Command::alloc(temp_allocator, "echo");
    cmd.arg("hello").arg("world");

    auto err = cmd.exec();
    OK_ASSERT(!err.has_value());

    cmd = Command::alloc(temp_allocator, "cat");
    cmd.set_stdin("this was sent to cat (meow)\n"_sv);

    err = cmd.exec();
    OK_ASSERT(!err.has_value());
#endif

    return 0;
}
