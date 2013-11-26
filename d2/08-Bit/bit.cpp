#include <iostream>
#include <boost/thread.hpp>

const long number_of_iterations = 54123;


struct BField
{
  int a:4, b:4; 
};

BField bf;

void concurrent_task_A()
{
	for(long i = 0; i < number_of_iterations; ++i)
	{
		bf.a++;  // wyœcig - race condition		
	}
}

void concurrent_task_B()
{
	for(long i = 0; i < number_of_iterations; ++i)
	{
		bf.b++;  // wyœcig - race condition		
	}
}


int main()
{

	boost::thread_group thd_group;

	thd_group.create_thread(concurrent_task_A);
	thd_group.create_thread(concurrent_task_B);
	
	thd_group.join_all();


	std::cout << bf.a << " : " << bf.b << std::endl;

}
