#include <stdexcept>
#include <functional>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "rqueue.h"

util::RedisQueue::RedisQueue(
                std::string const &queue_name,
                std::string const &host_name, 
                uint16_t port,
                timeval const &timeout)
                :_main_q_name(queue_name)
{
    ctx = redisConnectWithTimeout(host_name.c_str(), port, timeout);
    if(ctx == NULL || ctx -> err)
    {
        if (ctx)
        {
            printf("Encountered Connection Error: %s\n", ctx -> errstr);
            redisFree(ctx);
        } else
        {
            printf("Could not allocate Redis Context.\n");
        }
        throw std::runtime_error("Could not initialize RedisQueue, exiting...");
    }
    _session = boost::uuids::to_string(boost::uuids::random_generator_mt19937()());
    _processing_q_name = _main_q_name + ":processing";
    _lease_key_prefix = _main_q_name + ":leased_by_session:";
}

void util::RedisQueue::_redis_dispatch(const char* command, char* response) const
{
    redisReply *repl = static_cast<redisReply*>(redisCommand(ctx, command));
    printf("Redis Response: %s\n", repl -> str);
    response = repl -> str;
    freeReplyObject(repl);
}

inline size_t util::RedisQueue::_key_for(std::string const& item)
{
    return std::hash<std::string>{}(item);
}

size_t util::RedisQueue::_llen(RedisQueue::Type _q) const
{
    size_t _len;
    const char* command;
    char *response;
    _q == RedisQueue::Type::MAIN 
        ? command = (LLEN + _main_q_name).c_str()
        : command = (LLEN + _processing_q_name).c_str();
    _redis_dispatch(command, response);
    return strtoul(response, NULL, 0);
}

bool util::RedisQueue::_lease_exists(std::string const &item)
{
    const char* command = (
        EXISTS + (_lease_key_prefix + std::to_string(_key_for(item)))
    ).c_str();
    char *response;
    _redis_dispatch(command, response);
    return strtoul(response, NULL, 0) >= 1;
}

inline bool util::RedisQueue::empty() const
{
    return (
        (_llen(RedisQueue::Type::MAIN) == 0) && 
        (_llen(RedisQueue::Type::PROCESSING) == 0)
    );
}