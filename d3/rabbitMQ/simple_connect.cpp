#include <SimpleAmqpClient/SimpleAmqpClient.h>

#include <iostream>
#include <stdlib.h>
#include <boost/lexical_cast.hpp>

using namespace AmqpClient;

int direct()
{
    const std::string queue_name = "nsn-queue-direct";
    const std::string exchange_name = "nsn-exchange-direct";
    const std::string consumer_tag = "nsn-consumertag";
    const std::string routing = "nsn-routing-key";
    
    Channel::ptr_t channel;
    channel = Channel::Create();

    // przygotowanie
    channel->DeclareExchange(exchange_name, Channel::EXCHANGE_TYPE_DIRECT, false, false, false);
    
    channel->DeclareQueue(queue_name, false, false, false, false);
    channel->BindQueue(queue_name, exchange_name, routing);

    
    // wysylanie
    BasicMessage::ptr_t msg_in = BasicMessage::Create();
    msg_in->Body("This is a small message.");
    channel->BasicPublish(exchange_name, routing, msg_in);
    channel->BasicPublish(exchange_name, routing, BasicMessage::Create("ph'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn") );

    channel->BasicPublish(exchange_name, routing,
             BasicMessage::Create(boost::lexical_cast<std::string>(42) + " - message" ) );

    // odbior
    channel->DeclareQueue(queue_name, false, false, false, false);
    channel->BindQueue(queue_name, exchange_name, routing);
    channel->BasicConsume(queue_name, consumer_tag);
    
    BasicMessage::ptr_t msg_out = channel->BasicConsumeMessage(consumer_tag)->Message();
    std::cout << "Message text: " << msg_out->Body() << std::endl;

    Envelope::ptr_t env;
    while( true )
    {
        
        if ( channel->BasicConsumeMessage(consumer_tag, env, 500) )
        {
            std::cout << "Envelope received: \n"
            << " Exchange: " << env->Exchange()
            << "\n Routing key: " << env->RoutingKey()
            << "\n Consumer tag: " << env->ConsumerTag()
            << "\n Delivery tag: " << env->DeliveryTag()
            << "\n Redelivered: " << env->Redelivered()
            << "\n Body: " << env->Message()->Body() << std::endl;
        }
        else
        {
            std::cout << "empty\n";

            break;
        }
    }
}

int fanout()
{
    const std::string queue_name = "nsn-queue-fanout";
    const std::string exchange_name = "nsn-exchange-fanout";
    const std::string consumer_tag = "nsn-consumertag";
    const std::string routing = "nsn-routing-key";
    
    Channel::ptr_t channel;
    channel = Channel::Create();
    
    // przygotowanie
    channel->DeclareExchange(exchange_name, Channel::EXCHANGE_TYPE_FANOUT, false, false, false);
    
    channel->DeclareQueue(queue_name, false, false, false, false);
    channel->BindQueue(queue_name, exchange_name, routing);
    
    
    // wysylanie
    BasicMessage::ptr_t msg_in = BasicMessage::Create();
    msg_in->Body("This is a small message.");
    channel->BasicPublish(exchange_name, routing, msg_in);
    channel->BasicPublish(exchange_name, routing, BasicMessage::Create("Second message") );
    
    channel->BasicPublish(exchange_name, routing,
                          BasicMessage::Create(boost::lexical_cast<std::string>(42) + " - message" ) );
    
    // odbior
    channel->DeclareQueue(queue_name, false, false, false, false);
    channel->BindQueue(queue_name, exchange_name, routing);
    channel->BasicConsume(queue_name, consumer_tag);
    
    
    Envelope::ptr_t env;
    while( true )
    {
        
        if ( channel->BasicConsumeMessage(consumer_tag, env, 500) )
        {
            std::cout << "Envelope received: \n"
            << " Exchange: " << env->Exchange()
            << "\n Routing key: " << env->RoutingKey()
            << "\n Consumer tag: " << env->ConsumerTag()
            << "\n Delivery tag: " << env->DeliveryTag()
            << "\n Redelivered: " << env->Redelivered()
            << "\n Body: " << env->Message()->Body() << std::endl;
        }
        else
        {
            std::cout << "empty\n";
            
            break;
        }
    }
}



int topic()
{
    const std::string queue_name = "nsn-queue-topic";
    const std::string exchange_name = "nsn-exchange-topic";
    const std::string consumer_tag = "nsn-consumertag";
    const std::string routing_publish = "nsn.low.log";
    
    Channel::ptr_t channel;
    channel = Channel::Create();
    
    // przygotowanie
    channel->DeclareExchange(exchange_name, Channel::EXCHANGE_TYPE_TOPIC, false, false, false);
    
    const std::string routing_consume = "nsn.low.#";
    const std::string low_queue = "nsn-low.any";
    channel->DeclareQueue(low_queue, false, false, false, false);
    channel->BindQueue(low_queue, exchange_name, routing_consume);
    
    
    // wysylanie
    BasicMessage::ptr_t msg_in = BasicMessage::Create();
    msg_in->Body("This is a small message.");
    
    channel->BasicPublish(exchange_name, routing_publish, msg_in);
    channel->BasicPublish(exchange_name, routing_publish, BasicMessage::Create("Second message") );
    channel->BasicPublish(exchange_name, routing_publish,
                          BasicMessage::Create(boost::lexical_cast<std::string>(42) + " - message" ) );

    
    // odbior

    channel->BasicConsume(low_queue, consumer_tag);
    
    Envelope::ptr_t env;
    while( true )
    {
        
        if ( channel->BasicConsumeMessage(consumer_tag, env, 500) )
        {
            std::cout << "Envelope received: \n"
            << " Exchange: " << env->Exchange()
            << "\n Routing key: " << env->RoutingKey()
            << "\n Consumer tag: " << env->ConsumerTag()
            << "\n Delivery tag: " << env->DeliveryTag()
            << "\n Redelivered: " << env->Redelivered()
            << "\n Body: " << env->Message()->Body() << std::endl;
        }
        else
        {
            std::cout << "empty\n";
            
            break;
        }
    }

}

int main()
{
    topic();
    direct();
    fanout();
    return 0;
}

