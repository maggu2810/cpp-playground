//
// Created by maggu2810 on 11/6/24.
//

#ifndef CLEANUP_HXX
#define CLEANUP_HXX

#include <functional>

class cleanup {
public:
    explicit cleanup(std::function<void()> &&func);

    ~cleanup();

private:
    std::function<void()> m_func;
};

#endif //CLEANUP_HXX
