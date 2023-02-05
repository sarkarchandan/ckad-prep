#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "subscriber.h"


rds::Subscriber::Subscriber(std::string const &host, uint16_t port, std::string const &queue)
:RedisBase(host, port, queue)
{
    _session = boost::uuids::to_string(boost::uuids::random_generator_mt19937()());
    _proc_q_name = _q_name + ":processing";
    _lease_key_pref = _q_name + ":leased_by_session:";
}

inline size_t rds::Subscriber::_key_for(std::string const &item) const
{
    return std::hash<std::string>{}(item);
}

bool rds::Subscriber::_lease_exist(std::string const &item) const
{
    sw::redis::StringView key = _lease_key_pref + std::to_string(_key_for(item));
    return ctx -> exists(key) == 1;
}

bool rds::Subscriber::empty() const
{
    return ctx -> llen(_q_name) && ctx -> llen(_proc_q_name);
}

sw::redis::OptionalString rds::Subscriber::lease(
    std::chrono::seconds const &duration, 
    std::chrono::seconds const &timeout,  
    bool blocking)
{
    sw::redis::OptionalString item;
    (blocking)
        ? item = ctx -> brpoplpush(_q_name, _proc_q_name, timeout)
        : item = ctx -> brpoplpush(_q_name, _proc_q_name);
    if (item.has_value())
    {
        std::string key = _lease_key_pref + std::to_string(_key_for(item.value()));
        ctx -> setex(key, duration, _session);
    }
    return item;
}

void rds::Subscriber::complete(std::string const &item)
{
    ctx -> lrem(_proc_q_name, 0, item);
    std::string _key = _lease_key_pref + std::to_string(_key_for(item));
    ctx -> del(_key);
}