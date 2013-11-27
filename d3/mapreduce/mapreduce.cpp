#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include "thread_safe_queue.hpp"

class ReduceFileSize
{
    ThreadSafeQueue<boost::filesystem::path> input_queue;
    ThreadSafeQueue<int64_t> output_queue;
    std::string path_;
    std::string filter_;
    size_t n_of_mappers;
    boost::promise<int64_t> promise;
    boost::thread_group threads;

    void producer()
    {
        //feeds input queue with matching file paths
        boost::filesystem::recursive_directory_iterator it(path_);
        boost::filesystem::recursive_directory_iterator end;
        for ( ; it != end ; ++it)
        {
            if(boost::filesystem::is_regular_file(*it))
            {
                if (boost::filesystem::extension(*it) == filter_)
                {
                    input_queue.push(*it);
                }
            }
        }
        for ( int i = 0 ; i < n_of_mappers ; i++)
        {
            input_queue.push( boost::filesystem::path() );
        }
    }

    void mapper()
    {
        // for file path in input queue puts file size in output queue
        while(true)
        {
            boost::filesystem::path path;
            input_queue.wait_and_pop(path);
            if (path.string() == "")
            {
                output_queue.push(-1);
                return;
            }
            output_queue.push(boost::filesystem::file_size(path));
        }
    }

    void reducer()
    {
        // sums file sizes from output queue
        int64_t suma = 0;
        size_t stopper = 0;
        while(stopper < n_of_mappers)
        {
            int64_t size =0;
            output_queue.wait_and_pop(size);
            if (size == -1)
            {
                stopper += 1;
            }
            else
            {
                suma += size;
                //std::cout << suma << std::endl;
            }
        }
        promise.set_value(suma);
    }

public:
    ReduceFileSize(std::string path, std::string filter)
        : path_(path), filter_(filter)
    {
        n_of_mappers = boost::thread::hardware_concurrency();
    }

    ~ReduceFileSize()
    {
        threads.join_all();
    }

    boost::unique_future<int64_t> run()
    {
        threads.create_thread( [&](){ producer(); });
        for (int i = 0 ; i < n_of_mappers ; ++i)
        {
            threads.create_thread( [&](){ mapper(); });
        }
        threads.create_thread( [&]() { reducer(); });
        return promise.get_future();
    }
};

int main()
{


    ReduceFileSize reducer("/home/leszek", ".cpp");
    boost::unique_future<int64_t> result = reducer.run();

    std::cout << "Total file size = " << result.get() << std::endl;
}
