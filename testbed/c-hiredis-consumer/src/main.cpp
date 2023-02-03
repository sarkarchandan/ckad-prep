#include <iostream>
#include <string.h>
#include <unistd.h>
#include "rqueue.h"

int main(int argc, char **argv)
{
    std::string host_name = (argc > 1) ? argv[1] : "localhost";
    uint16_t port = (argc > 2) ? *argv[2] : 8888;
    std::string queue_name = (argc > 3) ? argv[3] : "foo";
    util::RedisQueue q = { queue_name, host_name, port  };
    std::cout << "Worker with Session ID: " << q.session_id() << "\n";
    std::cout << "Initial queue state empty ?: " << q.empty() << "\n";
    while (!q.empty())
    {
        char *item = (char*) malloc(10 * sizeof(char));
        q.lease(item);
        if(item != nullptr && strlen(item) > 0)
        {
            std::cout << "Processing item: " << item << "\n";
            // Here we would do some actual work instead of sleeping like 
            // executing a CUDA kernel
            sleep(2);
            q.complete(item);
            free(item);
        }else
        {
            std::cout << "Waiting..." << "\n";
        }
    }
    std::cout << "All items processed, exiting..." << "\n";
    return 0;
}