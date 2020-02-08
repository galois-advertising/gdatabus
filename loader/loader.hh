#include <optional>
#include <string>
#include "log.h"

namespace galois::gdatabus {
const static char * version_prefix = "version_";
const static char * data_file_suffix = ".data";
const static char * desc_file_suffix = ".n";
template<typename traits>
bool loader<traits>::create() 
{
    if (auto name = get_schema_name(event_schema_id::USER); name) {
        this->schema_names[event_schema_id::USER] = name;
    }
    if (auto name = get_schema_name(event_schema_id::PLAN); name) {
        this->schema_names[event_schema_id::PLAN] = name;
    }
    return true;
}
template<typename traits>
std::optional<std::string> loader<traits>::get_schema_name(event_schema_id es) {
    switch (es) {
        case event_schema_id::USER:return std::optional<std::string>("user");break;
        case event_schema_id::PLAN:return std::optional<std::string>("plan");break;
        default:return std::nullopt;
    }
    return std::nullopt;
}
template<typename traits>
int loader<traits>::get_lastest_snapshot() 
{
    // 打开数据目录
    auto dir = opendir(batch_root_path().c_str());
    if (dir == nullptr) {
        log(LOG_LEVEL::FATAL, "failed to open directory [%s].", this->batch_root_path.c_str());
        return -1;
    }
    
    return 0;
}


}