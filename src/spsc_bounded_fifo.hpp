#pragma once

#include "utils.hpp"

#include<atomic>
#include<array>

template<typename T, size_t N = 128>
class spsc_bounded_fifo
{
    public:
        template<typename U>
            bool HOT push(U && x)
            {
                size_t const t = tail.load(std::memory_order_relaxed);
                size_t const tt = increment(t);
                if(tt != head.load(std::memory_order_acquire))
                {
                    produce_at(t, std::forward<U>(x));
                    tail.store(tt, std::memory_order_release);
                    return true;
                }
                return false;
            }

        bool HOT pop(T& x)
        {
            size_t const h = head.load(std::memory_order_relaxed);
            if(h != tail.load(std::memory_order_acquire))
            {
                x = std::move(buffer[h]);
                head.store(increment(h), std::memory_order_release);
                return true;
            }
            return false;
        }

        template<typename F>
            size_t HOT consume_n(F callback)
            {
                size_t const h = head.load(std::memory_order_relaxed);
                size_t i = h;
                size_t const t = tail.load(std::memory_order_acquire);
                while(i != t && callback(buffer[i])) {
                    i = increment(i);
                }
                if ( h != i ) {
                    head.store(i, std::memory_order_release);
                    return (i >= h) ? (i-h) : (N+i-h);
                }
                return 0;
            }

        constexpr size_t INLINE capacity() const { return N-1; }

        spsc_bounded_fifo()
        {
            static_assert( N > 1 && is_power_of_2(N), "N expected to be power of 2");
            buffer.fill({}); // touch memory
            head = 0;
            tail = 0;
        }

    private:
        void HOT INLINE produce_at(size_t n, T && x) { buffer[n] = std::move(x); }
        void HOT INLINE produce_at(size_t n, T const& x) { buffer[n] = x; }

        constexpr static inline size_t HOT INLINE mask()
        {
            static_assert( N > 1 && is_power_of_2(N), "N expected to be power of 2");
            return N - 1;
        }

        static inline size_t HOT INLINE increment(size_t n)
        {
            static_assert( N > 1 && is_power_of_2(N), "N expected to be power of 2");
            return (n+1) & mask();
            // otherwise: return ((n+1) >= N) ? 0 : (n+1);
        }

        // each variable on separate cache line
        alignas(64) std::atomic<size_t> tail;
        alignas(64) std::array<T, N> buffer;
        alignas(64) std::atomic<size_t> head;
};

