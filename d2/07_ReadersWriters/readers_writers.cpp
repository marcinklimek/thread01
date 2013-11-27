/*
 * readers_writers.cpp
 *
 *  Created on: 2010-04-15
 *      Author: InfoTraining.KP
 */

#include <iostream>
#include <boost/thread.hpp>

const size_t size = 20;
int slots[size];
boost::shared_mutex mtx;
boost::mutex cout_mtx;

void init_slots(int slots[], size_t size)
{
	for(size_t i = 0; i < size; ++i)
		slots[i] = i + 1;
}

void wait_miliseconds(int ms)
{
	boost::this_thread::sleep(boost::posix_time::milliseconds(ms));
}

void reader(int id)
{
	wait_miliseconds(500);
	srand(int(time(NULL)));
	for(int i = 0; i < 100; ++i)
	{
		size_t index = std::rand() % size;

		boost::shared_lock<boost::shared_mutex> lock(mtx);
		
		int value = slots[index];
		// processing data

		boost::unique_lock<boost::mutex> cout_lock(cout_mtx);
		std::cout << "Thread#" << id << " reads slot at " << index << " : " << value << std::endl;
		cout_lock.unlock();

		lock.unlock();
		wait_miliseconds(1500*id);
	}
}

void writer(int id)
{
	for(size_t i = 0; i < size; ++i)
	{
		wait_miliseconds(3000);

		// writing data
		boost::lock_guard<boost::shared_mutex> lock(mtx);
		slots[i] = 0;


		boost::unique_lock<boost::mutex> cout_lock(cout_mtx);
		std::cout << "Thread#" << id << " writes slot at " << i << " : " << 0 << std::endl;
		cout_lock.unlock();


		wait_miliseconds(3000);
	}
}

int main()
{
	init_slots(slots, size);

	boost::thread_group thd_group;

	thd_group.create_thread(boost::bind(&reader, 1));
	thd_group.create_thread(boost::bind(&reader, 2));
	thd_group.create_thread(boost::bind(&reader, 3));
	thd_group.create_thread(boost::bind(&writer, 4));

	thd_group.join_all();
}
