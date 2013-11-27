#include <iostream>
#include <string>
#include <boost/thread.hpp>

class Worker
{
	int id_;
public:
	Worker(int id) : id_(id)
	{
	}

	void operator()(const std::string& str)
	{
		std::cout << "Starting a thread#" << id_ << std::endl;
		std::string::const_iterator it = str.begin();
		std::string::const_iterator end = str.end();
		for(; it != end; ++it)
		{
			std::cout << "Thread#" << id_ << " processing: " << *it << std::endl;
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}
		std::cout << "Ending a thread#" << id_ << std::endl;
	}
};

int main()
{
	std::cout << "Starting a main thread." << std::endl;

	boost::thread_group thd_group;

	thd_group.create_thread(boost::bind(&Worker::operator(), Worker(1), "hello"));

	thd_group.add_thread(new boost::thread(Worker(3), "concurrent"));
	thd_group.create_thread(boost::bind(&Worker::operator(), Worker(2), "world"));

	thd_group.join_all();

	std::cout << "End of main thread." << std::endl;
}
