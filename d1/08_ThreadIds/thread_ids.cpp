/*
* thread_ids.cpp
*
*  Created on: 2010-04-12
*      Author: InfoTraining.KP
*/

#include <iostream>
#include <boost/thread.hpp>

struct AtExit
{

	void operator()() const
	{
		std::cout << "atexit" << std::endl;
	};
};

void fnc_atexit()
{
	std::cout << "fnc_atexit" << std::endl;
}

void do_work()
{
	std::cout << boost::this_thread::get_id() << std::endl;

	AtExit atx;
	//boost::this_thread::at_thread_exit(atx);
	boost::this_thread::at_thread_exit(boost::bind( &AtExit::operator(), &atx));

	//boost::this_thread::at_thread_exit(boost::bind( &fnc_atexit ));

	//int v = boost::this_thread::get_id();
}

int main()
{
	do_work();

	boost::thread_group thd_group;
	thd_group.create_thread(do_work);
	thd_group.create_thread(do_work);
	thd_group.create_thread(do_work);

	thd_group.join_all();
}
