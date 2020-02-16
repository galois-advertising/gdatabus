#include <string>
#include <iostream>
#include "loader.h"
#include "pack_header.h"
#include "dbschema/freyja/databus_event.pb.h"

using namespace galois::gdatabus;

typedef int parse_env;
typedef int update_env;

typedef event_traits_t<galois::freyja::user_event> my_user_event;

struct my_user_callbacks : public schema_callbacks<parse_env, update_env, my_user_event> {
    static int insert(parse_env& env, galois::gformat::pack_header_t& header, 
        const event::update_t & data, uint32_t data_type) {
        INFO("user_callback::insert:[%u][%u] [%s]", 
            data.key().user_id(), data.user_stat(), data.region().c_str());
        return 0;
    };

    static int del(parse_env& env, galois::gformat::pack_header_t& header, 
        const event::delete_t& data, uint32_t data_type) {
        return 0;
    };

    static int update(update_env& env, galois::gformat::pack_header_t& header, 
        const event::update_t& data, uint32_t data_type) {
        return 0;
    };
};

struct my_traits: public default_traits<parse_env, update_env> {
    typedef parse_env parse_env;
    typedef update_env update_env;

    typedef my_user_event user_event;
    typedef my_user_callbacks user_callbacks;
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
    parse_env penv; 
    update_env uenv; 
    galois::gformat::pack_header_t header; 
    my_user_event::update_t update; 
    my_user_event::delete_t del; 
    uint32_t data_type = 0;
    my_loader::user_callbacks::insert(penv, header, update, data_type);
    my_loader::user_callbacks::del(penv, header, del, data_type);
    my_loader::user_callbacks::update(uenv, header, update, data_type);
    ld.create();
    ld.load_base();
    return 0;   
}