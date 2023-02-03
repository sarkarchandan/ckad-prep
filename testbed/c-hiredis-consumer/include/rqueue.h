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
            enum QType {MAIN, PROCESSING};

            /// @brief Redis context encapsulating server connection
            redisContext *ctx;
            std::string _session;
            std::string _main_q_name;
            std::string _processing_q_name;
            std::string _lease_key_prefix;

            /// Redis command stubs
            const char *LLEN = "LLEN";
            const char *EXISTS = "EXISTS";
            const char *RPOPLPUSH = "RPOPLPUSH";
            const char *BRPOPLPUSH = "BRPOPLPUSH";
            const char *SETEX = "SETEX";
            const char *LREM = "LREM";
            const char *DEL = "DEL";

            /// @brief Internal utility function to generate a unique hash value 
            /// for the item
            inline size_t _key_for(const char *item);
            /// @brief Internal utility function to checks if the item exists in 
            /// the redis queue of leased items 
            bool _lease_exists(const char *item);

            /// Internal utility functions corresponding to redis 
            /// commands used in the implementation 
            size_t _llen(RedisQueue::QType _q) const;
            void _rpoplpush(bool blocking, uint8_t timeout, char *item);
            void _setex(const char* item, uint8_t duration);
            void _lrem(const char* item, uint8_t count = 0);
            void _del(const char *item);
        
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
            /// @param host_name Redis server host e.g., "localhost", 
            /// "127.0.0.1", "redis" etc
            /// @param port Port number for redis server, default 6379 
            /// @param timeout Connection timeout, default 1.5 seconds
            RedisQueue(
                std::string const &queue_name,
                std::string const &host_name, 
                uint16_t port = 6379,
                timeval const &timeout = {1, 500000});

            /// @brief Accessor for session identifier
            inline std::string session_id() const  { return _session; }

            /// @brief Validator for empty queue
            bool empty() const;

            /// @brief Leases a given item from the queue, which essentially 
            /// means to pop the item from the main queue to and push to 
            /// internal processing queue.
            /// @param item Buffer for storing the item currently in processing
            /// @param duration Maximum duration to keep the item in the 
            /// processing queue
            /// @param timeout Timeout for blocking the main queue
            /// @param blocking Whether blocking variant of the RPOPLPUSH will 
            /// be used.
            void lease(char *item, uint8_t duration = 5, uint8_t timeout = 2, bool blocking = true);

            /// @brief Marks the completion of processing a given item
            void complete(const char* item);
    };
} // namespace util


#endif // RQUEUE_H