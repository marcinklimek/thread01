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
    unique_ptr<thread> thd;            // watek roboczy

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
        thd = unique_ptr<thread>(
                    new thread( [=]{ this->Run(); } ) );
    }

    ~Active()
    {
        Send( [&]{ done = true; } ); ;
        thd->join();
    }

    void Send( Message m ) {
        mq.push(m);
    }

    template <typename F>
    shared_future<F> SendFunction(function<F()> func )
    {
        shared_ptr<packaged_task<F()>> spt(new packaged_task<F()>(func));

        shared_future<F> result = spt->get_future();

        Send( [=] { (*spt)(); } );

        return result;
    }
};

typedef int Data;

class Document {
public:

    void Save( string filename ) {
            cout << "Saving " << filename << endl;
        }

    void Save( string filename ) { a.Send( [=] {
            cout << "Saving " << filename << endl;
        } ); }

    void Print(Data& data ) { a.Send( [=, &data] {
            cout << "Printing " << data << endl;
        } ); }

    shared_future<string> Load()
    {
        shared_future<string> result = a.SendFunction<string>([]() -> string {
                                                                  cout << "Calculating..." << endl;
                                                                  this_thread::sleep_for(chrono::milliseconds(2000));
                                                                  return "Content";
                                                              });

        return result;
    }

private:
    Data somePrivateStateAcrossCalls;
    Active a;
};

class AsyncDb
{
    Active ao;
public:
    void insert(string sql)
    {
        ao.Send([=]() {
            cout << "Inserting..." << endl;
            this_thread::sleep_for(chrono::milliseconds(2000));
            cout << "Inserted " << sql << endl;
        });
    }

    shared_future<string> select(string sql)
    {
        shared_future<string> result = ao.SendFunction<string>([]() -> string {
                                                                   cout << "Select..." << endl;
                                                                   this_thread::sleep_for(chrono::milliseconds(2000));
                                                                   return "ID = 10";
                                                               });

        return result;
    }

};

int main()
{
    Document d;
    d.Save("ActiveObject.txt");
    Data data = 10;
    d.Print(data);
    shared_future<string> result = d.Load();
    cout << "Main thread working..." << endl;
    cout << "Result = " << result.get() << endl;

    //	AsyncDb db;
    //	db.insert("INSERT VALUE");
    //	auto id = db.select("SELECT ID FROM TABLE");

    //	cout << id.get() << endl;

}

