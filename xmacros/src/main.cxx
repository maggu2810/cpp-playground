#include <iostream>
#include <string>

#include <macros/enum_utils.h>

#define FOR_LIST_OF_VARIABLES_ENUM_COLOR_1(DO, ...) \
    DO(red, "a wonderful red")                      \
    DO(green, "green like the gras")                \
    DO(blue, "the sky is blue")

#define FOR_LIST_OF_VARIABLES_ENUM_COLOR_2(DO, ...) \
    DO(red)                                         \
    DO(green)                                       \
    DO(blue)

#define ENUM_HEAD_NAME color1
DEFINE_ENUM_CLASS(FOR_LIST_OF_VARIABLES_ENUM_COLOR_1);

DEFINE_ENUM_TO_CHARP_FUNC(FOR_LIST_OF_VARIABLES_ENUM_COLOR_1)

#undef ENUM_HEAD_NAME

#define ENUM_HEAD_NAME color2
DEFINE_ENUM_CLASS(FOR_LIST_OF_VARIABLES_ENUM_COLOR_2);

DEFINE_ENUM_TO_CHARP_FUNC(FOR_LIST_OF_VARIABLES_ENUM_COLOR_2)

#undef ENUM_HEAD_NAME

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
    color1 col1{color1::red};
    color2 col2{color2::blue};

    std::cout << toString(col1) << '\n';
    std::cout << toString(col2) << '\n';
    return 0;
}


#if 0
#define FOR_LIST_OF_VARIABLES_ENUM_COLOR_1(DO, ...)             \
    DO(red, "a wonderful red" __VA_OPT__(, ) __VA_ARGS__)       \
    DO(green, "green like the gras" __VA_OPT__(, ) __VA_ARGS__) \
    DO(blue, "the sky is blue" __VA_OPT__(, ) __VA_ARGS__)

#define FOR_LIST_OF_VARIABLES_ENUM_COLOR_2(DO, ...) \
    DO(red __VA_OPT__(, ) __VA_ARGS__)              \
    DO(green __VA_OPT__(, ) __VA_ARGS__)            \
    DO(blue __VA_OPT__(, ) __VA_ARGS__)
#endif