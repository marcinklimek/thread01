#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <map>
#include <string>
#include <atomic>
#include "handlers.h"
#include "ts_queue.h"

class ThreadPool;

class Worker
{
    ThreadPool* pool_;

public:
    Worker(ThreadPool* pool);
    void operator()();
};

typedef std::function<void()> Task;

const std::nullptr_t END_OF_WORK = nullptr;

class ThreadPool
{
    ThreadSafeQueue<Task> queue_;
    std::vector<std::thread> threads_;

    friend class Worker;

public:
    ThreadPool() //: stop_(false)
    {      
        for (int i = 0 ; i < number_of_hardware_threads() ; ++i)
        {
            threads_.emplace_back(std::thread{ Worker{this} });
        }
    }

    ~ThreadPool()
    {
        for (int i = 0 ; i < threads_.size() ; ++i)
            queue_.push(END_OF_WORK);

        for (int i = 0 ; i < threads_.size() ; ++i)
        {
            threads_.at(i).join();
        }
    }

    void add_task(std::function<void()> task)
    {
        queue_.push(task);
    }

private:
    int number_of_hardware_threads()
    {
        int n_of_threads = std::thread::hardware_concurrency();

        if (n_of_threads == 0)
            n_of_threads = 2;

        return n_of_threads;
    }
};

Worker::Worker(ThreadPool* pool) : pool_(pool) {}

void Worker::operator()()
{
    while(true)
    {
        Task current_task;

        pool_->queue_.wait_and_pop(current_task);

        if (current_task == END_OF_WORK)
            return;

        current_task();
   }
}

