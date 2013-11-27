#include <iostream>
#include <memory>
#include <functional>
#include <thread>
#include <string>
#include <future>
#include "thread_safe_queue.hpp"

using namespace std;


class Active
{
public:
    typedef function<void()> Message;

private:
    Active( const Active& );           // no copying
    void operator=( const Active& );   // no copying

    bool done;                         // flaga, czy konczyc watek roboczy
    
    ThreadSafeQueue<Message> mq;       // synchronizacja kolejka
    
    void Run()
    {
        while( !done ) {
            Message msg;
            if (mq.try_pop(msg))
                msg();            // execute message
        }                         // note: last message sets done to true
    }

public:
    Active() : done(false)
    {
    }

    ~Active()
    {
    }

    void Send( Message m ) 
    {
        mq.push(m);
    }

    template <typename F>
    shared_future<F> SendFunction(function<F()> func )
    {
    }
};

typedef int Data;

class Document 
{

public:

    void Save( string filename ) 
    { 
        cout << "Saving " << filename << endl;
    }

    void Print(Data& data ) 
    { 

        cout << "Printing " << data << endl;
    }

    string Load()
    {
        cout << "Calculating..." << endl;
        this_thread::sleep_for(chrono::milliseconds(2000));
        return "Content";

    }

private:
    
    Data document_data;

};


int main()
{
    Document d;
    d.Save("ActiveObject.txt");
    
    Data data = 10;
    d.Print(data);
    
    string result = d.Load();
    
    cout << "Main thread working..." << endl;
    cout << "Result = " << result.get() << endl;
}

