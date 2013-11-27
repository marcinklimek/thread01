#include <iostream>
#include <string>
#include <boost/thread.hpp>

void do_something(int value)
{
	std::cout << value << std::endl;
}

class Task
{
	int& ref_member_;
public:
	Task(int& ref_member) : ref_member_(ref_member)
	{
		std::cout << "Task(" << ref_member_ << ")" << std::endl;
	}

	Task(const Task& t) : ref_member_(t.ref_member_)
	{
		std::cout << "Task(" << ref_member_ << ") - cc" << std::endl;
	}

	~Task()
	{
		std::cout << "~Task(" << ref_member_ << ")" << std::endl;
	}

	void operator()()
	{
		boost::posix_time::seconds work_time(1);
		
		for (unsigned int i = 0; i < 10; ++i)
		{
			do_something(ref_member_);
			boost::this_thread::sleep(work_time);
		}
	}
};

void dangling_reference()
{
	int local_state = 13;
	Task t( local_state );
	boost::thread thd(t);	
    //thd.join();
    //or std::detach - otherwise quit...
}

int main()
{
	int value = 7;
    Task t( value );
    boost::thread thd(t);

    //dangling_reference();
	boost::this_thread::sleep(boost::posix_time::seconds(10));
	std::cout << "The end of main" << std::endl;
}
