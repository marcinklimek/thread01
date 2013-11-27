#include <iostream>

#include "threadpool.h"

void newTask(int i)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "newTask " << i << std::endl;
}

int Question()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 42;
}

int main()
{
    ThreadPool tp;

    tp.add_task( []() { std::cout << "Task1" << std::endl;} );
    tp.add_task( []() { std::cout << "Task2" << std::endl;} );
    tp.add_task( []() { std::cout << "Task3" << std::endl;} );

    tp.add_task( []() { newTask(1); } );
    tp.add_task( []() { newTask(2); } );
    tp.add_task( []() { newTask(3); } );

    std::future<int> res = tp.add_future_task<int>(Question);

    std::cout << res.get() << std::endl;

}
