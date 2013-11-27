//
//  Task sink in C++
//  Binds PULL socket to tcp://localhost:5558
//  Collects results from workers via that socket
//
//  Olivier Chamoux <olivier.chamoux@fr.thalesgroup.com>
//
#include <zmq.h>
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include "zhelpers.hpp"


int main (int argc, char *argv[])
{
    int h = 768;
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t receiver(context,ZMQ_PULL);
	receiver.bind("tcp://*:5558");

    //  Wait for start of batch
    zmq::message_t message;
    receiver.recv(&message);

    //  Start our clock now
    struct timeval tstart;
    gettimeofday (&tstart, NULL);

    //  Process 100 confirmations
    int task_nbr;
    int total_msec = 0;     //  Total calculated cost in msecs
    
    
    std::string buf[768];
    for (task_nbr = 0; task_nbr < h; task_nbr++) {

        receiver.recv(&message);
        int workload_y;
        
        
        std::istringstream iss(static_cast<char*>(message.data()) );
        iss >> workload_y;
        
        std::getline(iss,  buf[h-workload_y-1]);
        
        std::cout << "Sink -> Received line: " << workload_y << std::endl; //<< " | "<< buf[h-workload_y-1] << std::endl;

    }
    
    std::cout << "Image" << std::endl;
    
     FILE *f = fopen("image.ppm", "w");         // Write image to PPM file.
     fprintf(f, "P3\n%d %d\n%d\n", 1024, 768, 255);
     
     for (int i=0; i<h; i++)
          fprintf(f, "%s ", buf[i].c_str() );  
//     
     fclose(f);
     
    //  Calculate and report duration of batch
    struct timeval tend, tdiff;
    gettimeofday (&tend, NULL);

    if (tend.tv_usec < tstart.tv_usec) {
        tdiff.tv_sec = tend.tv_sec - tstart.tv_sec - 1;
        tdiff.tv_usec = 1000000 + tend.tv_usec - tstart.tv_usec;
    }
    else {
        tdiff.tv_sec = tend.tv_sec - tstart.tv_sec;
        tdiff.tv_usec = tend.tv_usec - tstart.tv_usec;
    }
    total_msec = tdiff.tv_sec * 1000 + tdiff.tv_usec / 1000;
    std::cout << "\nTotal elapsed time: " << total_msec << " msec\n" << std::endl;
    return 0;
}
