/*
 * unique_locking.cpp
 *
 *  Created on: 2010-04-13
 *      Author: InfoTraining.KP
 */

#include <iostream>
#include <boost/thread.hpp>

void sleep_for_sec(int seconds)
{
	boost::this_thread::sleep(boost::posix_time::seconds(seconds));
}

boost::timed_mutex mutex;

void worker(int id, int time_interval)
{
	
	boost::unique_lock<boost::timed_mutex> lock(mutex, boost::try_to_lock);

	if ( !lock.owns_lock() )
	{
		int count = 0;
		do
		{
			lock.lock();
			std::cout << "Thread#" << id << " doesn't own a lock... Tries to acquire a mutex for a "
					  << ++count << ". time..." << std::endl;
		} while( !lock.timed_lock(boost::posix_time::seconds(1)));
	}

	std::cout << "Thread#" << id << " owns a lock. Processing data takes time..." << std::endl;
	sleep_for_sec(time_interval);

	std::cout << "Thread#" << id << " releases a lock... " << std::endl;
}

int main()
{
	boost::thread t1(worker, 1, 15);
	boost::thread t2(worker, 2, 5);

	t1.join();
	t2.join();

	std::cout << "End of main..." << std::endl;
}

