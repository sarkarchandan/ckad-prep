#include <iostream>
#include <unistd.h>
#include "publisher.h"

int main(int argc, const char** argv)
{
    const std::string host = (argc > 1) ? argv[1] : "redis-server";
    const uint16_t port = (argc > 2) ? *argv[2] : 6379;
    const std::string queue = (argc > 3) ? argv[3] : "foo";
    rds::Publisher pub = rds::Publisher(host, port, queue);
    for(size_t idx = 1; idx <= 15; idx += 1)
    {
        std::string stub = "WorkItem";
        pub.publish(stub + "-" + std::to_string(idx));
        std::cout << "Publishing: " << stub + "-" + std::to_string(idx) << "\n";
        sleep(1);
    }
    return EXIT_SUCCESS;
}