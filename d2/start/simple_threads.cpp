#include <iostream>
#include <boost/thread.hpp>

std::string async_fn()
{
    boost::this_thread::sleep( boost::posix_time::seconds(3));

    return "ABCD";
}


int mainxxx()
{
    boost::packaged_task<std::string> pt( async_fn );
    boost::unique_future<std::string> f = pt.get_future();
    boost::thread th( boost::move(pt) );

    while( !f.is_ready() )
    {
        std::cout << "!ready" << std::endl;
        boost::this_thread::sleep( boost::posix_time::millisec(300));
    }

    std::cout << f.has_value() << std::endl;
    std::cout << f.has_exception() << std::endl;
    std::cout << f.get() << std::endl;

}



class BackgroundTask {
    boost::promise<std::string> promise_;
public:
    void operator()()
    {
        boost::this_thread::sleep(boost::posix_time::seconds(3));
        promise_.set_value("Eureka");
    }

    boost::unique_future<std::string> get_future()
    {
        return promise_.get_future();
    }
};

void waiter(boost::unique_future<std::string>& f)
{
    std::cout << f.get() << std::endl;
}

int main()
{
    BackgroundTask bt;
    boost::unique_future<std::string> future = bt.get_future();

    std::cout << " ---------------------------- " << std::endl;

    boost::thread thd1(waiter, boost::ref(future));
    boost::thread thd2(boost::ref(bt));
    thd1.join();
    thd2.join();
}

