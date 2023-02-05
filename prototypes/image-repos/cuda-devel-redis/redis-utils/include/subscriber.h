#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <chrono>
#include <cstdint>
#include "base.h"

namespace rds
{
    class Subscriber: protected RedisBase
    {
        std::string _proc_q_name;
        std::string _session;
        std::string _lease_key_pref;

        inline size_t _key_for(std::string const &item) const;
        bool _lease_exist(std::string const &item) const;

        public:
        // Subscriber has not default constructor
        Subscriber() = delete;
        // Subscriber is not copyable
        Subscriber(Subscriber const&) = delete;
        Subscriber operator=(Subscriber const&) = delete;
        // Subscriber is movable
        Subscriber(Subscriber &&) = default;
        Subscriber& operator=(Subscriber &&) = default;

        Subscriber(std::string const &host, uint16_t port, std::string const &queue);

        ~Subscriber() {};

        inline std::string session() const { return _session; }
        bool empty() const;
        sw::redis::OptionalString lease(
            std::chrono::seconds const &duration = std::chrono::seconds(5), 
            std::chrono::seconds const &timeout = std::chrono::seconds(2), 
            bool blocking = true);
        void complete(std::string const &item);
    };
} // namespace rds

#endif // SUBSCRIBER_H