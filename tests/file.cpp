#define _CRT_SECURE_NO_WARNINGS

#define OK_IMPLEMENTATION
#include "../ok.hpp"

using namespace ok;

char* read_file_to_cstr(const char* filepath) {
    FILE* f = fopen(filepath, "r");
    if (f == nullptr) {
        perror("failed to execute fopen");
        abort();
    }

    void* mem = static_allocator->raw_alloc(50'000'000);
    size_t nread = fread(mem, sizeof(char), 50'000'000, f);

    OK_ASSERT(nread != 0);

    ((char*)(mem))[nread] = '\0';

    return (char*)mem;
}

int main() {
    const char* test_file_path = "./tests/file.txt";

    File file;

    auto open_err = File::open(&file, test_file_path);
    if (open_err) {
        printf("could not open file: %s\n", File::error_string(temp_allocator, open_err.value).cstr());
        abort();
    }
    OK_ASSERT(strcmp(file.path, test_file_path) == 0);

    Optional<File::WriteError> write_err = file.write("HELLO!"_sv);
    if (write_err) {
        printf("could not write to file: %s\n", File::error_string(temp_allocator, write_err.value).cstr());
        abort();
    }

    List<uint8_t> buffer;
    auto read_err = file.read_full(temp_allocator, &buffer);
    OK_ASSERT(!read_err.has_value());

    String s = String::from(buffer);

    OK_ASSERT(s.starts_with("HELLO!"));

    OK_ASSERT(!file.close());

    const char* file_contents_cstr = read_file_to_cstr(test_file_path);

    OK_ASSERT(s == StringView{file_contents_cstr});

    return 0;
}
