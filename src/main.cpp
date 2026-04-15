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
    double sum_payoffs=0.0;
    double sum_payoffs_squared=0.0;

    for(int i=0; i<num_paths; i++){
        sum_payoffs +=std::max(0.0, call.price() - params.K());
        sum_payoffs_squared += sum_payoffs * sum_payoffs;
    }
    double mean_payoffs= sum_payoffs / num_paths;
    double mean_payoffs_squared= sum_payoffs_squared / num_paths;
    double std_error = std::sqrt((mean_payoffs_squared - (mean_payoffs * mean_payoffs)) / num_paths);
    double discounted= std::exp(-params.r() * params.T());
    double option_price = discounted * mean_payoffs;
    struct MonteCarloResult result{mean_payoffs, std_error, num_paths};
    nlohmann::json j = result;
    channel->BasicPublish("", "result_queue", AmqpClient::BasicMessage::Create(j.dump()));
    

    return 0;
}