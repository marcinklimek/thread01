/*
 * thread_safe_queue.hpp
 *
 *  Created on: 2010-04-15
 *      Author: InfoTraining.KP
 */

#ifndef THREAD_SAFE_QUEUE_HPP_
#define THREAD_SAFE_QUEUE_HPP_

#include <queue>
#include <boost/thread.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

template <typename T>
class ThreadSafeQueue  
{
private:
	mutable boost::mutex mtx_;
	std::queue<T> queue_;
	boost::condition_variable condition_;
public:
	ThreadSafeQueue()
	{
	}

	ThreadSafeQueue(const ThreadSafeQueue& other)
	{
		boost::lock_guard<boost::mutex> lk(other.mtx_);
		queue_ = other.queue_;
	}

	void push(T value)
	{
		boost::lock_guard<boost::mutex> lk(mtx_);
		queue_.push(value);
		condition_.notify_one();
	}

	void wait_and_pop(T& value)
	{
		boost::unique_lock<boost::mutex> lk(mtx_);
		condition_.wait(lk, !boost::bind(&std::queue<T>::empty, boost::ref(queue_)));
		value = queue_.front();
		queue_.pop();
	}

	boost::shared_ptr<T> wait_and_pop()
	{
		boost::unique_lock<boost::mutex> lk(mtx_);
		condition_.wait(lk, !boost::bind(&std::queue<T>::empty, boost::ref(queue_)));
		boost::shared_ptr<T> result(new T(queue_.front()));
		queue_.pop();
		return result;
	}

	bool try_pop(T& value)
	{
		boost::lock_guard<boost::mutex> lk(mtx_);
		if (queue_.empty())
			return false;
		value = queue_.front();
		queue_.pop();
        return true;
	}

	boost::shared_ptr<T> try_pop()
	{
		boost::lock_guard<boost::mutex> lk(mtx_);
		boost::shared_ptr<T> result;
		if (queue_.empty())
			return result;
		result.reset(new T(queue_.front()));
		queue_.pop();
		return result;
	}
};

#endif /* THREAD_SAFE_QUEUE_HPP_ */
