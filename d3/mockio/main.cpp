#include <time.h>

#include <iostream>
#include <boost/thread.hpp>
#include <queue>

#include <functional>
#include <boost/date_time/posix_time/posix_time.hpp>
//#include <boost/date_time/posix_time/posix_time_io.hpp>

void wait(u_int t)
{
	boost::this_thread::sleep( boost::posix_time::seconds(1) );
}



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

    bool peek(T& value)
    {
        boost::lock_guard<boost::mutex> lk(mtx_);
        if (queue_.empty())
            return false;

        value = queue_.front();

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

    bool empty()
    {
        boost::lock_guard<boost::mutex> lk(mtx_);
        return queue_.empty();
    }

};


struct IOData
{
    enum
    {
        NONE,  // brak zdarzen
        INIT,  // zainicjowana asynchroniczna operacja IO
        READY  // operacja asynchroniczna zakończona, dane skopiowane do bufora użyszkodnika
    };


    u_int id;
    boost::system_time t;
    u_int state;
    std::string  data;

    IOData() : state( IOData::INIT )
	{}

    IOData(u_int id_in, std::string data_in, boost::system_time delay) : id(id_in), 
    																	 data(data_in), 
    																	 t(delay), 
    																	 state( IOData::INIT )
	{
	}

    void change_state_ready()
    {
        std::cout << "[" << id << "] change state ready" << std::endl;

        if (state==INIT)
            state = READY;
    }

    boost::system_time get_processing_time()
    {
    	return t;
    }
};


struct MockIO
{
    boost::mutex receive_mutex;
    boost::mutex handlers_mutex;

    std::map< u_int, ThreadSafeQueue< IOData > > receive_queue;
    std::map< u_int, std::function<void(std::string)> > completion_handlers;

    boost::thread process_thread;

    enum
    {
        HANDLE_NONE  = 0,
        HANDLE_QUIT  = 1,
        HANDLE_FIRST = 5
    };

    MockIO() : process_thread( boost::bind(&MockIO::operator(), this  ) )
    {
        srand( time(NULL) );
    }

    ~MockIO()
    {
        //send(HANDLE_QUIT, "");

        if (process_thread.joinable())
            process_thread.join();
    }


    // strona usera
    void  receive( u_int id, std::string* buf);
    void  receive_async( u_int id, std::function<void(std::string)> completion_handler);

    u_int       poll();

    // strona admina
    void send( u_int id, std::string data );
    void send( u_int id );
    void register_handler(u_int id, std::function<void(std::string)> handler);
    void get( u_int id, std::string data );


    void operator()();


    void call_completion_handler(u_int id )
    {
        if ( completion_handlers.find(id) != completion_handlers.end() )
        {
            IOData iod;
            if ( receive_queue[id].try_pop( iod ) )
            {
                std::cout << "call completion handler "
                          << "    " << boost::posix_time::to_simple_string( processing_delay ).c_str()
                          << std::endl;

                completion_handlers[id]( iod.data );
            }

        }
    }

    bool empty()
    {
        boost::lock_guard<boost::mutex> lg( receive_mutex );

        for( auto q : receive_queue )
            if ( !q.second.empty() )
                return false;

        return true;
    }
};

void MockIO::receive( u_int id, std::string* buf)
{
    boost::lock_guard<boost::mutex> lg( receive_mutex );

    IOData iod;
    receive_queue[id].wait_and_pop( iod );

    *buf = iod.data;
}


void MockIO::receive_async( u_int id, std::function<void(std::string)> completion_handler)
{
    boost::lock_guard<boost::mutex> lg( receive_mutex );

    register_handler( id, completion_handler );
}

void MockIO::operator()()
{
    bool quit = false;
    while( !quit )
    {
        u_int id = poll();

        if ( id >= MockIO::HANDLE_FIRST )
        {
            call_completion_handler( id );
        }
        else if ( id == MockIO::HANDLE_QUIT )
        {
            quit = true;
        }

    }
    std::cout << "MockIO STOP" <<std::endl;
}

u_int MockIO::poll()
{

    for( auto q : receive_queue )
    {
        IOData iod;
        boost::lock_guard<boost::mutex> lg( receive_mutex );
        if ( q.second.peek( iod ) )
        {
            if ( iod.state == IOData::INIT )
            {
                if ( boost::get_system_time() >= iod.get_processing_time() )
                {
                    iod.change_state_ready();
                    return iod.id;	
                }
            }   
        }
    }

    return MockIO::HANDLE_NONE;
}

void MockIO::send( u_int id, std::string data )
{
    boost::lock_guard<boost::mutex> lg( receive_mutex );

    // symulacja oczekiwania na zakonczenie operacji asynchronicznej
	boost::system_time processing_delay = boost::get_system_time() + boost::posix_time::millisec( rand() % 3000 + 1000 );
    IOData iod(id, data, processing_delay);

    receive_queue[id].push( iod );

    std::cout << "MockIO::send #" << id << " -> " << data
              << "    " << boost::posix_time::to_simple_string( boost::get_system_time() ).c_str()
              << "    " << boost::posix_time::to_simple_string( processing_delay ).c_str() <<std::endl;
}

void MockIO::register_handler(u_int id, std::function<void(std::string)> handler)
{
    boost::lock_guard<boost::mutex> lg( handlers_mutex );

    completion_handlers[id] = handler;
}


bool test_mockio_add()
{
    MockIO mio;

    IOData iod;
    assert( mio.receive_queue[0].try_pop( iod ) == false );


    mio.send(0, "pumba");
    mio.receive_queue[0].wait_and_pop( iod );
    assert( iod.id == 0 );
    assert( iod.data == "pumba" );
    assert( iod.state == IOData::INIT );


    mio.send(100, "pumba pumba");
    mio.send(100, "pumba pumba 2");
    mio.receive_queue[100].wait_and_pop( iod );
    mio.receive_queue[100].wait_and_pop( iod );
    assert( iod.id == 100 );
    assert( iod.data == "pumba pumba 2" );
    assert( iod.state == IOData::INIT );


    mio.send(200, "pumba-pumba");
    std::string sval;
    mio.receive(200, &sval);
    assert( sval == "pumba-pumba" );

    // ASYNC 1

    std::cout << "receive_async\n"; 
    mio.receive_async(10, [](std::string val){ std::cout << "rcv async: " << val << std::endl; });

	std::cout << "1s delay\n";
    wait( 1 );   

    for(int i=0; i<10; i++)
    {
        mio.send(10, "pumba-pumba");
        wait( 1 );
    }

    // ASYNC 2

    u_int counter = 5;
    std::cout << "receive_async 2\n"; 
    mio.receive_async(11, [&mio, &counter](std::string val)
    	{ 
    		std::cout << "rcv async: ping-" << val << std::endl;
    		
    		if ( counter > 0 )
	    		mio.send(11, "pong");
            else
                mio.send(MockIO::HANDLE_QUIT,"");
    
    		--counter;
    	});


	std::cout << "1s delay\n";
    wait( 1 );   
	mio.send(11, "pong");
}


int main(int argc, char **argv) {

    test_mockio_add();

    return 0;
}
