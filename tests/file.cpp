#define COMPARTMENT_IMPLEMENTATION
#include "../compartment.hpp"

using namespace comt;

int main() {
    File file;

    auto open_err = File::open(&file, __FILE__);
    COMT_ASSERT(!open_err.has_value());
    COMT_ASSERT(strcmp(file.path, __FILE__) == 0);

    List<uint8_t> buffer;
    auto read_err = file.read_full(temp_allocator, &buffer);
    COMT_ASSERT(!read_err.has_value());

    String s = String::from(buffer);

    COMT_ASSERT(s.starts_with("#define COMPARTMENT_IMPLEMENTATION"_sv));

    return 0;
}
