
#include <iostream>
#include <boost/thread.hpp>
#include <queue>
#include <vector>
#include <functional>
#include <stdexcept>


template <typename IN, typename OUT>
class SyncMap;

template <typename IN, typename OUT>
class Worker
{
public:
    Worker(SyncMap<IN, OUT>& map) : map(map) {}
    void operator ()();
    {    
        //while (true)
        {
            /*
            while(!map.stop && map.input_.empty())
            {
                //wait
            }

            if(map.stop && map.input_.empty())
                return;
            */

            auto data = map.input_.front();
            map.input_.pop();
            
            // real job
            OUT value = map.func(data.first);
            
            // data ready - put in output
            map.output_[data.second] = value;
        }
    }
private:
    SyncMap<IN, OUT>& map;
};


template <typename IN, typename OUT>
class SyncMap
{
    friend class Worker<IN, OUT>;

    std::queue< std::pair<IN, size_t> > input_;
    std::vector< OUT > output_;

    std::function<OUT(IN)> func;
    bool stop;

    Worker<IN, OUT>& worker;

public:
    
    SyncMap() : stop(false), worker( Worker<IN, OUT>(*this) )
    {
        
    }

    std::vector< OUT > operator()(const std::vector<IN>& in, std::function < OUT(IN) >)
    {
        func = f;
        output_.resize(in.size());

        for (size_t i = 0 ; i < in.size() ; ++i)
        {
            input_.push(std::pair<IN, size_t>(in[i], i));

        }

        /*
        stop = true;
        cond_.notify_all();
        for (auto &worker : workers_)
        {
            worker.join();
        }
        stop = false;
        */

        return output_;
    }
    ~SyncMap()
    {
        /*
        stop = true;
        cond_.notify_all();
        for (auto &worker : workers_)
        {
            worker.join();
        }
        stop = false;
        */
    }
};
