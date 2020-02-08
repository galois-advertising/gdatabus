#pragma once
#include <set>
#include <map>
#include <string>
#include <optional>
#include "loader_def.h"
#include "loader_item.h"
#include "configuration.h"
#include "dbschema/freyja/databus_event.pb.h"

namespace galois::gdatabus {
typedef int pack_header_t;
template <typename event, typename custom_event>
struct schema_traits {
    typedef typename event::update_event update_event;
    typedef typename event::del_event del_event;
    typedef typename custom_event::update_event custom_update_event;
    typedef typename custom_event::del_event custom_del_event;
};

template<typename parse_env, typename update_env, typename schema>
struct schema_callbacks {
    static int parse(
            parse_env& env,
            const pack_header_t& header,
            const typename schema::update_event& data,
            typename schema::custom_update_event& custom_data, 
            uint32_t data_type) {
        return 0;
    };

    static int parse(
            parse_env& env, 
            const pack_header_t& header,
            const typename schema::del_event& data,
            typename schema::custom_del_event& custom_data, 
            uint32_t data_type) {
        return 0;
    };

    static int update(
            update_env& env,
            const pack_header_t& header,
            const typename schema::update_event& data,
            const typename schema::custom_update_event& custom_data, 
            uint32_t data_type) {
        return 0;
    };

    static int update(
            update_env& env,
            const pack_header_t& header,
            const typename schema::del_event& _data,
            const typename schema::custom_update_event& custom_data, 
            uint32_t data_type) {
        return 0;
    };
};

template<typename parse_env_t, typename update_env_t>
class default_traits {
public:
    typedef parse_env_t parse_env;
    typedef update_env_t update_env;

    // user
    typedef schema_traits<ignore_event, ignore_event> user_events;
    typedef schema_callbacks<parse_env, update_env, user_events> user_callbacks;

    // plan
    typedef schema_traits<ignore_event, ignore_event> plan_events;
    typedef schema_callbacks<parse_env, update_env, plan_events> plan_callbacks;
};

template<typename traits>
class loader : public configuration {
public:
    typedef typename traits::parse_env parse_env;
    typedef typename traits::update_env update_env;

    typedef typename traits::user_events user_events;
    typedef typename traits::plan_events plan_events;

public:
    loader();
    virtual ~loader();
    bool create();
private:
    int get_lastest_snapshot();
    std::optional<std::string> get_schema_name(event_schema_id es);
private:
    std::map<event_schema_id, std::string> schema_names;
};
}
#include "loader.hh"