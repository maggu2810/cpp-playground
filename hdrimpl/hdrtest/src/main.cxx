//
// Created by maggu2810 on 7/20/24.
//

#if FOO_ENABLED
#include <foo/Foo.hxx>
#endif

#if BAR_ENABLED
#include <bar/Bar.hxx>
#endif

#include <iostream>

int main(int argc, char *argv[]) {
#if FOO_ENABLED
    Foo foo;
    std::cout << foo.get() << "\n";
#endif
#if BAR_ENABLED
    Bar bar;
    std::cout << bar.get() << "\n";
#endif

    return 0;
}