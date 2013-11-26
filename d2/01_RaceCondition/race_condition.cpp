#include <iostream>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

const long number_of_iterations = 100;

long shared_counter = 0;

void concurrent_task()
{
	
	for(long i = 0; i < number_of_iterations; ++i)
	{

			shared_counter++;
	}
}

int main()
{
	const int number_of_threads = 4;

	for( int j=0; j<10; ++j)
	{
		boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();

		boost::thread_group thd_group;
		shared_counter = 0;

		for(int i = 0; i < number_of_threads; ++i)
			thd_group.create_thread(concurrent_task);

		thd_group.join_all();

		std::cout << shared_counter << " == " << number_of_threads * number_of_iterations << " : ";
		std::cout << (shared_counter == number_of_threads*number_of_iterations ? "OK" : "WRONG") << std::endl;


		boost::posix_time::ptime end = boost::posix_time::microsec_clock::local_time();

		std::cout << "Total time: " << end - start << std::endl;
	}
}
