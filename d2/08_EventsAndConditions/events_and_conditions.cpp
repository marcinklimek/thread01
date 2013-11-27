/*
 * events_and_conditions.cpp
 *
 *  Created on: 2010-04-15
 *      Author: InfoTraining.KP
 */

#include <iostream>
#include <queue>
#include <boost/thread.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

boost::mutex mutex;
std::queue<int> data_queue;
boost::condition_variable data_cond;

boost::mutex cm;


void process_data(int data)
{
	cm.lock();
	std::cout << "Processing data : " << data << std::endl;
	cm.unlock();
	boost::this_thread::sleep(boost::posix_time::milliseconds(1500));
}

void data_preparation_thread()
{
	boost::unique_lock<boost::mutex> lk(mutex, boost::defer_lock);
	for (int i = 0; i < 10; ++i)
	{
		int data = i;
		lk.lock();
		data_queue.push(data);
		data_cond.notify_one();
		lk.unlock();
		cm.lock();
		std::cout << "Pushing a value: " << data << std::endl;
		cm.unlock();
		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	}

	lk.lock();
	data_queue.push(-1);
	data_cond.notify_one();
}

void data_processing_thread()
{
	while(true)
	{
		boost::unique_lock<boost::mutex> lk(mutex);

		while ( data_queue.empty() )
			data_cond.wait(lk );
			
		//data_cond.wait(lk, !boost::bind(&std::queue<int>::empty, boost::ref(data_queue)));
		
		int data = data_queue.front();
		data_queue.pop();
		lk.unlock();

		if (data == -1)
		{
			std::cout << "End of processing..." << std::endl;
			break;
		}

		process_data(data);
	}
}

int main()
{
	boost::thread thd_producer(data_preparation_thread);
	boost::thread thd_consumer(data_processing_thread);

	thd_producer.join();
	thd_consumer.join();
}
