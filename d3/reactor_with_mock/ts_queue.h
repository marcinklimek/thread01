#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

template <typename T>
class ThreadSafeQueue
{
private:
    mutable std::mutex mtx_;
    std::queue<T> queue_;
    std::condition_variable condition_;

public:
    ThreadSafeQueue()
    {
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lk(mtx_);
        return queue_.size();
    }

    ThreadSafeQueue(const ThreadSafeQueue& other)
    {
        std::lock_guard<std::mutex> lk(other.mtx_);
        queue_ = other.queue_;
    }

    void push(T value)
    {
        std::lock_guard<std::mutex> lk(mtx_);
        queue_.push(value);
        condition_.notify_one();
    }

    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lk(mtx_);
        condition_.wait(lk, [&]() {return !queue_.empty();} );
        value = queue_.front();
        queue_.pop();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mtx_);
        condition_.wait(lk, !std::bind(&std::queue<T>::empty, std::ref(queue_)));
        std::shared_ptr<T> result(new T(queue_.front()));
        queue_.pop();
        return result;
    }

    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lk(mtx_);
        if (queue_.empty())
            return false;

        value = queue_.front();
        queue_.pop();

        return true;
    }

    bool peek(T& value)
    {
        std::lock_guard<std::mutex> lk(mtx_);
        if (queue_.empty())
            return false;

        value = queue_.front();

        return true;
    }

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mtx_);
        std::shared_ptr<T> result;
        if (queue_.empty())
            return result;
        result.reset(new T(queue_.front()));
        queue_.pop();
        return result;
    }

};
