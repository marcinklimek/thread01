#include <iostream>
#include "demultiplexer.h"
#include "gtest/gtest.h"
#include "reactor.h"

TEST(demultiplexer_test, can_add_event)
{
    Demultiplexer dmx{};
    dmx.__add_event(IOService::FTP, "Test_ftp" );
    ASSERT_EQ(1, dmx.queued_events_count());
}

TEST(demultiplexer_test, can_dequeue_event)
{
    Demultiplexer dmx{};
    std::string expected_data("Test_ftp");
    dmx.__add_event(IOService::FTP, expected_data);
    io_data result = dmx.get();
    ASSERT_EQ(IOService::FTP, result.service_type);
    ASSERT_EQ(expected_data, result.data);
}

// Fixture for reactor
class ReactorTests : public ::testing::Test
{
protected:
    Demultiplexer dmx;
    Reactor rct;

public:
    ReactorTests() : rct(dmx)
    {
    }
};

TEST_F(ReactorTests, can_register_handle)
{
    std::shared_ptr<EventHandler> ftp_handler(new FTPHandler());
    rct.register_handler(IOService::FTP, ftp_handler);
    ASSERT_TRUE(rct.is_handler_registered(ftp_handler));
}

TEST_F(ReactorTests, can_unregister_handle)
{
    std::shared_ptr<EventHandler> ftp_handler(new FTPHandler());
    rct.register_handler(IOService::FTP, ftp_handler);
    ASSERT_TRUE(rct.is_handler_registered(ftp_handler));
    rct.remove_handler( ftp_handler );
    ASSERT_FALSE(rct.is_handler_registered(ftp_handler));
}

TEST_F(ReactorTests, can_invoke_handlers)
{
    std::string expected_data = "Test";
    std::shared_ptr<MockEventHandler> mq_handler(new MockEventHandler);
    EXPECT_CALL(*mq_handler, process(expected_data));
    rct.register_handler(IOService::FTP, mq_handler);
    dmx.__add_event(IOService::FTP, expected_data);
    dmx.__stop();
    rct.run();
}


int main(int argc, char **argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();

    Demultiplexer dmx;
    Reactor rct(dmx);
    std::shared_ptr<EventHandler> handler(new FTPHandler());
    rct.register_handler(IOService::FTP, handler);
    dmx.__add_event(IOService::FTP, "Ala");
    dmx.__add_event(IOService::FTP, "Ola");
    dmx.__add_event(IOService::FTP, "Jola");
    dmx.__add_event(IOService::FTP, "Tola");
    dmx.__add_event(IOService::FTP, "Ala2");
    dmx.__add_event(IOService::FTP, "Ola2");
    dmx.__add_event(IOService::FTP, "Jola2");
    dmx.__add_event(IOService::FTP, "Tola2");
    dmx.__stop();
    rct.run();
}
