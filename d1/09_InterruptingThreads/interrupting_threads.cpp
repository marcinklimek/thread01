#include <iostream>
#include <vector>
#include <boost/assign.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/thread.hpp>

using namespace boost::assign;

template <typename T>
void background_copy(const std::vector<T>& from, std::vector<T>& to)
{
	to.clear();

	typename std::vector<T>::const_iterator it = from.begin();
	typename std::vector<T>::const_iterator end = from.end();
	try
	{
		for(; it != end; ++it)
		{
			{
				boost::this_thread::disable_interruption di;
				std::cout << "try copy of " << *it << std::endl;
				boost::this_thread::sleep(boost::posix_time::millisec(150));
			}
		
			boost::this_thread::interruption_point();

			{
				boost::this_thread::disable_interruption di;
				to.push_back(*it);
				boost::this_thread::sleep(boost::posix_time::millisec(50));
			}
		}
	}
	catch(const boost::thread_interrupted& e)
	{
		std::cout << "copy has been interrupted..."<< std::endl;
	}
}

int main()
{
	std::vector<int> vec1;
	vec1 += 1, 2, 3, 4, 5, 6, 7, 8, 9, 10;

	std::vector<int> vec2;

	boost::thread thd(&background_copy<int>, boost::cref(vec1), boost::ref(vec2));

	std::cout << "Main thread works..." << std::endl;

	boost::this_thread::sleep(boost::posix_time::seconds(1));

	thd.interrupt();

	thd.join();

	std::cout << "vec2: ";

	std::for_each(vec2.begin(), vec2.end(), [](int x){std::cout << x << " ";});

	//std::for_each(vec2.begin(), vec2.end(), (std::cout << boost::lambda::_1 << " "));

	std::cout << std::endl;
	std::cin.get();
}