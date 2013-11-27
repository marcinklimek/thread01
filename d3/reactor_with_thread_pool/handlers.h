#ifndef HANDLERS_H
#define HANDLERS_H

class EventHandler
{
public:
    virtual void process(std::string data) = 0;
    virtual ~EventHandler() {};
};

class MockEventHandler : public EventHandler
{
public:
    MOCK_METHOD1(process, void (std::string data));

};

class FTPHandler : public EventHandler
{
public:
    virtual void process(std::string data)
    {
        std::cout << "FTP RUN: " << data << std::endl;
    }
};

#endif // HANDLERS_H
