#include "publisher.h"

rds::Publisher::Publisher(std::string const &host, uint16_t port, std::string const &queue)
:RedisBase(host, port, queue){}

size_t rds::Publisher::publish(std::string const &item)
{
    return ctx -> rpush(_q_name, item);
}