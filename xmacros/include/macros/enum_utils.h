// Copyright [2024] [maggu2810]
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
