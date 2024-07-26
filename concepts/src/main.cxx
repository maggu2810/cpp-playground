#include <concepts>
#include <memory>
#include <set>

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

    void f2(int i) {}

    void f3(int i) {}
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

int main(int argc, char *argv[]) {
    return 0;
}