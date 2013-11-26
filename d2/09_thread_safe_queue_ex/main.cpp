#include <iostream>
#include <vector>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "thread_safe_queue.hpp"
#include "boost/thread.hpp"


TEST(ts_queue_test_single, int_two_elements)
{
    ThreadSafeQueue<int> q;

    for (int i = 0; i < 2 ; ++i)
        q.push(i);

    for (int i = 0; i < 2 ; ++i)
        q.pop(i);

    ASSERT_TRUE(q.is_empty());
}


#define NUM_ITERATIONS 200000
void producer(ThreadSafeQueue<int>& queue)
{
    for(int i = 0; i < NUM_ITERATIONS; ++i)
    {
        queue.push(i);
    }

    queue.push(-1);
}

void consumer(int id, ThreadSafeQueue<int>& queue, int& num_iterations)
{
    int value;
    num_iterations = 0;
    
    while(true)
    {
        queue.pop(value);

        if (value == -1)
        {
            break;
        }
        else
            num_iterations++;
    }
}

TEST(ts_queue_test_threaded, valid_number_of_iterations)
{
    ThreadSafeQueue<int> q;

    int num_iter;
    boost::thread t1( &producer, boost::ref(q)  );
    boost::thread t2( &consumer, 1, boost::ref(q), boost::ref(num_iter) );

    t1.join();
    t2.join();

    ASSERT_EQ(NUM_ITERATIONS, num_iter);
}



TEST(ts_queue_test_threaded, is_empty)
{
    ThreadSafeQueue<int> q;

    int num_iter;
    boost::thread t1( &producer, boost::ref(q)  );
    boost::thread t2( &consumer, 1, boost::ref(q), boost::ref(num_iter) );

    t1.join();
    t2.join();

    ASSERT_TRUE(q.is_empty());
}


TEST(ts_queue_test_threaded, try_pop)
{
    ThreadSafeQueue<int> q;

    int num_iter;
    boost::thread t1( &producer, boost::ref(q)  );
    boost::thread t2( &consumer, 1, boost::ref(q), boost::ref(num_iter) );

    t1.join();
    t2.join();

    int v;
    ASSERT_FALSE( q.try_pop(v) );
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
