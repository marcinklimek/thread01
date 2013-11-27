/*
 * deadlock.cpp
 *
 *  Created on: 2010-04-12
 *      Author: InfoTraining.KP
 */

#include <iostream>
#include <boost/thread.hpp>

class BankAccount
{
private:
	const int id_;
	double balance_;
	mutable boost::mutex mutex_;
public:
	BankAccount(int id, double balance) : id_(id), balance_(balance)
	{
	}

	void transfer(BankAccount& to, double amount)
	{
        //boost::unique_lock<boost::mutex> guard_from(this->mutex_, boost::defer_lock);
        //boost::unique_lock<boost::mutex> guard_to(to.mutex_, boost::defer_lock);
        //boost::lock(guard_from, guard_to);
		
        boost::unique_lock<boost::mutex> guard_from(this->mutex_);
        //boost::thread::yield();
        boost::unique_lock<boost::mutex> guard_to(to.mutex_);

		this->balance_ -= amount;
		to.balance_ += amount;
	}

	void print() const
	{
		std::cout << "BankAccount: id = " << id_;

		boost::lock_guard<boost::mutex> guard(mutex_);
		std::cout << " balance = " << balance_ << std::endl;
	}
};

void deadlock_test(int thread_id, BankAccount* from, BankAccount* to)
{
	for(int i = 0; i < 10000; ++i)
	{
		from->transfer(*to, 1.0);
		std::cout << "Transfer#" << i << " from Thread#" << thread_id << std::endl;
	}
}

int main()
{
	BankAccount ba1(1, 10000.0);
	BankAccount ba2(2, 20000.0);

	boost::thread transfer1(deadlock_test, 1, &ba1, &ba2);
	boost::thread transfer2(deadlock_test, 2, &ba2, &ba1);

	std::cout << "START";

	transfer1.join();
	transfer2.join();

	std::cout << "\nAfter transfers:\n";
	ba1.print();
	ba2.print();

	std::cout << "End of main." << std::endl;
}
