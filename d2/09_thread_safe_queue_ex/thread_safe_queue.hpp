#include <boost/thread.hpp>
#include <queue>

template <typename T>
class ThreadSafeQueue
{
private:
    std::queue<T> queue_;
    
public:
    ThreadSafeQueue()
    {
    }
    
    ThreadSafeQueue(const ThreadSafeQueue& other)
    {
        queue_ = other.queue_;
    }
    
    void push(T value)
    {
        queue_.push( value);
    }
    
    void pop(T& value)
    {
        value = queue_.front();
        queue_.pop();
    }
    
    bool try_pop(T& value)
    {
        if (queue_.empty())
            return false;

        value = queue_.front();
        queue_.pop();

        return true;
    }
    
    bool is_empty()
    {
    	return queue_.empty();
    }

};
