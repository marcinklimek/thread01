#include <iostream>
#include <boost/thread.hpp>

class BackgroundTask
{
	boost::posix_time::milliseconds work_time;
public:
	BackgroundTask(unsigned int interval) : work_time(interval)
	{
	}

	void operator()(const std::string& id, const std::string& data) const
	{
		throw std::runtime_error("Forced exception");

		for(size_t i = 0; i < data.length(); i++)
		{
			std::cout << data[i] << "; id: " << id << std::endl;

			boost::this_thread::sleep(this->work_time);


		}
	}
};


void test(int& v)
{
	v=10;
}

int main()
{
	std::cout << "Starting main..." << std::endl;

	/*
	BackgroundTask bg(1000);  // obiekt funkcyjny

	//int x = 0;
	//boost::shared_ptr<int> y(new int);


	//std::cout << x << std::endl;

	boost::thread thdx(test, boost::ref(x));
	thdx.join();
	std::cout << x << std::endl;
	*/

	boost::thread thd1(BackgroundTask(10), "THD#1", "simple");
	//boost::thread thd2(BackgroundTask(500), "THD#2", "threads");
//	boost::thread thd3(boost::bind(&BackgroundTask::operator(), BackgroundTask(1500), "THD#3", "bind"));

	
	//thd1.join();
	//thd3.join();


	while( true )
	{
		std::cout << "aaaaa" << std::endl;
	}

	thd1.join();

	std::cout << "The end of main." << std::endl;
}
