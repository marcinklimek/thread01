#include <iostream>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <cassert>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

typedef boost::shared_ptr<boost::thread> SharedPtrThread;

#define foreach BOOST_FOREACH


template<typename Iterator,typename T> 
class accumulate_block 
{ 
public:
    void operator()(Iterator first,Iterator last, T& result) 
    { 
        result = std::accumulate(first,last,result); 
    } 
}; 
 

template <typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) 
{
	unsigned long const length=std::distance(first,last); 

	if(!length)                                                       
		return init; 

	unsigned long const min_per_thread=25; 
	unsigned long const max_threads=(length+min_per_thread-1)/min_per_thread;                     

	unsigned long const hardware_threads= boost::thread::hardware_concurrency(); 

	unsigned long const num_threads=                                  
		std::min(hardware_threads!=0?hardware_threads:2,max_threads); 

	unsigned long const block_size=length/num_threads;                

	std::vector<T> results(num_threads); 
	std::vector<boost::thread>  threads(num_threads-1);

	Iterator block_start = first;
	for(unsigned long i=0;i<(num_threads-1);++i)
	{ 
		Iterator block_end=block_start; 
		std::advance(block_end,block_size);                           
		threads[i]=boost::thread(                                      
			accumulate_block<Iterator,T>(), 
			block_start,block_end,boost::ref(results[i])); 

		block_start=block_end;                                       
	} 
	
	accumulate_block<Iterator, T>()(block_start,last,results[num_threads-1]);     

	for(unsigned long i=0;i<(num_threads-1);++i)
		threads[i].join();

	return std::accumulate(results.begin(),results.end(),init);
}

int main()
{
	cout << "hardware_concurrency() = " << boost::thread::hardware_concurrency() << endl;

	const size_t size = 100;
	int numbers[size];

	long result = 0;
	for(size_t i = 0; i < size; ++i)
	{
		numbers[i] = i;
		result += i;
	}

	//-------

	vector<int> vec_numbers;
	for(size_t i=0; i<size; ++i)
		vec_numbers.push_back(i);
	
	int acc_result;
	accumulate_block<vector<int>::const_iterator, int>()(vec_numbers.begin(), vec_numbers.end(), acc_result);

	//-------

	long parallel_result = 0;
	parallel_result = parallel_accumulate(vec_numbers.begin(), vec_numbers.end(), parallel_result);


	cout << "result = " << result << endl;
	cout << "parallelel_result = " << parallel_result << endl;

	system("PAUSE");
}
