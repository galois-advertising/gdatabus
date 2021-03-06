#pragma once
#include <set>
#include <map>
#include <string>
#include <optional>
#include "loader_def.h"
#include "configuration.h"
#include "ignore.pb.h"
#include "pack_header.h"

namespace galois::gdatabus {
typedef std::string file_path_t;
template <typename event_t>
struct event_traits_t {
    typedef event_t event;
    typedef typename event_t::schema_t update_t;
    typedef typename event_t::derivative_t derivative_t;
    typedef typename event_t::key_t delete_t;
};

template<typename gtable_env, typename eventt_t>
struct schema_callbacks {
    typedef gtable_env env; 
    typedef eventt_t event;
    static int insert(env& env, const galois::gformat::pack_header_t& header,
        const typename event::update_t& data) {
        return 0;
    };

    static int del(env& env, const galois::gformat::pack_header_t& header,
        const typename event::delete_t& data) {
        return 0;
    };

    static int update(env& env, const galois::gformat::pack_header_t& header,
        const typename event::update_t& data) {
        return 0;
    };
};

template<typename gtable_env>
class default_traits {
public:
    typedef gtable_env env;

    // user
    typedef event_traits_t<galois::gdatabus::ignore> user_event;
    typedef schema_callbacks<env, user_event> user_event_callbacks;

    // plan
    typedef event_traits_t<galois::gdatabus::ignore> plan_event;
    typedef schema_callbacks<env, plan_event> plan_event_callbacks;

    // unit 
    typedef event_traits_t<galois::gdatabus::ignore> unit_event;
    typedef schema_callbacks<env, unit_event> unit_event_callbacks;

    // xdv 
    typedef event_traits_t<galois::gdatabus::ignore> xdv_event;
    typedef schema_callbacks<env, xdv_event> xdv_event_callbacks;

    // idea 
    typedef event_traits_t<galois::gdatabus::ignore> idea_event;
    typedef schema_callbacks<env, idea_event> idea_event_callbacks;
};

typedef std::map<std::string, std::pair<file_path_t, uint64_t> > snapshot_files_t;
template<typename traits>
class loader : public configuration {
public:
    typedef typename traits::env env_t;
    typedef typename traits::user_event user_event;
    typedef typename traits::user_event_callbacks user_event_callbacks;
    typedef typename traits::plan_event plan_event;
    typedef typename traits::plan_event_callbacks plan_event_callbacks;
    typedef typename traits::unit_event unit_event;
    typedef typename traits::unit_event_callbacks unit_event_callbacks;
    typedef typename traits::xdv_event xdv_event;
    typedef typename traits::xdv_event_callbacks xdv_event_callbacks;
    typedef typename traits::idea_event idea_event;
    typedef typename traits::idea_event_callbacks idea_event_callbacks;

public:
    loader();
    virtual ~loader();
    bool init(env_t);
    bool load_base();
    bool load_increment();
    template<typename callbacks>
    static bool load_snap(std::string file, uint64_t datetime, env_t env);
private:
    snapshot_files_t get_lastest_snapshot();
    std::optional<std::string> get_schema_name(event_schema_id);
private:
    env_t handler;
    std::map<event_schema_id, std::string> schema_names;
};
}
#include "loader.hh"