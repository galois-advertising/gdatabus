#include <string>
#include <iostream>
#include "loader.h"
#include "pack_header.h"
#include "dbschema/freyja/databus_event.pb.h"

using namespace galois::gdatabus;

typedef int* my_env;
typedef event_traits_t<galois::freyja::user_event> my_user_event;

struct my_user_callbacks : public schema_callbacks<my_env, my_user_event> {
    static int insert(env& env, galois::gformat::pack_header_t& header, 
        const event::update_t & data) {
        INFO("user_callback::insert:[%u][%u] [%s][%s]", 
            data.key().user_id(), data.user_stat(), data.region().c_str(),
            data.user_name().c_str());
        return 0;
    };

    static int del(env& env, galois::gformat::pack_header_t& header, 
        const event::delete_t& data) {
        return 0;
    };

    static int update(env& env, galois::gformat::pack_header_t& header, 
        const event::update_t& data) {
        return 0;
    };
};

struct my_traits: public default_traits<my_env> {
    typedef my_env env;
    typedef my_user_event user_event;
    typedef my_user_callbacks user_event_callbacks;
};

class my_loader : public galois::gdatabus::loader<my_traits> {
public:
    file_path_t databus_root_path() const {return "../test/shared/data/GALOIS";};
    file_path_t stream_path() const {return "./stream/";};
    std::string stream_prefix() const {return "stream_GALOIS_DATA_";};
    file_path_t snapshot_path() const {return "./snapshot/";};
    std::string snapshot_prefix() const {return "snapshot_GALOIS_DATA_";};
};

int main()
{
    my_loader ld;
    my_env penv; 
    galois::gformat::pack_header_t header; 
    my_user_event::update_t update; 
    my_user_event::delete_t del; 
    my_user_event::derivative_t derivative;
    my_loader::user_event_callbacks::insert(penv, header, update);
    my_loader::user_event_callbacks::del(penv, header, del);
    my_loader::user_event_callbacks::update(penv, header, update);
    ld.init(nullptr);
    ld.load_base();
    return 0;   
}