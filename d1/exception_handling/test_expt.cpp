#include <boost/exception/all.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>

class DelegeeThread
{
public:
    void operator()( boost::exception_ptr& excPtr )
    {
        try
        {
            int counter = 0;
            while( true )
            {
                // Do some work

                if( ++counter == 1000000000 )
                {
                    throw boost::enable_current_exception( std::exception( "An error happened!" ) );
                }

            }
        }
        catch( ... )
        {
            // type of exception is preserved
            excPtr = boost::current_exception();
        }
    }
};

class DelegatorThread
{
public:
    DelegatorThread() : 
      delegeeThread( boost::bind( &DelegeeThread::operator(), boost::ref( delegee ), boost::ref( exceptionPtr ) ) )
      {
          // launches DelegeeThread
      }

    void wait()
    {
        // wait for a worker thread to finish
        delegeeThread.join();

        // Check if a worker threw
        if( exceptionPtr )
        {
            // if so, rethrow on the wait() caller thread
            boost::rethrow_exception( exceptionPtr );
        }
    }

private:
    DelegeeThread           delegee;
    boost::thread           delegeeThread;
    boost::exception_ptr    exceptionPtr;
};


int main () 
{
    try
    {
        // asynchronous work starts here
        DelegatorThread dt;

        // do some other work on a main thread...

        dt.wait();

    }
    catch( std::exception& e )
    {
        std::cout << e.what();
    }

    return 0;
}