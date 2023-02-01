#include <stdio.h>
#include <hiredis.h>
#include <stdint.h>
#include <stdlib.h>
#include <adapters/poll.h>

// Loop control flag, intentionally placed in global scope for simplicity
static int exit_loop = 0;

/// @brief Defines callback for asynchronous connection to redis
/// @param c Asynchronous redis context
/// @param status Asynchronous connection status
void connectionCallback(const redisAsyncContext *c, int status)
{
    if(status != REDIS_OK)
    {
        printf("Encountered Connection Error: %s\n", c -> errstr);
        exit_loop = 1;
        return;
    }
    printf("Connected to Redis server...\n");
}

/// @brief Defines callback for a disconnection event
/// @param c Asynchronous redis context
/// @param status Asynchronous connection status
void disconnectCallback(const redisAsyncContext *c, int status)
{
    exit_loop = 1;
    if(status != REDIS_OK)
    {
        printf("Unexpected Error: %s\n", c -> errstr);
        return;
    }
    printf("Disconnected from Redis server...\n");
}

/// @brief Defines call back for a GET event
/// @param c Asynchronous redis context
/// @param r Void pointer denoting a reply from Redis server
/// @param privData Nullable data retrieved from Redis
void getCallback(redisAsyncContext *c, void *r, void *privData)
{
    redisReply *reply = r;
    if(reply == NULL)
    {
        return;
    }
    printf("Response - %s - %s\n", (char*)privData, reply -> str);
}

int main(int argc, char **argv)
{
    // Define connection properties for redis
    const char *host_name = (argc > 1) ? argv[1] : "localhost";
    uint16_t port = (argc > 2) ? *argv[2] : 8888;
    // const char* queue_name = (argc > 3) ? argv[3] : "foo";
    // Establish an asynchronous connection
    redisAsyncContext *ctx = redisAsyncConnect(host_name, port);
    if (ctx -> err)
    {
        // TODO: Understand if it is ok to let the asynchronous context pointer to leak
        printf("Encountered Connection Error: %s\n", ctx -> errstr);
        return 1;
    }
    // Register for polling
    redisPollAttach(ctx);
    redisAsyncSetConnectCallback(ctx, connectionCallback);
    redisAsyncSetDisconnectCallback(ctx, disconnectCallback);
    redisAsyncCommand(ctx, getCallback, (char*)"Received: ", "LPOP foo");
    while(!exit_loop)
    {
        redisPollTick(ctx, -1);
    }
    // Free asynchronous Redis context
    redisAsyncFree(ctx);
    return 0;
}