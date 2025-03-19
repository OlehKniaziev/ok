#define OK_IMPLEMENTATION
#include "../ok.hpp"

using namespace ok;

int main() {
    File file;

    auto open_err = File::open(&file, __FILE__);
    OK_ASSERT(!open_err.has_value());
    OK_ASSERT(strcmp(file.path, __FILE__) == 0);

    List<uint8_t> buffer;
    auto read_err = file.read_full(temp_allocator, &buffer);
    OK_ASSERT(!read_err.has_value());

    String s = String::from(buffer);

    OK_ASSERT(s.starts_with("#define OK_IMPLEMENTATION"_sv));

    return 0;
}
