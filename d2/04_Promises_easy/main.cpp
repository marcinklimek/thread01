#include <iostream>
#include <string>
//#include <boost/thread.hpp>
#include <thread>
#include <future>


class BackgroundTask
{
    std::promise<std::string> promise_;

public:
    void operator()()
    {
        //std::this_thread::sleep(boost::posix_time::seconds(3));
        promise_.set_value("Eureka");
    }

    std::future<std::string> get_future()
    {
        return promise_.get_future();
    }
};

void waiter(std::future<std::string> f)
{
    std::cout << f.get() << std::endl;
}

int main()
{
    BackgroundTask bt{};
    //std::future<std::string> future = bt.get_future();
    //std::thread thd1(waiter, std::move(future));
    std::thread thd1(waiter, bt.get_future());
    std::thread thd2(std::ref(bt));
    thd1.join();
    thd2.join();
}
