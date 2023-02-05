#include <iostream>
#include "subscriber.h"

int main(int argc, const char** argv)
{
    const std::string host = (argc > 1) ? argv[1] : "redis-server";
    const uint16_t port = (argc > 2) ? *argv[2] : 6379;
    const std::string queue = (argc > 3) ? argv[3] : "foo";
    rds::Subscriber sub = rds::Subscriber(host, port, queue);
    std::cout << "Working wit sessionID: " << sub.session() <<  "\n";
    std::string q_state = (sub.empty() == 1) ? "True" : "False";
    std::cout << "Inital queue state: " << q_state << "\n";
    int empty_q_probe = 0;
    while (!sub.empty())
    {
        sw::redis::OptionalString item = sub.lease();
        if (item.has_value())
        {
            std::string value = item.value();
            std::cout << "Working on item: " << item.value() << "\n";
            sub.complete(value);
        }else
        {
            std::cout << "Waiting for work..." << "\n";
        }
    }
    std::cout << "Last item processed exiting" << "\n";
    return EXIT_SUCCESS;
}