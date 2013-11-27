/*
 * barriers.cpp
 *
 *  Created on: 2010-04-17
 *      Author: InfoTraining.KP
 */

#include <iostream>
#include <string>

#include <boost/thread.hpp>


class Randevous
{
private:
	int id_;
	int delay_;
	boost::shared_ptr<boost::barrier> barrier_;
public:
	Randevous(int id, int delay, boost::shared_ptr<boost::barrier> barrier)
		: id_(id), delay_(delay), barrier_(barrier)
	{
	}

	void operator()()
	{
		std::cout << "Thread#" << id_ << " has started" << std::endl;
		boost::this_thread::sleep(boost::posix_time::seconds(delay_));
		std::cout << "Thread#" << id_ << " waits for the others" << std::endl;
		barrier_->wait();
		std::cout << "Thread#" << id_ << " is running." << std::endl;
		boost::this_thread::sleep(boost::posix_time::seconds(1));
		std::cout << "Thread#" << id_ << " finished." << std::endl;
	}
};


int main()
{
	boost::shared_ptr<boost::barrier> barrier(new boost::barrier(3));

	boost::thread_group thd_group;
	thd_group.create_thread(Randevous(1, 1, barrier));
	thd_group.create_thread(Randevous(2, 3, barrier));
	thd_group.create_thread(Randevous(3, 5, barrier));

	thd_group.join_all();

	std::cout << "End of main." << std::endl;
}
