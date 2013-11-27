#include <iostream>
#include <string>
#include <boost/thread.hpp>
#include <boost/assign.hpp>

boost::thread_specific_ptr<std::string> ptr_text;
boost::thread_specific_ptr<int> ptr_counter;


void concatenate(const std::string& word)
{
	if (ptr_text.get() == 0)
		ptr_text.reset(new std::string());

	if (ptr_counter.get() == 0)
		ptr_counter.reset(new int(0));

	(*ptr_text) += word;
	(*ptr_counter)++;
}

void join_words(int id, std::vector<std::string> words)
{
	std::for_each(words.begin(), words.end(), concatenate);

	std::cout << "Thread#" << id << " called concatenate() " << (*ptr_counter) << " times" << std::endl;
	std::cout << "Whole sentence: " << (*ptr_text) << std::endl;
}

int main()
{
	using namespace boost::assign;

	std::vector<std::string> words1;
	words1 +=  "This", "is", "test", "of", "thread_specific_ptr", ".";

	std::vector<std::string> words2;
	words2 += "Hello", "concurrent", "world", "!";

	boost::thread thd1(join_words, 1, words1);
	boost::thread thd2(join_words, 2, words2);

	thd1.join();
	thd2.join();
}
