#pragma once

#include "thread_safe_queue.hpp"
#include <thread>
#include <vector>
#include <future>

typedef std::function<void()> Task;

class ThreadPool;

class Worker
{
    ThreadPool *pool;
public:
    Worker(ThreadPool *pool);
    void operator()();
};

class ThreadPool
{
    ThreadSafeQueue<Task> queue;
    std::vector<std::thread> threads;

    friend class Worker;

public:
    ThreadPool()
    {
        for (int i = 0 ; i < std::thread::hardware_concurrency() ; ++i)
        {
            threads.emplace_back(std::thread{ Worker {this} } );
        }
    }

    ~ThreadPool()
    {
        for (int i = 0 ; i < std::thread::hardware_concurrency() ; ++i)
        {
            queue.push( nullptr );
        }
        for (auto& thd : threads)
        {
            thd.join();
        }
    }

    void add_task(Task task)
    {
        queue.push(task);
    }

    template< class T, class F>
    std::future<T> add_future_task(F f)
    {
        auto task = std::make_shared<std::packaged_task<T()>>(f);
        std::future<T> res = task->get_future();
        queue.push( [task]() { (*task)(); } );
        return res;
    }

};

Worker::Worker(ThreadPool *pool) : pool(pool)
{

}

void Worker::operator ()()
{
    while(true)
    {
        Task current_task;
        pool->queue.wait_and_pop(current_task);
        if (current_task == nullptr)
        {
            return;
        }
        current_task();
    }
}
