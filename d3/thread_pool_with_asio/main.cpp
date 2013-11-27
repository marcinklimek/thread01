#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>

// the actual thread pool

struct ThreadPool {
    ThreadPool(std::size_t);
    template<class F>
    void enqueue(F f);
    ~ThreadPool();

private:
    // the io_service we are wrapping
    boost::asio::io_service service;
    using asio_worker = std::unique_ptr<boost::asio::io_service::work>;
    asio_worker working;
    // need to keep track of threads so we can join them
    std::vector<std::unique_ptr<boost::thread>> workers;
};

// the constructor just launches some amount of workers
ThreadPool::ThreadPool(size_t threads)
    : service()
    , working(new asio_worker::element_type(service))
{
    for ( std::size_t i = 0; i < threads; ++i ) {
        workers.push_back(
                    std::unique_ptr<boost::thread>(
                        new boost::thread([this] { service.run(); }))
                    );
    }
}

// add new work item to the pool
template<class F>
void ThreadPool::enqueue(F f) {
    service.post(f);
}

// the destructor joins all threads
ThreadPool::~ThreadPool() {    
    for ( std::size_t i = 0; i < workers.size(); ++i )
       workers[i]->join();
    working.reset();
    //service.run();
    service.stop();
}


int main() {
    // create a thread pool of 4 worker threads
    ThreadPool pool(4);

    // queue a bunch of "work items"
    for( int i = 0; i < 8; ++i ) {
        //std::cout << "task " << i << " created" << std::endl;
        pool.enqueue([i]
        {
            std::cout << "hello " << i << std::endl;
            //boost::this_thread::sleep(boost::posix_time::milliseconds(300));
            std::cout << "world " << i << std::endl;
            boost::this_thread::sleep(boost::posix_time::milliseconds(300));
        });
    }
}


