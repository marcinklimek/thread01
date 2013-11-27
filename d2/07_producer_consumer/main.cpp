#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>
#include <condition_variable>

std::mutex con_lock;

void lock_print(const std::string& text)
{
    std::lock_guard<std::mutex> lock(con_lock);
    std::cout << text << std::endl;
}

class ProducerConsumer
{
    std::queue<int> queue_;
    std::condition_variable cond_;
    std::mutex qm_;
    std::shared_ptr<std::thread> prod_;
    std::shared_ptr<std::thread> cons_;

public:
    void producer()
    {
        for(int i = 10; i >= 0; --i)
        {
            lock_print("producing " + std::to_string(i));
            //std::cout << "producing " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1300));
            std::unique_lock<std::mutex> lock(qm_);
            queue_.push(i);
            cond_.notify_one();
        }
    }
    void consumer()
    {
        while(true)
        {
            std::unique_lock<std::mutex> lock(qm_);
            while(queue_.empty())
            {
                lock_print("...waiting for element...");
                cond_.wait(lock);
            }
            int message = queue_.front();
            queue_.pop();

            if(!message)
            {
                lock_print("terminating");
                return;
            }

            lock_print("consuming " + std::to_string(message));
        }
    }

    void run()
    {
        prod_ = std::make_shared<std::thread>([=]() { producer(); } );
        cons_ = std::make_shared<std::thread>([=]() { consumer(); } );
    }

    void stop()
    {
        prod_->join();
        cons_->join();
    }
};

int main()
{
    ProducerConsumer pc;
    pc.run();
    pc.stop();
    return;
}
