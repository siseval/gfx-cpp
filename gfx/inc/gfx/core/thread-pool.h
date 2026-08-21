#pragma once

#include <atomic>
#include <barrier>
#include <functional>
#include <thread>
#include <vector>

namespace gfx
{
    class ThreadPool
    {
    public:

        explicit ThreadPool(int num_threads);

        ~ThreadPool();
        
        static std::shared_ptr<ThreadPool> default_thread_pool();

        template <typename FN>
        void run(const int count, FN&& fn)
        {
            _work_fn = std::forward<FN>(fn);
            _total_work = count;
            _next_index.store(0, std::memory_order_relaxed);

            _barrier.arrive_and_wait();

            _barrier.arrive_and_wait();
        }
        
        int get_num_threads() const;

    private:

        void worker_loop();

        std::vector<std::thread> _workers;
        std::barrier<> _barrier;

        std::atomic<bool> _running;
        std::atomic<int> _next_index;
        int _total_work;
        int _num_threads;

        std::function<void(int)> _work_fn;
    };
}
