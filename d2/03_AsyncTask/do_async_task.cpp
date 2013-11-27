#include <iostream>
#include <string>
#include <boost/thread.hpp>

template <typename Result, typename Func>
boost::unique_future<Result> async_task(Func task)
{
	boost::packaged_task<Result> pt(task);
	boost::unique_future<Result> future = pt.get_future();

	boost::thread thd(std::move(pt));
	thd.detach();

	return static_cast<boost::unique_future<Result> >(std::move(future));
}

int task1()
{
	std::cout << "Starting task1..." << std::endl;
	boost::this_thread::sleep(boost::posix_time::seconds(2));

	return 44;
}

class Task2
{
public:
	std::string operator()()
	{
		std::cout << "Starting task2..." << std::endl;
		boost::this_thread::sleep(boost::posix_time::seconds(4));

		return std::string("Bite my shiny metal ass...");
	}
};

int main()
{
	boost::unique_future<int> ft1(async_task<int>(&task1));
	boost::unique_future<std::string> ft2(async_task<std::string>(Task2()));

	std::cout << "ft1 gets: " << ft1.get() << std::endl;
	std::cout << "ft2 gets: " << ft2.get() << std::endl;
}
