#pragma once

#include <iostream>
#include "demultiplexer.h"
#include <string>
#include <map>
#include <algorithm>
#include "gmock/gmock.h"
#include "thread_pool.h"

#include "handlers.h"

class Reactor
{
    Demultiplexer& dmx_;
    std::map<IOService::service, std::shared_ptr<EventHandler> > handlers_;
    ThreadPool pool_;

public:
    Reactor(Demultiplexer& dmx) : dmx_(dmx)
    {
    }

    void register_handler(IOService::service serv, std::shared_ptr<EventHandler> handler)
    {
        handlers_[serv] = handler;
    }

    void remove_handler(std::shared_ptr<EventHandler> handler)
    {
        for (auto it = handlers_.begin(); it != handlers_.end(); )
        {
            if(it->second == handler)
                handlers_.erase(it++);
            else
                ++it;
        }
    }

    bool is_handler_registered(std::shared_ptr<EventHandler> handler) const
    {
        return std::count_if(handlers_.begin(), handlers_.end(), [=](const std::pair<IOService::service, std::shared_ptr<EventHandler> >& item) { return item.second == handler; });
    }

    void run()
    {
        while (!dmx_.is_finished())
        {
            auto result = dmx_.get();
            auto handler = handlers_[result.service_type];
            pool_.add_task([=]() { handler->process(result.data); });
        }
    }
};

