// cc -Wall -g -ggdb -o streams streams.c -lhiredis
// Usage: ./streams <add count> <read count> [block time, default: 1]

#include <hiredis/hiredis.h>
#include <stdio.h>

#define dbg_log(fmt, ... ) \
    do { \
        fprintf(stderr, "%s:%d : " fmt "\n", __FILE__, __LINE__,__VA_ARGS__); \
    } while (0);


#define dbg_abort(fmt, ...) \
    do { \
        dbg_log(fmt, __VA_ARGS__); exit(-1); \
    } while (0)

void assertReplyType(redisContext *c, redisReply *r, int type) {
    if (r == NULL)
        dbg_abort("NULL redisReply (error: %s)", c->errstr);
    if (r->type != type)
        dbg_abort("Expected reply type %d but got type %d", type, r->type);
}

void assertReply(redisContext *c, redisReply *r) {
    if (r == NULL)
        dbg_abort("NULL redisReply (error: %s)", c->errstr);
}

void dumpReply(redisReply *r, int indent) {
    sds buffer = sdsempty();

    switch (r->type) {
        case REDIS_REPLY_STRING:
            buffer = sdscatfmt(buffer, "(string) %s\n", r->str);
            break;
        case REDIS_REPLY_STATUS:
            buffer = sdscatfmt(buffer, "(status) %s\n", r->str);
            break;
        case REDIS_REPLY_INTEGER:
            buffer = sdscatlen(buffer, "(integer) %lld\n", r->integer);
            break;
        case REDIS_REPLY_NIL:
            buffer = sdscatlen(buffer, "(nill)\n", 7);
            break;
        case REDIS_REPLY_ERROR:
            buffer = sdscatfmt(buffer, " (error) %s", r->str);
            break;
        case REDIS_REPLY_ARRAY:
            for (size_t i = 0; i < r->elements; i++) {
                dumpReply(r->element[i], indent + 2);
            }
            break;
        default:
            dbg_abort("Don't know how to deal with reply type %d", r->type);
    }

    if (sdslen(buffer) > 0) {
        for (int i = 0; i < indent; i++)
            printf(" ");

        printf("%s", buffer);
    }

    sdsfree(buffer);
}

void initStreams(redisContext *c, const char *stream) {
    redisReply *r = redisCommand(c, "XGROUP CREATE %s diameter $ MKSTREAM", stream);
    assertReply(c, r);
    freeReplyObject(r);
}

#define STREAM_NAME "example-stream"

int main(int argc, const char **argv) {
    redisContext *c = redisConnect("localhost", 6379);
    redisReply *r;

    if (argc < 3) {
        dbg_abort("Usage:  %s <add entries> <read entries> [block ms]", argv[0]);
    }

    int add = atoi(argv[1]);
    int read = atoi(argv[2]);
    int block = argc > 3 ? atoi(argv[3]) : 1;
    if (add < 0 || read < 0)
        dbg_abort("Neither read or add can be negative (read: %d, add: %d)", read, add);

    /* Create streams/groups */
    initStreams(c, STREAM_NAME);

    for (int i = 0; i < add; i++) {
        r = redisCommand(c, "XADD %s * foo mem:%d", STREAM_NAME, i);
        assertReplyType(c, r, REDIS_REPLY_STRING);
        printf("Added foo -> mem:%d (id: %s)\n", i, r->str);
        freeReplyObject(r);
    }

    for (int i = 0; i < read; i++) {
        printf("[%d] Sending XREADGROUP (stream: %s, BLOCK: %d)\n", i, STREAM_NAME, block);
        r = redisCommand(c, "XREADGROUP GROUP diameter Tom BLOCK %d COUNT 1 NOACK STREAMS %s >", 
                         block, STREAM_NAME);

        assertReply(c, r);

        dumpReply(r, 0);
        freeReplyObject(r);
    }

    redisFree(c);
}