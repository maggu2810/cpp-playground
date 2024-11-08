//
// Created by maggu2810 on 11/7/24.
//

#ifndef VISIT_UTILS_HXX
#define VISIT_UTILS_HXX

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

#endif //VISIT_UTILS_HXX
