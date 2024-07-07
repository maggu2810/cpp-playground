#ifndef MACRO_PLAYGROUND_ENUM_UTILS_H
#define MACRO_PLAYGROUND_ENUM_UTILS_H

#include <macros/overloading.h>

#include <utility>

#define DO_ENUM_DEFINITION_NAME(name) name,
#define DO_ENUM_DEFINITION_NAME_TEXT(name, text) name,
#define DO_ENUM_DEFINITION(...) GET_MACRO_2(__VA_ARGS__ __VA_OPT__(,) DO_ENUM_DEFINITION_NAME_TEXT, DO_ENUM_DEFINITION_NAME)(__VA_ARGS__)


#define DEFINE_ENUM_CLASS(enumerator_list) \
    enum class ENUM_HEAD_NAME { enumerator_list(DO_ENUM_DEFINITION) }

#define DO_ENUM_CLASS_CASE_RETURN_NAME(name) \
    case ENUM_HEAD_NAME::name:               \
        return #name;
#define DO_ENUM_CLASS_CASE_RETURN_TEXT(name, text) \
    case ENUM_HEAD_NAME::name:                     \
        return text;
#define DO_ENUM_CLASS_CASE_RETURN_NAME_OR_TEXT(...) GET_MACRO_2(__VA_ARGS__ __VA_OPT__(,) DO_ENUM_CLASS_CASE_RETURN_TEXT, DO_ENUM_CLASS_CASE_RETURN_NAME)(__VA_ARGS__)

#define DEFINE_ENUM_TO_CHARP_FUNC(enumerator_list)                  \
    std::string toString(const ENUM_HEAD_NAME ehn) {                \
        switch (ehn) {                                              \
            enumerator_list(DO_ENUM_CLASS_CASE_RETURN_NAME_OR_TEXT) \
        }                                                           \
        std::unreachable();                                         \
    }

#endif //MACRO_PLAYGROUND_ENUM_UTILS_H
