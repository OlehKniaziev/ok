#define _CRT_SECURE_NO_WARNINGS

#define OK_IMPLEMENTATION
#include "../ok.hpp"

using namespace ok;

char* read_file_to_cstr(const char* filepath) {
    FILE* f = fopen(filepath, "r");
    OK_ASSERT(f);

    void* mem = static_allocator->raw_alloc(50'000'000);
    size_t nread = fread(mem, sizeof(char), 50'000'000, f);

    OK_ASSERT(nread != 0);

    ((char*)(mem))[nread] = '\0';

    return (char*)mem;
}

int main() {
    File file;

    auto open_err = File::open(&file, __FILE__);
    OK_ASSERT(!open_err.has_value());
    OK_ASSERT(strcmp(file.path, __FILE__) == 0);

    List<uint8_t> buffer;
    auto read_err = file.read_full(temp_allocator, &buffer);
    OK_ASSERT(!read_err.has_value());

    String s = String::from(buffer);

    OK_ASSERT(s.starts_with("#define _CRT_SECURE_NO_WARNINGS"));

    const char* file_contents_cstr = read_file_to_cstr(__FILE__);

    OK_ASSERT(s == StringView{file_contents_cstr});

    return 0;
}
