
#include <iostream>
#include <boost/thread.hpp>
#include <queue>
#include <vector>
#include <functional>
#include <stdexcept>


template <typename IN, typename OUT>
class AsyncMap;

template <typename IN, typename OUT>
class Worker
{
public:
    Worker(AsyncMap<IN, OUT>& map) : map(map) {}
    void operator ()();
private:
    AsyncMap<IN, OUT>& map;
};


template <typename IN, typename OUT>
class AsyncMap
{
    friend class Worker<IN, OUT>;
    std::vector< boost::thread > workers_;
    std::queue< std::pair<IN, size_t> > input_;
    std::vector< OUT > output_;
    boost::mutex input_m_;
    boost::mutex output_m_;
    boost::condition_variable cond_;
    std::function<OUT(IN)> func;
    bool stop;

public:
    AsyncMap();
    std::vector< OUT > operator()(const std::vector<IN>& in, std::function < OUT(IN) >);
    ~AsyncMap();
};

template <typename IN, typename OUT>
void Worker<IN, OUT>::operator ()()
{    
    while (true)
    {
        boost::unique_lock<boost::mutex> in_lock(map.input_m_);

        while(!map.stop && map.input_.empty())
            map.cond_.wait(in_lock);

        if(map.stop && map.input_.empty())
            return;

        auto data = map.input_.front();
        map.input_.pop();
        in_lock.unlock();
        // real job
        OUT value = map.func(data.first);
        // data ready - put in output
        boost::lock_guard<boost::mutex> out_lock(map.output_m_);
        map.output_[data.second] = value;
    }
}

template <typename IN, typename OUT>
std::vector<OUT> AsyncMap<IN, OUT>::operator()(const std::vector<IN> &in, std::function<OUT (IN)> f)
{
    func = f;
    output_.resize(in.size());
    for (size_t i = 0 ; i < in.size() ; ++i)
    {
        boost::lock_guard<boost::mutex> lock(input_m_);
        input_.push(std::pair<IN, size_t>(in[i], i));
        cond_.notify_one();
    }
    stop = true;
    cond_.notify_all();
    for (auto &worker : workers_)
    {
        worker.join();
    }
    stop = false;
    return output_;
}

template <typename IN, typename OUT>
AsyncMap<IN, OUT>::AsyncMap() : stop(false)
{
    int n_of_threads = boost::thread::hardware_concurrency();
    if (n_of_threads == 0) n_of_threads = 2;
    for (size_t i = 0 ; i < n_of_threads ; ++i)
    {
        workers_.push_back(boost::thread(Worker<IN, OUT>(*this)));
    }
}

template <typename IN, typename OUT>
AsyncMap<IN, OUT>::~AsyncMap()
{
    stop = true;
    cond_.notify_all();
    for (auto &worker : workers_)
    {
        worker.join();
    }
}
