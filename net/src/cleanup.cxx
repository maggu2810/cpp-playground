//
// Created by maggu2810 on 11/6/24.
//

#include "cleanup.hxx"

cleanup::cleanup(std::function<void()> &&func) : m_func{std::move(func)} {
}

cleanup::~cleanup() {
    m_func();
}
