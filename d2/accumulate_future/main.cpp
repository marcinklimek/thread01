#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <chrono>
#include <future>

template <typename It, typename T>
T parallel_accumulate(It first, It last, T init)
{
    // parallel version of accumulate
    size_t len = std::distance(first, last);
    if (!len) return init;

    unsigned int hw_thd = std::thread::hardware_concurrency();
    hw_thd = 2;
    size_t block_size = len/hw_thd;

    std::vector<std::future<T>> results;

    It block_start = first;

    for (size_t i = 0 ; i < hw_thd-1 ; ++i)
    {
        It block_end = block_start;
        std::advance(block_end, block_size);
        results.push_back( std::async( std::launch::async,
                                       std::accumulate<It, T>,
                                       block_start, block_end, T() ));
        block_start = block_end;
    }
    results.push_back(std::async( std::launch::async,
                                  std::accumulate<It, T>,
                                  block_start, last, T() ));

    T result = init;
    for (auto &res : results)
    {
        result += res.get();
    }
    return result;
}

int main(int argc, char *argv[])
{
    std::cout << "Hardware threads: "  << std::thread::hardware_concurrency() << std::endl;
    const size_t SIZE = 500000000;
    std::vector<long> v(SIZE);
    for (size_t i = 0 ; i < SIZE; ++i)
    {
        v[i] = i;
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "Accumulate:          " << std::accumulate(v.begin(), v.end(), 0);
    auto end = std::chrono::high_resolution_clock::now();
    float seconds = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout << "   " << seconds << " msec" << std::endl;

    std::vector<long> v2(SIZE);
    for (size_t i = 0 ; i < SIZE; ++i)
    {
        v2[i] = i;
    }


    start = std::chrono::high_resolution_clock::now();
    std::cout << "Parallel Accumulate: " << parallel_accumulate(v2.begin(), v2.end(), 0);
    end = std::chrono::high_resolution_clock::now();
    seconds = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout << "   " << seconds << " msec" << std::endl;
    return 0;
}
