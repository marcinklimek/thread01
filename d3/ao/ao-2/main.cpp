#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <fstream>
#include <algorithm>

using std::shared_ptr;
using std::string;
using std::vector;

template< typename T >
class Queue 
{
    private:
        mutable std::mutex mutex_;
        std::queue< T > queue_;
        std::condition_variable condition_;
        
    public:
        void push(T value)
        {
            std::lock_guard<std::mutex> lk(mutex_);
            queue_.push(value);
            condition_.notify_one();
        }
        
        void pop(T& value)
        {
            std::unique_lock<std::mutex> lk(mutex_);
            condition_.wait(lk, [&](){ return !this->queue_.empty(); });
            value = queue_.front();
            queue_.pop();
        }
};

class ActiveObject 
{
    public:
        class Message
        {
            public:
                virtual ~Message() {}
                virtual bool execute() {  return false; }
        };

        explicit ActiveObject()
        {
            thread_ = shared_ptr<std::thread>( new std::thread(std::bind(&ActiveObject::run, this)) );
        }

        ~ActiveObject()
        {
            send( shared_ptr< Message >(new Message) );
            thread_->join();
        }
        
        ActiveObject(const ActiveObject& obj) = delete;
        ActiveObject& operator=(const ActiveObject& rhs) = delete;      

        void send( shared_ptr< Message > msg )
        {
            std::cout << "send " << std::endl;
            mq_.push(msg);
        }

    private:
        Queue< shared_ptr< Message > > mq_;
        shared_ptr<std::thread> thread_;

        void run()
        {
            shared_ptr< Message > msg;
            while(true)
            {
                mq_.pop(msg);
                if (!msg->execute())
                     break;
            }
        }
};

class Backgrounder
{
    private:
        class MessagePrint : public ActiveObject::Message
        {
            Backgrounder* this_;
            const vector<double>& data_;
            public:
                MessagePrint(Backgrounder* b, const vector<double>& d) : this_(b), data_(d) {}
                bool execute()
                {
                    std::cout << "print " << std::endl;
                    
                    std::ostream_iterator<double> it_out(std::cout, " ");
                    std::copy(data_.begin(), data_.end(), it_out);
                    std::cout << std::endl;
                    return true;
                }
        };
        class MessageSave : public ActiveObject::Message
        {
            Backgrounder* this_;
            string filename_;
            const vector<double>& data_;
            public:
                MessageSave(Backgrounder* b, const string& f, const vector<double>& d) : this_(b), filename_(f), data_(d) {}
                bool execute()
                {
                    std::cout << "save " << std::endl;
                    std::ofstream out(filename_.c_str(), std::ios::out);  
                    std::ostream_iterator<double> it_out(out, " ");
                    std::copy(data_.begin(), data_.end(), it_out);
                    out.close();
                    return true;
                }
        };

        ActiveObject active_;

    public:
        void save(const string& filename, const vector<double>& data)
        {
            shared_ptr< MessageSave > msgSave(new MessageSave(this, filename, data));
            active_.send(msgSave);
        }
        void print(const vector<double>& data)
        {
            shared_ptr< MessagePrint > msgPrint(new MessagePrint(this, data));
            active_.send(msgPrint);
        }
        void done()
        {
            shared_ptr< ActiveObject::Message > msgDone(new ActiveObject::Message);
            active_.send(msgDone);
        }
};

int main()
{
    double data[10] = { 1,2,3,4,5,6,7,8,9,10 };
    std::vector<double> coll(data, data+10);

    Backgrounder bwork;
    
    bwork.save("test.txt", coll);
    bwork.print(coll);    
    //bwork.done(); 
    bwork.print(coll);

    std::cout << "main thread!" << std::endl;

    return 0;
}
