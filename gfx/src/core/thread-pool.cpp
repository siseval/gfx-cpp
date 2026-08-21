#include "../../inc/gfx/core/thread-pool.h"

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace gfx
{
    ThreadPool::ThreadPool(const int num_threads)
        : _barrier(num_threads + 1)
        , _running(true)
        , _num_threads(num_threads)
    {
        for (int i = 0; i < num_threads; ++i)
        {
            _workers.emplace_back(
                [this] {
                    worker_loop();
                }
            );
        }
    }

    ThreadPool::~ThreadPool()
    {
        _running = false;
        _barrier.arrive_and_drop();
        for (auto& worker : _workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }
    
    std::shared_ptr<ThreadPool> ThreadPool::default_thread_pool() 
    {
        static std::weak_ptr<ThreadPool> cached;
        static std::mutex mutex;

        std::lock_guard<std::mutex> lock(mutex);
        if (auto pointer = cached.lock()) 
        {
            return pointer;
        }
        auto pointer = std::make_shared<ThreadPool>(std::thread::hardware_concurrency());
        cached = pointer;
        return pointer;
    }    

    int ThreadPool::get_num_threads() const
    {
        return _num_threads;
    }

    void ThreadPool::worker_loop()
    {
        while (_running)
        {
            _barrier.arrive_and_wait();

            if (!_running)
            {
                return;
            }

            while (true)
            {
                const int i = _next_index.fetch_add(1, std::memory_order_relaxed);
                if (i >= _total_work)
                {
                    break;
                }

                _work_fn(i);
            }

            _barrier.arrive_and_wait();
        }
    }
}
