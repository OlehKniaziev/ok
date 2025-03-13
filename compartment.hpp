#ifndef COMPARTMENT_H_
#define COMPARTMENT_H_

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cstdarg>

#ifndef COMT_ASSERT
#define COMT_ASSERT(x) do { \
    if (!(x)) { \
    fprintf(stderr, "%s:%d: Assertion failed: %s\n", __FILE__, __LINE__, #x); \
    __builtin_trap(); \
    } \
    } while(0)
#endif // COMT_ASSERT

#define COMT_TODO() do { \
    fprintf(stderr, "%s:%d: TODO: Not implemented\n", __FILE__, __LINE__); \
exit(1); \
    } while (0)

#define COMT_UNUSED(arg) (void)(arg);

#define COMT_UNREACHABLE() do { \
    fprintf(stderr, "%s:%d: Encountered unreachable code\n", __FILE__, __LINE__); \
    __builtin_trap(); \
    } while (0)

namespace comt {
struct Allocator {
    // required methods
    virtual void* raw_alloc(size_t size) = 0;
    virtual void raw_dealloc(void* ptr, size_t size) = 0;

    // optional methods
    virtual void* raw_resize(void* ptr, size_t old_size, size_t new_size) {
        void* new_ptr = raw_alloc(new_size);
        memcpy(new_ptr, ptr, old_size);
        raw_dealloc(ptr, old_size);
        return new_ptr;
    }

    // provided methods
    template <typename T>
    inline T* alloc(size_t size = 1) {
        return (T*)raw_alloc(sizeof(T) * size);
    }

    template <typename T>
    inline void dealloc(T* ptr, size_t count) {
        return raw_dealloc((void*)ptr, count * sizeof(T));
    }

    template <typename T>
    inline T* resize(T* ptr, size_t old_size, size_t new_size) {
        T* new_ptr = alloc<T>(new_size);
        memcpy((void*)new_ptr, ptr, old_size * sizeof(T));
        dealloc<T>(ptr, old_size);
        return new_ptr;
    }
};

#ifndef COMT_PAGE_SIZE
#define COMT_PAGE_SIZE 4096
#endif // COMT_PAGE_SIZE

#if defined(__unix__) || defined(__unix) || defined(__APPLE__)

#include <sys/mman.h>
#include <unistd.h>

#define COMT_ARENA_ALLOC_PAGE(sz) (mmap(NULL, (sz), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0))
#define COMT_ARENA_DEALLOC_PAGE(page, size) (munmap((page), (size)))
#define COMT_ARENA_ALLOC_SMOL(sz) (sbrk((sz)))

#else

#error "Only UNIX-like systems are supported"

#endif // unix check

static inline uintptr_t align_to(uintptr_t size, uintptr_t align) {
    return size + ((align - (size & (align - 1))) & (align - 1));
}

struct ArenaAllocator : public Allocator {
    struct Region {
        size_t avail() const {
            COMT_ASSERT(off <= size);
            return size - off;
        }

        Region* next;
        void* data;
        size_t size;
        size_t off;
    };

    void* raw_alloc(size_t size) override;
    void raw_dealloc(void* ptr, size_t size) override;
    void* raw_resize(void* ptr, size_t old_size, size_t new_size) override;

    Region* alloc_region(size_t size);

    inline size_t avail() const {
        size_t result = 0;
        for (Region* r = head; r != nullptr; r = r->next) result += r->avail();
        return result;
    }

    inline void reserve(size_t bytes) {
        size_t available_bytes = avail();

        if (available_bytes < bytes) {
            size_t bytes_needed = bytes - available_bytes;
            Region* r = alloc_region(bytes_needed);
            r->next = head;
            head = r;
        }
    }

    inline void reset() {
        for (Region* r = head; r != nullptr; r = r->next) r->off = 0;
    }

    inline void free() {
        for (Region* r = head; r != nullptr; r = r->next) COMT_ARENA_DEALLOC_PAGE(head->data, head->size);
    }

    Region* head;
    Region* region_pool;
};

#define COMT_LIST_GROW_FACTOR(x) ((((x) + 1) * 3) >> 1)

template <typename T>
struct List {
    static List<T> alloc(Allocator* a, size_t cap = List::DEFAULT_CAP);

    static constexpr size_t DEFAULT_CAP = 7;

    void push(const T& item);
    void extend(List<T> other);
    void remove_at(size_t idx);

    List<T> copy(size_t start, size_t end) const;

    void reserve(size_t new_cap);

    size_t find_index(const T& elem);
    template <typename F>
    size_t find_index(F pred);

    inline T& operator [](size_t idx) {
        COMT_ASSERT(idx < count);

        return items[idx];
    }

    inline const T& operator [](size_t idx) const {
        COMT_ASSERT(idx < count);

        return items[idx];
    }

    T* items;
    size_t count;
    size_t capacity;
    Allocator* allocator;
};

#define COMT_TAB_META_OCCUPIED 0x01
#define COMT_TAB_IS_OCCUPIED(meta) ((meta) & COMT_TAB_META_OCCUPIED)
#define COMT_TAB_IS_FREE(meta) (!COMT_TAB_IS_OCCUPIED((meta)))

#define COMT_TABLE_GROWTH_FACTOR(x) (((x) + 1) * 3)

template <typename T>
struct Hash {};

template <typename T>
struct HashPtr {
    HashPtr(const T* v) : value{v} {}
    HashPtr(T* v) : value{v} {}
    const T* value;
};

// Default hash implementations
template <typename T>
struct Hash<HashPtr<T>> {
    static uint64_t hash(const HashPtr<T>& ptr) {
        return Hash<T>::hash(*ptr.value);
    }
};

template <>
struct Hash<uint32_t> {
    static uint64_t hash(const uint32_t& val) {
        return val;
    }
};

template <>
struct Hash<size_t> {
    static uint64_t hash(const size_t& val) {
        return val;
    }
};

template <typename T>
bool operator ==(const HashPtr<T>& lhs, const HashPtr<T>& rhs);

#define TABLE_FOREACH(tab, key, value, code) do { \
    for (size_t _tab_i = 0; _tab_i < (tab).capacity; _tab_i++) {\
    if (COMT_TAB_IS_FREE((tab).meta[_tab_i])) continue; \
    auto key = (tab).keys[_tab_i]; \
    auto value = (tab).values[_tab_i]; \
    code; \
    }\
    } while (0)

template <typename K, typename V>
struct Table {
    using Meta = uint8_t;

    static Table<K, V> alloc(Allocator* a, size_t capacity = Table::DEFAULT_CAPACITY);

    void put(const K& key, const V& value);
    bool get(const K& key, V* out);
    bool has(const K& key);

    static constexpr size_t DEFAULT_CAPACITY = 47;

    inline uint8_t load_percentage() const {
        return (uint8_t)((double)(count * 100) / (double)capacity);
    }

    K* keys;
    V* values;
    uint8_t* meta;
    size_t count;
    size_t capacity;
    Allocator* allocator;
};

using Char = uint8_t;

struct String;

struct StringView {
    String to_string(Allocator* a) const;

    const Char* data;
    size_t count;
};

inline bool operator ==(const StringView lhs, const StringView rhs) {
    if (lhs.count != rhs.count) return false;

    for (size_t i = 0; i < lhs.count; i++) {
        if (lhs.data[i] != rhs.data[i]) return false;
    }

    return true;
}

StringView operator ""_sv(const char*, size_t);

struct String {
    static constexpr Char NULL_CHAR = '\0';
    static constexpr size_t DEFAULT_CAPACITY = 7;

    static String alloc(Allocator* a, size_t capacity = DEFAULT_CAPACITY);

    static String alloc(Allocator* a, const Char* data, size_t data_len);
    static String alloc(Allocator* a, const Char* data);
    static String alloc(Allocator* a, const char* data);

    static String format(Allocator* a, const char* fmt, ...) __attribute__((format(printf, 2, 3)));

    inline const char* cstr() const {
        return reinterpret_cast<const char*>(data.items);
    }

    inline StringView view() const {
        return StringView{data.items, count()};
    }

    inline void push(Char character) {
        data.push(NULL_CHAR);
        data[data.count - 2] = character;
    }

    inline size_t count() const {
        COMT_ASSERT(data.count != 0);
        return data.count - 1;
    }

    List<Char> data;
};

#define COMT_SET_GROWTH_FACTOR COMT_TABLE_GROWTH_FACTOR

template <typename T>
struct Set {
    using Meta = uint8_t;

    static constexpr size_t DEFAULT_CAPACITY = 47;

    static Set<T> alloc(Allocator* a, size_t capacity = DEFAULT_CAPACITY);

    void put(const T& elem);
    bool has(const T& elem) const;

    inline uint8_t load_percentage() const {
        return (uint8_t)(100.0 * (double)count / (double)capacity);
    }

    Allocator* allocator;
    size_t capacity;
    size_t count;
    T* values;
    uint8_t* meta;
};

template <typename T>
struct Optional {
    Optional() : _has_value{false} {}

    Optional(T value) : _has_value{true}, value{value} {}

    inline bool has_value() const {
        return _has_value;
    }

    inline T& get() {
        COMT_ASSERT(has_value());
        return value;
    }

    bool _has_value;
    T value;

    static const Optional<T> NONE;
};

template <typename T>
struct Optional<T*> {
    Optional() : value{nullptr} {}

    Optional(T* value) : value{value} {}

    inline bool has_value() const {
        return value != nullptr;
    }

    inline T& get() {
        COMT_ASSERT(has_value());
        return *value;
    }

    T* value;

    static const Optional<T> NONE;
};

// HASH IMPLEMENTATION
template <typename T>
bool operator ==(const HashPtr<T>& lhs, const HashPtr<T>& rhs) {
    return *lhs.value == *rhs.value;
}

// OPTIONAL IMPLEMENTATION
template <typename T>
const Optional<T> Optional<T>::NONE = Optional<T>{};

template <typename T>
const Optional<T> Optional<T*>::NONE = Optional<T*>{};

// LIST IMPLEMENTATION
template <typename T>
List<T> List<T>::alloc(Allocator* a, size_t cap) {
    List<T> list{};
    list.items = a->alloc<T>(cap);
    list.count = 0;
    list.capacity = cap;
    list.allocator = a;

    return list;
}

template <typename T>
void List<T>::push(const T& item) {
    if (count >= capacity) {
        size_t new_capacity = COMT_LIST_GROW_FACTOR(capacity);
        items = allocator->resize<T>(items, capacity, new_capacity);
        capacity = new_capacity;
    }

    items[count++] = item;
}

template <typename T>
inline void List<T>::remove_at(size_t idx) {
    COMT_ASSERT(idx < count);

    for (size_t i = idx; i < count - 1; i++) {
        items[i] = items[i + 1];
    }

    count--;
}

template <typename T>
inline List<T> List<T>::copy(size_t start, size_t end) const {
    COMT_ASSERT(end >= start);

    auto res = List<T>::alloc(allocator, end - start);
    for (size_t i = start; i < end; i++) {
        res.push(items[i]);
    }
    return res;
}

template <typename T>
inline void List<T>::extend(List<T> other) {
    reserve(capacity + other.capacity);

    for (size_t i = 0; i < other.count; i++) {
        push(other.items[i]);
    }
}

template <typename T>
inline void List<T>::reserve(size_t new_cap) {
    if (new_cap <= capacity) {
        return;
    }

    items = allocator->resize<T>(items, capacity, new_cap);
    capacity = new_cap;
}

template <typename T>
inline size_t List<T>::find_index(const T& elem) {
    for (size_t i = 0; i < count; i++) {
        if (items[i] == elem) {
            return i;
        }
    }

    return (size_t)-1;
}

template <typename T>
template <typename F>
inline size_t List<T>::find_index(F pred) {
    for (size_t i = 0; i < count; i++) {
        if (pred(items[i])) {
            return i;
        }
    }

    return (size_t)-1;
}

// TABLE IMPLEMENTATION
template <typename K, typename V>
Table<K, V> Table<K, V>::alloc(Allocator* a, size_t capacity) {
    Table<K, V> tab{};

    tab.keys = a->alloc<K>(capacity);
    tab.values = a->alloc<V>(capacity);
    tab.meta = a->alloc<Meta>(capacity);
    tab.count = 0;
    tab.capacity = capacity;
    tab.allocator = a;

    return tab;
}

template <typename K, typename V>
void Table<K, V>::put(const K& key, const V& value) {
    if (load_percentage() >= 70) {
        auto new_table = Table<K, V>::alloc(allocator, COMT_TABLE_GROWTH_FACTOR(capacity));

        for (size_t i = 0; i < capacity; i++) {
            if (COMT_TAB_IS_OCCUPIED(meta[i])) {
                new_table.put(keys[i], values[i]);
            }
        }

        *this = new_table;
    }

    uint64_t idx = Hash<K>::hash(key) % capacity;

    while (true) {
        if (COMT_TAB_IS_FREE(meta[idx])) {
            meta[idx] |= COMT_TAB_META_OCCUPIED;
            values[idx] = value;
            keys[idx] = key;
            count++;
            return;
        }

        if (keys[idx] == key) {
            values[idx] = value;
            keys[idx] = key;
            return;
        }

        idx = (idx + 1) % capacity;
    }
}

template <typename K, typename V>
bool Table<K, V>::get(const K& key, V* out) {
    uint64_t idx = Hash<K>::hash(key) % capacity;
    uint64_t initial_idx = idx;

    do {
        if (COMT_TAB_IS_OCCUPIED(meta[idx]) && keys[idx] == key) {
            *out = values[idx];
            return true;
        }

        idx = (idx + 1) % capacity;
    } while (idx != initial_idx);

    return false;
}

template <typename K, typename V>
bool Table<K, V>::has(const K& key) {
    uint64_t idx = Hash<K>::hash(key) % capacity;
    uint64_t initial_idx = idx;

    do {
        if (COMT_TAB_IS_OCCUPIED(meta[idx]) && keys[idx] == key) {
            return true;
        }

        idx = (idx + 1) % capacity;
    } while (idx != initial_idx);

    return false;
}

// SET IMPLEMENTATION
template <typename T>
Set<T> Set<T>::alloc(Allocator* a, size_t capacity) {
    Set<T> set;
    set.allocator = a;
    set.count = 0;
    set.capacity = capacity;
    set.values = a->alloc<T>(capacity);
    set.meta = a->alloc<Meta>(capacity);
    return set;
}

template <typename T>
void Set<T>::put(const T& elem) {
    if (load_percentage() >= 70) {
        auto new_set = Set<T>::alloc(allocator, COMT_SET_GROWTH_FACTOR(capacity));

        for (size_t i = 0; i < capacity; i++) {
            if (COMT_TAB_IS_FREE(meta[i])) {
                continue;
            }

            new_set.put(values[i]);
        }

        *this = new_set;
    }

    uint64_t hash = Hash<T>::hash(elem);

    while (true) {
        if (COMT_TAB_IS_FREE(meta[hash])) {
            meta[hash] |= COMT_TAB_META_OCCUPIED;
            values[hash] = elem;
            count++;
            return;
        }

        if (values[hash] == elem) {
            values[hash] = elem;
            return;
        }

        hash = (hash + 1) % capacity;
    }
}

template <typename T>
bool Set<T>::has(const T& elem) const {
    uint64_t hash = Hash<T>::hash(elem);
    uint64_t idx = hash;

    do {
        if (COMT_TAB_IS_OCCUPIED(meta[idx]) && values[idx] == elem) {
            return true;
        }

        idx = (idx + 1) % capacity;
    } while (idx != hash);

    return false;
}

#ifdef COMPARTMENT_IMPLEMENTATION

static void _init_region(ArenaAllocator::Region* region, size_t size) {
    region->data = (uint8_t*)COMT_ARENA_ALLOC_PAGE(size);
    COMT_ASSERT(region->data != (void*)-1);
    region->size = size;
    region->off = 0;
    region->next = nullptr;
}

// ALLOCATORS IMPLEMENTATION
ArenaAllocator::Region* ArenaAllocator::alloc_region(size_t region_size) {
    using Region = ArenaAllocator::Region;

    region_size = align_to(region_size, COMT_PAGE_SIZE);

    Region* current_region = region_pool;

    while (current_region) {
        if (current_region->size - current_region->off >= sizeof(Region)) {
            auto* region = (Region*)((uint8_t*)current_region->data + current_region->off);

            _init_region(region, region_size);

            current_region->off += align_to(sizeof(Region), sizeof(void*));

            return region;
        }

        head = head->next;
    }

    current_region = (Region*)COMT_ARENA_ALLOC_SMOL(sizeof(Region));

    current_region->data = COMT_ARENA_ALLOC_PAGE(COMT_PAGE_SIZE);
    current_region->size = COMT_PAGE_SIZE;
    current_region->off = align_to(sizeof(Region), sizeof(void*));
    current_region->next = region_pool;

    this->region_pool = current_region;

    auto* resulting_region = (Region*)current_region->data;
    _init_region(resulting_region, region_size);
    return resulting_region;
}

void* ArenaAllocator::raw_alloc(size_t size) {
    ArenaAllocator::Region* head = this->head;

    size = align_to(size, sizeof(void*));

    while (head) {
        if (head->size - head->off >= size) {
            void* ptr = (void*)((uint8_t*)head->data + head->off);
            head->off += size;
            return ptr;
        }

        head = head->next;
    }

    size_t region_size = align_to(size, COMT_PAGE_SIZE);
    head = alloc_region(region_size);
    head->next = this->head;
    this->head = head;

    void* ptr = (void*)((uint8_t*)head->data + head->off);
    head->off += size;
    return ptr;
}

void ArenaAllocator::raw_dealloc(void* ptr, size_t size) {
    COMT_UNUSED(ptr);
    COMT_UNUSED(size);
}

void* ArenaAllocator::raw_resize(void* old_ptr, size_t old_size, size_t new_size) {
    auto* new_ptr = raw_alloc(new_size);
    memcpy(new_ptr, old_ptr, old_size);
    return new_ptr;
}

// STRING IMPLEMENTATION

String String::alloc(Allocator* a, size_t capacity) {
    String s;
    s.data = List<Char>::alloc(a, capacity + 1);
    s.data.push(NULL_CHAR);

    return s;
}

String String::alloc(Allocator* a, const Char* data, size_t data_len) {
    auto s = String::alloc(a, data_len);

    for (size_t i = 0; i < data_len; i++) s.push((Char)data[i]);

    return s;
}

String String::alloc(Allocator* a, const Char* data) {
    static_assert(sizeof(Char) == sizeof(char));

    size_t data_len = strlen((const char*)data);
    return String::alloc(a, data, data_len);
}

String String::alloc(Allocator* a, const char* data) {
    static_assert(sizeof(Char) == sizeof(char));

    size_t data_len = strlen(data);
    return String::alloc(a, (const Char*)data, data_len);
}

String String::format(Allocator* a, const char* fmt, ...) {
    va_list sprintf_args;

    int buf_size;
    va_start(sprintf_args, fmt);
    {
        char* sprintf_buf = nullptr;
        buf_size = vsnprintf(sprintf_buf, 0, fmt, sprintf_args);
        COMT_ASSERT(buf_size != -1);

        buf_size += 1;
    }
    va_end(sprintf_args);

    auto buf = String::alloc(a, buf_size);

    va_start(sprintf_args, fmt);
    {
        int bytes_written = vsnprintf((char*)buf.data.items, buf_size, fmt, sprintf_args);
        COMT_ASSERT(bytes_written != -1);
    }
    va_end(sprintf_args);

    buf.data.count = buf_size;

    return buf;
}

// STRING VIEW IMPLEMENTATION
StringView operator ""_sv(const char* cstr, size_t len) {
    static_assert(sizeof(Char) == sizeof(char));

    return StringView{(const Char*)cstr, len};
}

String StringView::to_string(Allocator* a) const {
    return String::alloc(a, data, count);
}

#endif
};

#endif // COMPARTMENT_H_
