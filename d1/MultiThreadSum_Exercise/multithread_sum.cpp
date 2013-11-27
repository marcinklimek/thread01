#include <iostream>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

const unsigned long max =  10000000;

void one_thread_sum()
{
	boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();

	unsigned long sum = 0;

	for(unsigned long i = 0; i < max; ++i)
		sum += i;

	boost::posix_time::ptime end = boost::posix_time::microsec_clock::local_time();

	std::cout << "Total time: " << end - start << std::endl;
	std::cout << "Sum: " << sum << std::endl;
}

// napisaæ wielow¹tkow¹ wersjê one_thread_sum()


class worker
{
public:
	worker() {}

	void operator()(unsigned long start, unsigned long end, unsigned long& result)
	{
		result = 0;

		for (unsigned long i=start; i < end; ++i) 
			result += i;
	};
};

void multithread_sum()
{
	unsigned long sum = 0;

	unsigned int num_threads = boost::thread::hardware_concurrency();
	unsigned int block_size = max/num_threads;
	unsigned int block_start = 0;
	unsigned int block_end;

	std::vector<unsigned long> results(num_threads+1); 

	boost::thread_group thd_group;

	std::cout << num_threads << "  " << block_size << std::endl;

	boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();
	
	unsigned int i;
	for ( i=0; i<num_threads; ++i) 
	{
		block_end = block_start;
		block_end += block_size;
		
		thd_group.create_thread(
			boost::bind(
			&worker::operator(), worker(), block_start, block_end, std::ref(results[i])));

		block_start = block_end;
	}
	
	if ( block_end < max )
	{
		worker()(block_start, max, std::ref(results[i]));
		i++;
	}

	thd_group.join_all();
	
	for (unsigned int j=0; j<(num_threads+1); ++j) 
		sum += results[j];

	boost::posix_time::ptime end = boost::posix_time::microsec_clock::local_time();

	std::cout << "Total time: " << end - start << std::endl;
	std::cout << "Sum: " << sum << std::endl;
}

int main()
{
	std::cout << max << std::endl;
	

	one_thread_sum();

	std::cout << "\n-----------------------------\n" << std::endl;

	multithread_sum();	

	std::cout << "\n-----------------------------\n" << std::endl;
}