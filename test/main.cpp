#include <string>
#include <iostream>
#include "loader.h"
#include "dbschema/freyja/databus_event.pb.h"

using namespace galois::gdatabus;

typedef int parse_env;
typedef int update_env;

typedef schema_traits<galois::freyja::user_event, ignore_event> user_schema;

struct user_callbacks : public schema_callbacks<parse_env, update_env, user_schema> {
    static int parse(parse_env& env, pack_header_t& header, 
            const user_schema::update_event& data, 
            user_schema::custom_update_event& custom_data, 
            uint32_t data_type) {
        return 0;
    };

    static int parse(parse_env& env, pack_header_t& header, 
            const user_schema::del_event& data, 
            user_schema::custom_del_event& custom_data, 
            uint32_t data_type) {
        return 0;
    };

    static int update(update_env& env, pack_header_t& header, 
            const user_schema::update_event& data, 
            const user_schema::custom_update_event& custom_data, 
            uint32_t data_type) {
        return 0;
    };

    static int update(update_env& env, pack_header_t& header, 
            const user_schema::del_event& data, 
            const user_schema::custom_del_event& custom_data, 
            uint32_t data_type) {
        return 0;
    };
};

struct my_traits: public default_traits<parse_env, update_env> {

    typedef parse_env parse_env;
    typedef update_env update_env;

};
int main()
{
    galois::freyja::idea_event a;
    a.set_id(1);
    user_schema b;
    return 0;   
}