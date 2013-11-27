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

boost::thread create_thread(const std::string& str)
{
	static int counter = 1;
	Worker worker(++counter);
	boost::thread thd(worker,"worker");

	return boost::thread(boost::move(thd)); // propozycja naprawy, https://svn.boost.org/trac/boost/ticket/4367

	//nie dziala pod gcc 4.4.6, m.in. ticket https://svn.boost.org/trac/boost/ticket/2380
	//return boost::move(thd);
}

int main()
{
	std::cout << "Starting a main thread" << std::endl;
	boost::thread threads[10];

	for(int i = 0; i < 10; ++i)
		threads[i] = boost::move(create_thread("thread"));

	for(int i = 0; i < 10; ++i)
		if (threads[i].joinable())
			threads[i].join();

	std::cout << "Ending of main thread." << std::endl;
}
