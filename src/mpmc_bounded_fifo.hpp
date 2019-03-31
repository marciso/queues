#pragma once

#include"utils.hpp"

#include<atomic>
#include<array>
#include<limits>
#include<algorithm>
#include<mutex>
#include<unordered_map>
#include<thread>
#include<sstream>


// based on https://www.linuxjournal.com/content/lock-free-multi-producer-multi-consumer-queue-ring-buffer

template<typename T, size_t N = 1024, size_t PRODUCERS = 128, size_t CONSUMERS = 128>
class mpmc_bounded_fifo
{
    public:
        template<typename U>
            void HOT push(U && x)
            {
                pos().head = head.load(std::memory_order_relaxed);
                pos().head = head.fetch_add(1); // reserve

                if( pos().head >= last_tail + N )
                    do {
                        last_tail = get_min_tail();
                        if ( pos().head < last_tail + N ) break;
                        yield();
                    } while(true);

                produce_at(pos().head & mask(), std::forward<U>(x));
                pos().head = std::numeric_limits<size_t>::max();
            }

        void HOT pop(T& x)
        {
            pos().tail = tail.load(std::memory_order_relaxed);
            pos().tail = tail.fetch_add(1); // claim we will use it

            if ( pos().tail >= last_head )
                do {
                    last_head = get_min_head();
                    if ( pos().tail < last_head ) break;
                    yield();
                } while(true);

            x = buffer[pos().tail & mask()];
            pos().tail = std::numeric_limits<size_t>::max();
        }

        constexpr size_t INLINE capacity() const { return N-1; }

        mpmc_bounded_fifo()
        {
            static_assert( N > 1 && is_power_of_2(N), "N expected to be power of 2");
            buffer.fill({}); // touch memory
            head = 0;
            tail = 0;
        }

        void register_this_thread() { id = get_thread_id(); }

    private:
        void HOT INLINE produce_at(size_t n, T && x) { buffer[n] = std::move(x); }
        void HOT INLINE produce_at(size_t n, T const& x) { buffer[n] = x; }

        size_t HOT INLINE get_min_tail() const
        {
            size_t r = tail.load(std::memory_order_relaxed);
            for(auto const & p : thread_positions) {
                auto const v = p.tail;
                compiler_barrier();
                if (v < r) r = v;
            }
            return r;
        }
        size_t HOT INLINE get_min_head() const
        {
            size_t r = head.load(std::memory_order_relaxed);
            for(auto const & p : thread_positions) {
                auto const v = p.head;
                compiler_barrier();
                if (v < r) r = v;
            }
            return r;
        }

        constexpr static inline size_t HOT INLINE mask()
        {
            static_assert( N > 1 && is_power_of_2(N), "N expected to be power of 2");
            return N - 1;
        }

        struct thread_info
        {
            size_t head = std::numeric_limits<size_t>::max();
            size_t tail = std::numeric_limits<size_t>::max();
        };

        static void INLINE yield() { std::this_thread::yield(); }

        thread_info & HOT INLINE pos() { return thread_positions[id]; }

        size_t get_thread_id(bool verbose = false) noexcept
        {
            std::lock_guard<std::mutex> lock(thread_mutex);
            std::thread::id id = std::this_thread::get_id();
            auto it = thread_ids.find(id);
            if (it == thread_ids.end()) {
                it = thread_ids.insert(std::pair<std::thread::id, std::size_t>(id, thread_idx++)).first;
            }
            if ( verbose )
            {
                std::ostringstream os;
                os << id;
                log_info("%s -> %ld\n", os.str().c_str(), it->second);
            }
            if ( it->second >= std::max(PRODUCERS, CONSUMERS))
            {
                fatal_error(
                        "Exceeded number of allowed threads, max(PRODUCERS=%ld, CONSUMERS=%ld)=%ld\n",
                        PRODUCERS, CONSUMERS, std::max(PRODUCERS, CONSUMERS));
            }
            return it->second;
        }


        // each variable on separate cache line
        alignas(64) std::atomic<size_t> tail;
        size_t last_head = 0;
        alignas(64) std::array<T, N> buffer;
        alignas(64) std::array<thread_info, std::max(PRODUCERS, CONSUMERS)> thread_positions;
        alignas(64) std::atomic<size_t> head;
        size_t last_tail = 0;

        alignas(64) size_t thread_idx = 0;
        std::mutex thread_mutex;
        std::unordered_map<std::thread::id, size_t> thread_ids;

        static thread_local size_t id;
};

template<typename T, size_t N, size_t PRODUCERS, size_t CONSUMERS>
thread_local size_t mpmc_bounded_fifo<T, N, PRODUCERS, CONSUMERS>::id = std::numeric_limits<int>::max();

