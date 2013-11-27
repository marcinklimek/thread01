/*
 * transferring_locks.cpp
 *
 *  Created on: 2010-04-13
 *      Author: InfoTraining.KP
 */

#include <iostream>
#include <boost/thread.hpp>

boost::unique_lock<boost::mutex> acquire_lock()
{
	static boost::mutex m;
	return boost::unique_lock<boost::mutex> (m);
}

void worker(int id)
{
	std::cout << "Starting a thread#" << id << std::endl;

	boost::unique_lock<boost::mutex> lock(acquire_lock());
	std::cout << "Thread#" << id << " acquired a lock..." << std::endl;

	boost::this_thread::sleep(boost::posix_time::seconds(5));

	std::cout << "Finishing a thread#" << id << std::endl;
}

int main()
{
	boost::thread t1(worker, 1);
	boost::thread t2(worker, 2);

	t1.join();
	t2.join();

	std::cout << "End of main..." << std::endl;
}
