#include "pricer/EuropeanCall.h"
#include "pricer/MonteCarloResult.h"
#include "pricer/Parameters.h"
#include <algorithm>
#include <cmath>
#include <amqpcpp.h>
#include <amqpcpp/linux_tcp.h>
#include <nlohmann/json.hpp>
#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <numeric>

int num_paths = 1000000;

int main(){
    AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::Create("localhost");
    channel->DeclareQueue("task_queue", false, true, false, false);
    channel->DeclareQueue("result_queue", false, true, false, false);
    AmqpClient::Envelope::ptr_t envelope = channel->BasicConsumeMessage("task_queue");

    Parameters params = nlohmann::json::parse(envelope->Message()->Body()).get<Parameters>();
    EuropeanCall call(params);

    double option_price = call.price();
    struct MonteCarloResult result{call.getMeanPayoffs(), call.getStdError(), num_paths};
    nlohmann::json j = result;
    channel->BasicPublish("", "result_queue", AmqpClient::BasicMessage::Create(j.dump()));
    

    return 0;
}