#pragma once

#include<array>
#include<cstdlib>
#include<algorithm>

// min element first
template<typename T, size_t N = 1024>
struct priority_queue_bounded
{
    std::array<T, N> v;
    size_t sz = 0;

    priority_queue_bounded() { v.fill({}); } // touch memory

    T const & top() const { return v[0]; }
    size_t size() const { return sz; }
    void push(T const & t)
    {
        v[sz++] = t;
        std::push_heap(v.data(), v.data()+sz, [](T const & a, T const & b){ return b < a; });
    }
    void pop()
    {
        std::pop_heap(v.data(), v.data()+sz, [](T const & a, T const & b){ return b < a; });
        sz--;
    }
};
