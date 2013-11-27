#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/thread/future.hpp>
#include <boost/bind.hpp>
#include <string>

#include <iostream>

class Message
{
    std::string message_;
public:
    explicit Message(const std::string& name) : message_(name) {}
    std::string report() const
    {
        return message_;
    }
    
};

class ActiveObject : boost::noncopyable
{
    
    int wait;
public:
    ActiveObject() : wait(0)
    {
    }
    
    virtual ~ActiveObject()
    {
        std::cout << "destroying active object" << std::endl;
    }
  
    boost::shared_future<Message> getMessage(const std::string& name)
    {
        wait++;
        
        boost::packaged_task<Message> task(boost::bind(&ActiveObject::concreteMethod, this, name, wait));
        boost::shared_future<Message> res(task.get_future());
        
        boost::thread(boost::move(task));
        
        return res;
    }
    
private:
    
    Message concreteMethod(const std::string& name, int waitTime)
    {
        std::cout << "START someMesssage "  << name << " " << waitTime << std::endl;
        boost::this_thread::sleep( boost::posix_time::seconds(waitTime) );
        
        std::cout << "END someMesssage "  << name << std::endl;
        
        return Message(name);
    }
};

int main()
{
    ActiveObject ao;
    
    std::string id = "abc";
    std::string id2 = "def";
    
    boost::shared_future<Message> fmessage = ao.getMessage(id);
    boost::shared_future<Message> fmessage2 = ao.getMessage(id2);

    while ( !fmessage.is_ready() )
    {
        std::cout << "+";
        boost::this_thread::sleep( boost::posix_time::millisec(500) );
        
    }
    std::cout << std::endl;
    
    Message message = fmessage.get();
    std::cout << message.report() << std::endl;
    
    std::cout << "+" << std::endl;;
    
    message = fmessage2.get();
    std::cout << message.report() << std::endl;
    
    return 0;
}