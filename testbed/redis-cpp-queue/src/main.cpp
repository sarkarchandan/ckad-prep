#include <iostream>
#include <unistd.h>
#include "subscriber.h"

int main()
{
    rds::Subscriber sub = rds::Subscriber("localhost", 8888, "foo");
    std::cout << "Working wit sessionID: " << sub.session() <<  "\n";
    std::string q_state = (sub.empty() == 1) ? "True" : "False";
    std::cout << "Inital queue state: " << q_state << "\n";
    while (true)
    {
        sw::redis::OptionalString item = sub.lease();
        if (item.has_value())
        {
            std::string value = item.value();
            if (value == "EOQ") break;
            std::cout << "Working on item: " << item.value() << "\n";
            sleep(2); // Mocking a long running work
            sub.complete(value);
        }else
        {
            std::cout << "Waiting for work..." << "\n";
        }
        sleep(1);
    }
    std::cout << "Last item processed exiting" << "\n";
    return EXIT_SUCCESS;
}