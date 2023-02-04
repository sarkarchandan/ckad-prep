#include <stdio.h>
#include <hiredis.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    // Define connection properties for redis
    const char *host_name = (argc > 1) ? argv[1] : "redis";
    uint16_t port = (argc > 2) ? *argv[2] : 6379;
    const char* queue_name = (argc > 3) ? argv[3] : "foo";
    // Attempt to establish connection
    struct timeval timeout = {1, 500000};
    redisContext *ctx = redisConnectWithTimeout(host_name, port, timeout);
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
        return 1;
    }
    redisReply *reply;
    // Test connection
    reply = redisCommand(ctx, "PING");
    printf("PING Response: %s\n", reply -> str);
    freeReplyObject(reply);
    // Attempt to send payload
    for(size_t idx = 1; idx <= 10; idx += 1)
    {
        char* payload = (char*) malloc(50 * sizeof(char));
        sprintf(payload, "bar-%lu", idx);
        reply = redisCommand(ctx, "%s %s %s", "RPUSH", queue_name, payload);
        free(payload);
        printf("RPUSH Response: %lld\n", reply -> integer);
        freeReplyObject(reply);
        sleep(1);
    }
    // Free Redis context
    redisFree(ctx);
    return 0;
}