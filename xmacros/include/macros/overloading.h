#ifndef MACRO_PLAYGROUND_OVERLOADING_H
#define MACRO_PLAYGROUND_OVERLOADING_H

#define GET_MACRO_0(NAME, ...) NAME
#define GET_MACRO_1(_1, NAME, ...) NAME
#define GET_MACRO_2(_1, _2, NAME, ...) NAME
#define GET_MACRO_3(_1, _2, _3, NAME, ...) NAME
#define GET_MACRO_4(_1, _2, _3, _4, NAME, ...) NAME
#define GET_MACRO_5(_1, _2, _3, _4, _5, NAME, ...) NAME
#define GET_MACRO_6(_1, _2, _3, _4, _5, _6, NAME, ...) NAME
#define GET_MACRO_7(_1, _2, _3, _4, _5, _6, _7, NAME, ...) NAME
#define GET_MACRO_8(_1, _2, _3, _4, _5, _6, _7, _8, NAME, ...) NAME
#define GET_MACRO_9(_1, _2, _3, _4, _5, _6, _7, _8, _9, NAME, ...) NAME

/* example */
#if 0
#define DO_0() 0
#define DO_1(x) 1
#define DO_2(x, y) 2

// allow 2, 1 and 0 arguments
#define DO_TEST_2_1_0(...) GET_MACRO_2(__VA_ARGS__ __VA_OPT__(,) DO_2, DO_1, DO_0)(__VA_ARGS__)

// allow 2 and 1 arguments
#define DO_TEST_2_1(...) GET_MACRO_2(__VA_ARGS__ __VA_OPT__(,) DO_2, DO_1)(__VA_ARGS__)

// allow 2 and 0 arguments
#define DO_TEST_2_0(...) GET_MACRO_2(__VA_ARGS__ __VA_OPT__(,) DO_2, , DO_0)(__VA_ARGS__)

#define DO_TEST DO_TEST_2_1_0
#endif

#endif //MACRO_PLAYGROUND_OVERLOADING_H