#include <thread>
#pragma once

#include <string>
#include "ts_queue.h"
//#include <queue>

namespace IOService
{
    enum service
    {
        FTP,
        HTTP
    };
}

struct io_data
{
    io_data() {}
    io_data(IOService::service serv, const std::string& data)
        : service_type(serv), data(data) {}
    IOService::service service_type;
    std::string data;
};

class Demultiplexer
{
    ThreadSafeQueue<io_data> queue_;
    //std::queue<io_data> queue_;
    bool finished_;

public:
    Demultiplexer() : finished_(false)
    {

    }

    void __add_event(IOService::service service, const std::string& input)
    {
        queue_.push(io_data(service, input));
    }

    size_t queued_events_count() const
    {
        queue_.size();
    }

    io_data get()
    {
        io_data result;
        queue_.wait_and_pop(result);
        return result;
    }

    bool is_finished() const
    {
        return (finished_ && (!queued_events_count()));
    }

    void __stop()
    {
        finished_ = true;
    }
};


