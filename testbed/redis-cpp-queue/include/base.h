#ifndef BASE_H
#define BASE_H

#include <string>
#include <memory>
#include <sw/redis++/redis++.h>

namespace rds
{
    typedef std::unique_ptr<sw::redis::Redis> RedisPtr;

    struct RedisBase
    {
        protected:
        RedisPtr ctx;
        std::string _q_name;

        public:
        RedisBase(std::string const &host, uint16_t port, std::string const &queue)
        :_q_name(queue)
        {
            sw::redis::ConnectionOptions opts;
            opts.host = host;
            opts.port = port;
            opts.db = 0;
            opts.connect_timeout = std::chrono::seconds(2);
            opts.keep_alive = true;
            ctx = std::make_unique<sw::redis::Redis>(opts);
        }

        // Has not default constructor
        RedisBase() = delete;
        // Is not copyable
        RedisBase(RedisBase const&) = delete;
        RedisBase operator=(RedisBase const&) = delete;
        // Is movable
        RedisBase(RedisBase &&) = default;
        RedisBase& operator=(RedisBase &&) = default;

        ~RedisBase() { ctx.release(); }
    };
} // namespace rds


#endif // BASE_H