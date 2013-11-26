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


boost::barrier br(3);

void foo(int id)
{
    while(1)
    {
        std::cout << "Start " << id << std::endl;

        br.wait();


        std::cout << "After wait "<< id << std::endl;
        boost::this_thread::sleep( boost::posix_time::millisec(450));

    }
}

void f(boost::barrier& b)
{
    //boost::this_thread::sleep( boost::posix_time::millisec(50));
    b.wait();
    //boost::this_thread::sleep( boost::posix_time::millisec(50));
}

void test_2()
{
    boost::barrier b(2);

    boost::thread t(f, boost::ref(b));
    b.wait();

    t.join();
}



int main()
{
    for(int i=0; i<1000; i++)
        test_2();
//    boost::thread thd_1(foo, 1);
//    boost::thread thd_2(foo, 2 );
//    boost::thread thd_3(foo, 3);
//    boost::thread thd_4(foo, 4);

//    thd_1.join();
//    thd_2.join();
//    thd_3.join();
//    thd_4.join();
}
