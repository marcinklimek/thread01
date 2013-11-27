//
//  Task worker in C++
//  Connects PULL socket to tcp://localhost:5557
//  Collects workloads from ventilator via that socket
//  Connects PUSH socket to tcp://localhost:5558
//  Sends results to sink via that socket
//
//  Olivier Chamoux <olivier.chamoux@fr.thalesgroup.com>
//
#include "zhelpers.hpp"
#include <sstream>
#include "smallpt_lib.hpp"

int main (int argc, char *argv[])
{
    zmq::context_t context(1);

    //  Socket to receive messages on -> producer
    zmq::socket_t receiver(context, ZMQ_PULL);
    receiver.connect("tcp://localhost:5557");

    //  Socket to send messages to -> sink
    zmq::socket_t sender(context, ZMQ_PUSH);
    sender.connect("tcp://localhost:5558");    
    
    //  Process tasks forever
    while (1) 
    {

        zmq::message_t message;
        int line_number;           //  Workload in msecs

        receiver.recv(&message);
          
        std::istringstream iss(static_cast<char*>(message.data()));
        iss >> line_number;
                
        //  Do the work
        std::string line = scan_line(line_number);
        std::ostringstream output; 
        output << "head: ";
        output << line_number << " :: "; 
        output << line;
        std::string text = output.str();
        zmq::message_t return_message(text.size () + 1);
        memcpy (return_message.data (), text.c_str (), text.size () + 1);

        //  Send results to sink

        //message.rebuild();
        sender.send(return_message);

        //  Simple progress indicator for the viewer
		std::cout << "." << std::flush;
    }
    return 0;
}
