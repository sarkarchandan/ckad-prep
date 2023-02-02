#ifndef RQUEUE_H
#define RQUEUE_H

#include <string>
#include <hiredis.h>
#include <stdint.h>


namespace util
{
    /// @brief Encapsulates utilities for a redis worker queue manager
    class RedisQueue
    {
        private:
            /// @brief Internal queue types
            enum Type {MAIN, PROCESSING};
            /// @brief Redis context encapsulating server connection
            redisContext *ctx;
            std::string _session;
            std::string _main_q_name;
            std::string _processing_q_name;
            std::string _lease_key_prefix;

            /// Redis command stubs
            std::string LLEN = "LLEN ";
            std::string EXISTS = "EXISTS ";
            std::string BRPOPLPUSH = "BRPOPLPUSH ";
            std::string SETEX = "SETEX ";
            std::string LREM = "LREM ";
            std::string DEL = "DEL ";
            
            /// @brief Internal dispatcher for redis commands using hiredis api 
            /// using redis command stubs: https://redis.io/commands/
            /// @param command Command to dispatch with REDIS command stubs and arguments
            /// @param response Potential response from command dispatch
            void _redis_dispatch(const char* command, char* response) const;

            inline size_t _key_for(std::string const &item);
            size_t _llen(RedisQueue::Type _q) const;
            bool _lease_exists(std::string const &item);
            void _brpoplpush(std::string const &src, std::string const &dst);
            bool _setex(std::string const &key, uint8_t duration);
            void _lrem(std::string const &key, uint8_t count);
            void _del(std::string const &key);
        
        public:
            RedisQueue() = delete;
            RedisQueue(RedisQueue const&) = delete;
            RedisQueue operator=(RedisQueue const&) = delete;
            
            ~RedisQueue()
            {
                if (ctx != NULL) redisFree(ctx);
            }

            /// @brief Constructor for Redis queue manager
            /// @param queue_name Name of the main messaging channel
            /// @param host_name Redis server host e.g., "localhost", "127.0.0.1", "redis"
            /// @param port Port number for redis server, default 6379 
            /// @param timeout Connection timeout, default 1.5 seconds
            RedisQueue(
                std::string const &queue_name,
                std::string const &host_name, 
                uint16_t port = 6379,
                timeval const &timeout = {1, 500000});

            /// @brief Validator for empty queue manager
            inline bool empty() const;


            void lease(std::string const &item);
            void complete(std::string const &item);  
    };
} // namespace util


#endif // RQUEUE_H