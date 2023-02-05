#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "base.h"

namespace rds
{
    class Publisher: protected RedisBase
    {
        public:
        // Subscriber has not default constructor
        Publisher() = delete;
        // Subscriber is not copyable
        Publisher(Publisher const&) = delete;
        Publisher operator=(Publisher const&) = delete;
        // Subscriber is movable
        Publisher(Publisher &&) = default;
        Publisher& operator=(Publisher &&) = default;

        Publisher(std::string const &host, uint16_t port, std::string const &queue);

        ~Publisher() {};

        size_t publish(std::string const &item);
    };
} // namespace rds

#endif // PUBLISHER