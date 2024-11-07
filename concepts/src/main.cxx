#include <concepts>
#include <memory>
#include <set>
#include <type_traits>
#include <cstdint>
#include <iostream>
#include <vector>

template<typename T>
concept foo1 = requires(T v)
{
    { v.f1() } -> std::same_as<bool>;
};

template<typename T>
concept foo2 = requires(T v)
{
    v.f2(std::declval<int>());
};

template<typename T>
concept foo3 = requires(T v, int i)
{
    { v.f3(i) } -> std::same_as<void>;
};

class Test {
public:
    bool f1() { return false; }

    void f2(int i) {
    }

    void f3(int i) {
    }
};

static_assert(foo1<Test>);
static_assert(foo2<Test>);
static_assert(foo3<Test>);

template<typename T>
void test(T arg) requires foo1<T> && foo2<T> {
    arg.f1();
    arg.f2(0);
}

#if 0
template<typename T> requires foo1<T> && foo2<T>
class Container {
public:
    void add(T *entry) {
        collection.insert(entry);
    }

private:
    std::set<T *> collection;
};
#endif

template<typename T, std::size_t SIZE>
concept IsSize = requires(T v)
{
    sizeof(T) == SIZE;
};

template<typename T, std::size_t SIZE>
concept PtrDerefTypeIsSize = requires(T v)
{
    requires std::is_pointer_v<T>;
    requires sizeof(*v) == SIZE;
};

template<typename T>
concept ByteContainer = requires(T v)
{
    { v.data() } -> PtrDerefTypeIsSize<1>;
};

template<ByteContainer T>
const void *test(T t) {
    auto data = t.data();
    std::cout << sizeof(*data) << std::endl;
    return data;
}

int main(int argc, char *argv[]) {
    const std::vector<std::int8_t> v1;
    const std::vector<std::uint8_t> v2;
    //std::vector<std::int16_t> v3;
    //std::vector<std::uint8_t> v4;
    test(v1);
    test(v2);
    //test(v3);
    //test(v4);
    return 0;
}
