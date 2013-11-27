#include "thread_safe_queue.hpp"
#include <boost/thread.hpp>
#include <vector>


typedef std::function<void()> Task;

class ThreadPool;

class Worker
{
    ThreadPool *pool;

public:
    Worker(ThreadPool *pool) : pool(pool)
    {}

    void operator()();
};

class ThreadPool
{
    ThreadSafeQueue<Task> queue;

    friend class Worker;

public:
    ThreadPool()
    {
    }

    ~ThreadPool()
    {
    }

    void add_task(Task task)
    {
        queue.push(task);
    }

    template< class T, class F>
    boost::unique_future<T> add_future_task(F f)
    {
        auto task = std::make_shared<boost::packaged_task<T>>(f);
        boost::unique_future<T> res = task->get_future();
        queue.push( [task]() { (*task)(); } );
        
        return res;
    }

};


void Worker::operator ()()
{
    while(true)
    {

    }
}
