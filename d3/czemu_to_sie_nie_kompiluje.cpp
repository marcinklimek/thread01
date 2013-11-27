#include<iostream>
#include<boost/thread/thread.hpp>

boost::mutex mutex;

class A
{
public:
    A() : a(0) {}

    void operator()()
    {
        while(true)
        {
            boost::mutex::scoped_lock lock(mutex);
            std::cout << a << std::endl;

            boost::this_thread::sleep( boost::posix_time::seconds(1) );
        }
    }
private:
    int a;
};

int main()
{
    boost::thread thr1( A() );
    boost::thread thr2( A() );
    thr1.join();
    thr2.join();
}
