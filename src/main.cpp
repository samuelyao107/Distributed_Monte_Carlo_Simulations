#include "pricer/EuropeanCall.h"
#include "pricer/MonteCarloResult.h"
#include "pricer/Parameters.h"
#include <algorithm>
#include <cmath>
#include <nlohmann/json.hpp>
#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <numeric>
#include <iostream>
#include <thread>



int main(){

    const char* host = std::getenv("RABBITMQ_HOST");
    if(host == nullptr) host = "localhost";

    AmqpClient::Channel::ptr_t channel;
    while(true){
        try {
            AmqpClient::Channel::OpenOpts opts;
            opts.host = host;
            opts.port = 5672;
            opts.auth = AmqpClient::Channel::OpenOpts::BasicAuth("guest", "guest");
            channel = AmqpClient::Channel::Open(opts);
            std::cout << "Connecté à RabbitMQ sur " << host << std::endl;
            break;
        } catch(const std::exception& e) {
            std::cout << "RabbitMQ pas prêt, retry dans 2s... (" << e.what() << ")" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
    
    channel->DeclareQueue("task_queue", false, true, false, false);
    channel->DeclareQueue("result_queue", false, true, false, false);
    
    std::string consumer_tag = channel->BasicConsume("task_queue", "", false, false, false);

    AmqpClient::Envelope::ptr_t envelope = channel->BasicConsumeMessage(consumer_tag);

    Parameters params = nlohmann::json::parse(envelope->Message()->Body()).get<Parameters>();
    EuropeanCall call(params);

    double option_price = call.price();
    struct MonteCarloResult result{call.getMeanPayoffs(), call.getStdError(), params.num_paths()};
    nlohmann::json j = result;
    channel->BasicPublish("", "result_queue", AmqpClient::BasicMessage::Create(j.dump()));
    

    return 0;
}