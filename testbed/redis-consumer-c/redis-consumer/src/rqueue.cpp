#include <stdexcept>
#include <functional>
#include <assert.h>
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
    redisReply *repl;
    repl = (redisReply*) redisCommand(ctx, "PING");
    if (repl != NULL && strcmp(repl -> str, "PONG") != 0)
    {
        freeReplyObject(repl);
        redisFree(ctx);
        throw std::runtime_error("Could not connect to redis server, exiting...");
    }
    _session = boost::uuids::to_string(boost::uuids::random_generator_mt19937()());
    _processing_q_name = _main_q_name + ":processing";
    _lease_key_prefix = _main_q_name + ":leased_by_session:";
}

inline size_t util::RedisQueue::_key_for(const char *item)
{
    return std::hash<std::string>{}(item);
}

size_t util::RedisQueue::_llen(RedisQueue::QType _q) const
{
    redisReply *repl = (redisReply*) redisCommand(
        ctx, "%s %s", 
        LLEN, 
        (_q == QType::MAIN) ? _main_q_name.c_str() : _processing_q_name.c_str()
    );
    size_t _len;
    if (repl != nullptr) _len = repl -> integer;
    freeReplyObject(repl);
    return _len;
}

bool util::RedisQueue::_lease_exists(const char *item)
{
    const char *arg = (_lease_key_prefix + std::to_string(_key_for(item))).c_str();
    redisReply *repl = (redisReply*) redisCommand(
        ctx, "%s %s", 
        EXISTS, arg);
    bool _exs;
    if (repl != nullptr) _exs = repl -> integer > 0;
    freeReplyObject(repl);
    return _exs;
}

void util::RedisQueue::_rpoplpush(bool blocking, uint8_t timeout, char *item)
{
    redisReply *repl;
    (blocking)
        ? repl = (redisReply*) redisCommand(
            ctx, "%s %s %s %u",
            BRPOPLPUSH, _main_q_name.c_str(), _processing_q_name.c_str(), timeout
        )
        : repl = (redisReply*) redisCommand(
            ctx, "%s %s %s",
            RPOPLPUSH, _main_q_name.c_str(), _processing_q_name.c_str()
        );
    if (repl != nullptr && repl -> len > 0)
    {
        item = (char*) malloc(repl -> len * sizeof(char));
        strcpy(item, repl -> str);
    }
    freeReplyObject(repl);
}

void util::RedisQueue::_setex(const char* item, uint8_t duration)
{
    std::string _item_key = _lease_key_prefix + std::to_string(_key_for(item));
    redisReply *repl = (redisReply*) redisCommand(
        ctx,
        "%s %s %u %s",
        SETEX, _item_key.c_str(), duration, item
    );
    if(repl != nullptr) assert(strcmp(repl -> str, "OK") == 0);
    freeReplyObject(repl);
}

void util::RedisQueue::_lrem(const char* item, uint8_t count)
{
    redisReply *repl = (redisReply*) redisCommand(
        ctx,
        "%s %s %u %s",
        LREM, _processing_q_name.c_str(), count, item
    );
    if(repl != nullptr) freeReplyObject(repl);
}

void util::RedisQueue::_del(const char *item)
{
    std::string _item_key = _lease_key_prefix + std::to_string(_key_for(item));
    redisReply *repl = (redisReply*) redisCommand(
        ctx,
        "%s %s",
        DEL, _item_key.c_str()
    );
    if(repl != nullptr) freeReplyObject(repl);
}

bool util::RedisQueue::empty() const
{
    return (_llen(RedisQueue::QType::MAIN) == 0) && (_llen(RedisQueue::QType::PROCESSING));
}

void util::RedisQueue::lease(char *item, uint8_t duration, uint8_t timeout, bool blocking)
{
    _rpoplpush(blocking, timeout, item);
    if (item != nullptr) _setex(item, duration);
}

void util::RedisQueue::complete(const char* item)
{
    _lrem(item);
    _del(item);
}