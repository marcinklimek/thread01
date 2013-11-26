/*
 * lazy_init.cpp
 *
 *  Created on: 2010-04-14
 *      Author: InfoTraining.KP
 */

#include <iostream>
#include <boost/thread.hpp>

class ResourceInterface
{
public:
	virtual ~ResourceInterface()
	{}

	virtual void do_something() = 0;
};

class Resource : public ResourceInterface
{
public:
	Resource()
	{
		std::cout << "Resource constructor" << std::endl;
	}

	~Resource()
	{
		std::cout << "Resource destructor" << std::endl;
	}

	void do_something()
	{
		std::cout << "Resource.do_something() implementation" << std::endl;
	}
};

class ProxyResource : public ResourceInterface
{
	boost::shared_ptr<Resource> resource_ptr_;
	boost::once_flag resource_flag_;

	void init_resource()
	{
		boost::this_thread::sleep( boost::posix_time::seconds(1) );

		resource_ptr_.reset(new Resource());
	}

public:
	ProxyResource() : resource_flag_(BOOST_ONCE_INIT)
	{
		std::cout << "ProxyResource constructor" << std::endl;
	}

	~ProxyResource()
	{
		std::cout << "ProxyResource destructor" << std::endl;
	}

	void do_something()
	{
		std::cout << "before call_once" << std::endl;
		
		boost::call_once(resource_flag_, 
			boost::bind(&ProxyResource::init_resource, this));
		
		std::cout << "after call_once" << std::endl;
		resource_ptr_->do_something();
	}
};


void use_resource(ResourceInterface* r)
{
	r->do_something();
	r->do_something();
}

int main()
{
	ProxyResource proxy;

	std::cout << "Using proxy in multiple threads" << std::endl;

	boost::thread thd1(use_resource, &proxy);
	boost::thread thd2(use_resource, &proxy);

	thd1.join();
	thd2.join();
}
