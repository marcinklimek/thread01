#include <boost/thread.hpp>
#include <boost/thread/thread_guard.hpp>
#include <boost/assign.hpp>
#include <vector>

void wait(u_int t)
{
    boost::this_thread::sleep( boost::posix_time::millisec( t ) );
}


template< typename T >
void bkg_copy( const std::vector<T>& from, std::vector<T>& to)
{

    auto it = from.begin();
    auto end = from.end();

    try
    {
        for(; it !=end; it++)
        {
            {
                boost::this_thread::disable_interruption di;
                std::cout << "Start copy: " << *it << std::endl;
                wait(100);
            }

            to.push_back( *it );

            wait(100);
            std::cout << "Stop copy: " << *it << std::endl;
        }
    }
    catch( const boost::thread_interrupted& e)
    {
        std::cout << "interrupted" << std::endl;
    }

}


void foo(int x)
{
    std::cout << x << std::endl;
    wait(100);
}


int main()
{
    std::vector<int> vec = {1,2,3,4,5,6};
    std::vector<int> vec_out;


    boost::thread thd(bkg_copy<int>, boost::cref(vec), boost::ref(vec_out));

    wait(310);

    thd.interrupt();

    thd.join();

    std::cout << "Finish" << std::endl;

}
