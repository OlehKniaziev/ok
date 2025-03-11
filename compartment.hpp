#ifndef COMPARTMENT_H_
#define COMPARTMENT_H_

#ifndef COMT_NO_STD

#include <cstdlib>
#include <cstdio>

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

#endif

namespace comt {
#define LIST_GROW_FACTOR(x) ((((x) + 1) * 3) >> 1)

template <typename T>
struct List {
    static List<T> alloc(Arena* a, size_t cap = List::DEFAULT_CAP) {
        List<T> list{};
        list.items = (T*)arena_alloc(a, sizeof(T) * cap);
        list.count = 0;
        list.capacity = cap;
        list.arena = a;

        return list;
    }

    static constexpr size_t DEFAULT_CAP = 7;

    inline void push(const T& item) {
        if (count >= capacity) {
            items = (T*)arena_realloc(arena, items, sizeof(T) * capacity, sizeof(T) * LIST_GROW_FACTOR(capacity));
            capacity = LIST_GROW_FACTOR(capacity);
        }

        items[count++] = item;
    }

    inline void remove_at(size_t idx) {
        COMT_ASSERT(idx < count);

        for (size_t i = idx; i < count - 1; i++) {
            items[i] = items[i + 1];
        }

        count--;
    }

    inline List<T> copy(size_t start, size_t end) const {
        COMT_ASSERT(end >= start);

        auto res = List<T>::alloc(arena, end - start);
        for (size_t i = start; i < end; i++) {
            res.push(items[i]);
        }
        return res;
    }

    inline void extend(List<T> other) {
        reserve(capacity + other.capacity);

        for (size_t i = 0; i < other.count; i++) {
            push(other.items[i]);
        }
    }

    inline void reserve(size_t new_cap) {
        if (new_cap <= capacity) {
            return;
        }

        items = (T*)arena_realloc(arena, items, sizeof(T) * capacity, sizeof(T) * new_cap);
        capacity = new_cap;
    }

    inline size_t find_index(const T& elem) {
        for (size_t i = 0; i < count; i++) {
            if (items[i] == elem) {
                return i;
            }
        }

        return (size_t)-1;
    }

    template <typename F>
    inline size_t find_index(F pred) {
        for (size_t i = 0; i < count; i++) {
            if (pred(items[i])) {
                return i;
            }
        }

        return (size_t)-1;
    }

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
    Arena* arena;
};

#define TAB_META_OCCUPIED 0x01
#define TAB_IS_OCCUPIED(meta) ((meta) & TAB_META_OCCUPIED)
#define TAB_IS_FREE(meta) (!TAB_IS_OCCUPIED((meta)))

#define TABLE_GROWTH_FACTOR(x) (((x) + 1) * 3)

template <typename T>
struct Hash {};

template <typename T>
struct HashPtr {
    HashPtr(const T* v) : value{v} {}
    HashPtr(T* v) : value{v} {}
    const T* value;
};

template <typename T>
struct Hash<HashPtr<T>> {
    static uint64_t hash(const HashPtr<T>& ptr) {
        return Hash<T>::hash(*ptr.value);
    }
};

template <typename T>
bool operator ==(const HashPtr<T>& lhs, const HashPtr<T>& rhs) {
    return *lhs.value == *rhs.value;
}

template <typename T>
std::ostream& operator <<(std::ostream& out, const HashPtr<T>& rhs) {
    return out << *rhs.value;
}

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

#define TABLE_FOREACH(tab, key, value, code) do { \
    for (size_t _tab_i = 0; _tab_i < (tab).capacity; _tab_i++) {\
    if (TAB_IS_FREE((tab).meta[_tab_i])) continue; \
    auto key = (tab).keys[_tab_i]; \
    auto value = (tab).values[_tab_i]; \
    code; \
    }\
    } while (0)

template <typename K, typename V>
struct Table {
    static Table<K, V> alloc(Arena* a, size_t capacity = Table::DEFAULT_CAPACITY) {
        Table<K, V> tab{};

        tab.keys = (K*)arena_alloc(a, sizeof(K) * capacity);
        tab.values = (V*)arena_alloc(a, sizeof(V) * capacity);
        tab.meta = (uint8_t*)arena_alloc(a, sizeof(uint8_t) * capacity);
        tab.count = 0;
        tab.capacity = capacity;
        tab.arena = a;

        return tab;
    }

    static constexpr size_t DEFAULT_CAPACITY = 47;

    void put(const K& key, const V& value) {
        if (load_percentage() >= 70) {
            auto new_table = Table<K, V>::alloc(arena, TABLE_GROWTH_FACTOR(capacity));

            for (size_t i = 0; i < capacity; i++) {
                if (TAB_IS_OCCUPIED(meta[i])) {
                    new_table.put(keys[i], values[i]);
                }
            }

            *this = new_table;
        }

        uint64_t idx = Hash<K>::hash(key) % capacity;

        while (true) {
            if (TAB_IS_FREE(meta[idx])) {
                meta[idx] |= TAB_META_OCCUPIED;
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

    bool get(const K& key, V* out) {
        uint64_t idx = Hash<K>::hash(key) % capacity;
        uint64_t initial_idx = idx;

        do {
            if (TAB_IS_OCCUPIED(meta[idx]) && keys[idx] == key) {
                *out = values[idx];
                return true;
            }

            idx = (idx + 1) % capacity;
        } while (idx != initial_idx);

        return false;
    }

    bool has(const K& key) {
        uint64_t idx = Hash<K>::hash(key) % capacity;
        uint64_t initial_idx = idx;

        do {
            if (TAB_IS_OCCUPIED(meta[idx]) && keys[idx] == key) {
                return true;
            }

            idx = (idx + 1) % capacity;
        } while (idx != initial_idx);

        return false;
    }

    uint8_t load_percentage() const {
        return (uint8_t)((double)(count * 100) / (double)capacity);
    }

    K* keys;
    V* values;
    uint8_t* meta;
    size_t count;
    size_t capacity;
    Arena* arena;
};

struct String {
    using Char = uint8_t;

    static constexpr Char NULL_CHAR = '\0';
    static constexpr size_t DEFAULT_CAPACITY = 7;

    static String alloc(Arena* a, size_t capacity = DEFAULT_CAPACITY) {
        String s;
        s.data = List<Char>::alloc(a, capacity + 1);
        s.data.push(NULL_CHAR);

        return s;
    }

    static String alloc(Arena* a, const char* data, size_t data_len) {
        auto s = String::alloc(a, data_len);

        for (size_t i = 0; i < data_len; i++) s.push((Char)data[i]);

        return s;
    }

    static String alloc(Arena* a, const char* data) {
        size_t data_len = strlen(data);
        return String::alloc(a, data, data_len);
    }

    static String format(Arena* a, const char* fmt, ...) {
        va_list sprintf_args;

        int buf_size;
        va_start(sprintf_args, a);
        {
            char* sprintf_buf = nullptr;
            buf_size = vsprintf(sprintf_buf, fmt, sprintf_args);
            COMT_ASSERT(buf_size != -1);

            buf_size += 1;
        }
        va_end(sprintf_args);

        auto buf = String::alloc(a, buf_size);

        va_start(sprintf_args, a);
        {
            int bytes_written = vsprintf((char*)buf.data.items, fmt, sprintf_args);
            COMT_ASSERT(bytes_written != -1);
        }
        va_end(sprintf_args);

        buf.data.count = buf_size;

        return buf;
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

template <typename T>
struct Set {
    static Set<T> alloc(Arena* a, size_t capacity = DEFAULT_CAPACITY) {
        Set<T> set;
        set.arena = a;
        set.count = 0;
        set.capacity = capacity;
        set.values = (T*)arena_alloc(a, sizeof(T) * capacity);
        set.meta = (uint8_t*)arena_alloc(a, sizeof(uint8_t) * capacity);
        return set;
    }

    static constexpr size_t DEFAULT_CAPACITY = 47;

    inline void put(const T& elem) {
        if (load_percentage() >= 70) {
            auto new_set = Set<T>::alloc(arena, TABLE_GROWTH_FACTOR(capacity));

            for (size_t i = 0; i < capacity; i++) {
                if (TAB_IS_FREE(meta[i])) {
                    continue;
                }

                new_set.put(values[i]);
            }

            *this = new_set;
        }

        uint64_t hash = Hash<T>::hash(elem);

        while (true) {
            if (TAB_IS_FREE(meta[hash])) {
                meta[hash] |= TAB_META_OCCUPIED;
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

    bool has(const T& elem) const {
        uint64_t hash = Hash<T>::hash(elem);
        uint64_t idx = hash;

        do {
            if (TAB_IS_OCCUPIED(meta[idx]) && values[idx] == elem) {
                return true;
            }

            idx = (idx + 1) % capacity;
        } while (idx != hash);

        return false;
    }

    uint8_t load_percentage() const {
        return (uint8_t)(100.0 * (double)count / (double)capacity);
    }

    Arena* arena;
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

template <typename T>
const Optional<T> Optional<T>::NONE = Optional<T>{};

template <typename T>
const Optional<T> Optional<T*>::NONE = Optional<T*>{};
};

#endif // COMPARTMENT_H_
