#include <iostream>
#include <string>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>

#include "ThreadGuard.hpp"

using namespace std;

class Task
{
	string& builder_;
public:
	Task(string& builder) :
		builder_(builder)
	{
	}

	int val;

	void operator()()
	{
		boost::posix_time::seconds work_time(1);
		for (int i = 0; i < 10; ++i)
		{
			cout << "task - iteration #" << i << " : " << builder_ << endl;
			builder_ += boost::lexical_cast<string>(i) + " ";
			boost::this_thread::sleep(work_time);
		}
		cout << "ending task - value of text: " << builder_ << endl;

		val = 10;

	}

};

void do_something_in_current_thread()
{
	boost::posix_time::milliseconds work_time(500);
	for (int i = 0; i < 10; ++i)
	{
		if (i == 5)
			throw std::runtime_error("Runtime error");

		cout << "iteration #" << i << " in do_something_in_current_thread()"
				<< endl;
		boost::this_thread::sleep(work_time);
	}

	cout << "end of do_something_in_current_thread()" << endl;
}

void join_without_raii()
{
	// TODO
	string local("local var: ");


	Task t(local);
	boost::thread thd(t);
    //ThreadGuard guard(thd);


}



class guard
{
	boost::thread& t;

public:

	explicit guard(boost::thread& t_): t(t_)
	{}

	~guard()
	{
		if (t.joinable())
		{
			cout << "guard join" << endl;
			t.join();
		}
	}

	//guard(guard const&)=delete;
	//guard& operator=(guard const&)=delete;
};


class scoped_thread 
{ 
    boost::thread t; 
public: 
    explicit scoped_thread(boost::thread t_):                     
        t(boost::move(t_)) 
    { 
        if(!t.joinable())                                       
            throw std::runtime_error("No thread"); 
    } 
    ~scoped_thread() 
    { 
        t.join();                                               
    } 

    //scoped_thread(scoped_thread const&)=delete; 
    //scoped_thread& operator=(scoped_thread const&)=delete; 
}; 
 

void join_with_raii()
{
	// TODO
	string local = "local var: ";

	Task t(local);
	boost::thread thd(t);
	guard g(thd);

    //scoped_thread thd(boost::thread(t));       

	do_something_in_current_thread();

	thd.join();
}

int main()
{
	try
	{
		join_without_raii();
		//join_with_raii();
	} catch (const std::runtime_error& e)
	{
		cout << "exception caught: " << e.what() << endl;
	}

	boost::this_thread::sleep(boost::posix_time::seconds(10));

	cout << "end of main thread";
}
