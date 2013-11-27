#ifndef THREADGUARD_H_
#define THREADGUARD_H_

#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>

class ThreadGuard // TODO
{
    boost::thread& t;
public:
    explicit ThreadGuard(boost::thread& t_):
        t(t_)
    {}
    ~ThreadGuard()
    {
    if(t.joinable())
        t.join();
    }
    ThreadGuard(ThreadGuard const&) = delete;
    ThreadGuard& operator=(ThreadGuard const&) = delete;
};

#endif /* THREADGUARD_H_ */
