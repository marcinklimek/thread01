#include <iostream>
#include <string>
#include <boost/thread.hpp>

std::string asynchronous_fn()
{
	boost::this_thread::sleep(boost::posix_time::seconds(5));

	return std::string("Eureka!");
}

std::string asynchronous_with_exception()
{
	boost::this_thread::sleep(boost::posix_time::seconds(5));

	throw std::runtime_error("Runtime error during async call.");

	return std::string("Eureka!");
}

int main()
{
	std::cout << "Asynchronous task launched..." << std::endl;

	boost::packaged_task<std::string> pt(asynchronous_fn);
	boost::unique_future<std::string> future = pt.get_future();

	boost::thread task1(std::move(pt));

	while (!future.is_ready())
	{
		std::cout << "Task not finished... Main thread works..." << std::endl;
		boost::this_thread::sleep(boost::posix_time::milliseconds(500));
	}

	std::cout << "future.has_value() - " << future.has_value() << std::endl;
	std::cout << "future.has_exception() - " << future.has_exception() << std::endl;
	std::cout << "future.get() - " << future.get() << std::endl;

	assert(future.get_state() == boost::future_state::ready);


	std::cout << "\n\nAsynchronous task with exception launched..." << std::endl;
	boost::packaged_task<std::string> pte(asynchronous_with_exception);
	boost::unique_future<std::string> future_with_excpt = pte.get_future();

	boost::thread task2(std::move(pte));

	std::cout << "Main thread waits for completing a task..." << std::endl;

    future_with_excpt.wait();  // czekanie na zakonczenie asynchronicznego zadania

	std::cout << "future_with_excpt.has_value() - " << future_with_excpt.has_value() << std::endl;
	std::cout << "future_with_excpt.has_exception() - " << future_with_excpt.has_exception() << std::endl;

	try
	{
		std::cout << "future_with_excpt.get() - " << future_with_excpt.get() << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}
