//
//  Task ventilator in C++
//  Binds PUSH socket to tcp://localhost:5557
//  Sends batch of tasks to workers via that socket
//
//  Olivier Chamoux <olivier.chamoux@fr.thalesgroup.com>
//
// Here is the ventilator. It generates 100 tasks,
// each one is a message telling the worker to sleep
// for some number of milliseconds:

#include <zmq.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

#define within(num) (int) ((float) num * random () / (RAND_MAX + 1.0))

int main (int argc, char *argv[])
{
    zmq::context_t context (1);

    //  Socket to send messages on
    zmq::socket_t  sender(context, ZMQ_PUSH);
    sender.bind("tcp://*:5557");

    std::cout << "Press Enter when the workers are ready: " << std::endl;
    getchar ();
    std::cout << "Sending tasks to workers...\n" << std::endl;

    //  The first message is "0" and signals start of batch
    zmq::socket_t sink(context, ZMQ_PUSH);
    sink.connect("tcp://localhost:5558");
    zmq::message_t message(2);
    memcpy(message.data(), "0", 1);
    sink.send(message);

    //  Send "height" tasks
    
    int line_nbr;
    int total_msec = 0;     //  Total expected cost in msecs
    for (line_nbr = 0; line_nbr < 768; line_nbr++) 
    {
        message.rebuild(10);
        sprintf ((char *) message.data(), "%d", line_nbr);
        sender.send(message);
        std::cout << "Producer -> Line: " << (char*)message.data() << std::endl;
    }
   
    sleep (1);              //  Give 0MQ time to deliver

    return 0;
}
