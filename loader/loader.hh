//solopointer1202@gmail.com
#include <optional>
#include <memory>
#include <filesystem>
#include <string>
#include <exception>
#include <fstream>
#include <set>
#include <sstream>
#include <regex>
#include <stdio.h>
#include <future>
#include "thread-pool.h"
#include "log.h"
#include "pb_fixed_file_reader.h"

namespace galois::gdatabus {
const static char * version_prefix = "version_";
const static char * data_file_suffix = "data";
namespace fs = std::filesystem;

template<typename traits>
loader<traits>::loader(){}

template<typename traits>
loader<traits>::~loader(){}

template<typename traits>
bool loader<traits>::init(env_t env) 
{
    handler = env;
    if (auto name = get_schema_name(event_schema_id::USER); name) {
        this->schema_names[event_schema_id::USER] = name.value();
    }
    if (auto name = get_schema_name(event_schema_id::PLAN); name) {
        this->schema_names[event_schema_id::PLAN] = name.value();
    }
    if (auto name = get_schema_name(event_schema_id::UNIT); name) {
        this->schema_names[event_schema_id::UNIT] = name.value();
    }
    if (auto name = get_schema_name(event_schema_id::XDV); name) {
        this->schema_names[event_schema_id::XDV] = name.value();
    }
    if (auto name = get_schema_name(event_schema_id::IDEA); name) {
        this->schema_names[event_schema_id::IDEA] = name.value();
    }
    return true;
}

template<typename traits>
template<typename callbacks>
bool loader<traits>::load_snap(std::string file, uint64_t datetime, env_t env) {
    if (typeid(typename callbacks::event) == 
        typeid(event_traits_t<galois::gdatabus::ignore>::event)) {
        FATAL("pb type is Ignored", "");
        return false;
    }
    std::cout<<typeid(typename callbacks::event).name()<<std::endl;
    std::cout<<typeid(event_traits_t<galois::gdatabus::ignore>::event).name()<<std::endl;
    std::shared_ptr<FILE> infile(fopen(file.c_str(), "rb"), [](FILE* f){if(f)fclose(f);});
    if (infile == nullptr) {
        FATAL("open file[%s] failed.", file.c_str());
        return false;
    }
    galois::gformat::pb_fixed_file_reader<galois::gformat::pack_header_t> protobuf_reader;
    galois::gformat::pack_header_t header;

    const size_t BUF_SIZE = 1024 * 1024;
    std::shared_ptr<char> buffer(new(std::nothrow) char[BUF_SIZE], 
        [](char* p){if(p)delete[]p;});
    if (buffer == nullptr) {
        FATAL("malloc buffer failed.", "");
        return false;
    }

    while (true) {
        auto rc = protobuf_reader.read_record(header, buffer.get(), BUF_SIZE, infile.get());
        if (rc == galois::gformat::error_t::REACH_EOF) {
            break;
        } else if (rc == galois::gformat::error_t::SUCCESS) {
            typename callbacks::event::event event_item;
            if (event_item.ParseFromArray(buffer.get(), header.pack_len)) {
                callbacks::insert(env, header, event_item.update());
            } else {
                FATAL("Parse fail", "");
            }
        } else {
            ERROR("Error:[%d]", static_cast<int>(rc));
            break;
        }
    }
    return true;
}

template<typename traits>
bool loader<traits>::load_base() {
    snapshot_files_t snapshot_files;
    snapshot_files = get_lastest_snapshot();
    std::set<std::future<bool>> tasks;
    ThreadPool thread_pool(5);
    for (auto i = static_cast<uint32_t>(event_schema_id::_begin);
    i != static_cast<uint32_t>(event_schema_id::_end); ++i) {
        if (auto label = schema_names.find(static_cast<event_schema_id>(i));
        label != schema_names.end()) {
            if (auto snap = snapshot_files.find(label->second); snap != snapshot_files.end()) {
                std::string file;
                uint64_t datetime;
                std::tie(file, datetime) = snap->second;
                switch(static_cast<event_schema_id>(i)) {
                case event_schema_id::USER:
                    thread_pool.enqueue(load_snap<user_event_callbacks>, file, datetime, handler);
                    break;
                case event_schema_id::PLAN:
                    thread_pool.enqueue(load_snap<plan_event_callbacks>, file, datetime, handler);
                    break;
                case event_schema_id::UNIT:
                    thread_pool.enqueue(load_snap<unit_event_callbacks>, file, datetime, handler);
                    break;
                case event_schema_id::XDV:
                    thread_pool.enqueue(load_snap<xdv_event_callbacks>, file, datetime, handler);
                    break;
                case event_schema_id::IDEA:
                    thread_pool.enqueue(load_snap<idea_event_callbacks>, file, datetime, handler);
                    break;
                default:FATAL("Invalid schema.", "");break;
                }
            }
        }
    }
    return true;
}

template<typename traits>
bool loader<traits>::load_increment() {
    return true;
}

template<typename traits>
std::optional<std::string> loader<traits>::get_schema_name(event_schema_id es) {
    switch (es) {
        case event_schema_id::USER:return std::optional<std::string>("user");break;
        case event_schema_id::PLAN:return std::optional<std::string>("plan");break;
        case event_schema_id::UNIT:return std::optional<std::string>("unit");break;
        case event_schema_id::XDV:return std::optional<std::string>("xdv");break;
        case event_schema_id::IDEA:return std::optional<std::string>("idea");break;
        default:return std::nullopt;
    }
    return std::nullopt;
}

template<typename traits>
snapshot_files_t loader<traits>::get_lastest_snapshot() 
{
    std::set<std::string> done;
    snapshot_files_t snapshort_files;
    auto snapshot = databus_root_path() + "/" + snapshot_path();
    if (!std::filesystem::exists(snapshot)) {
        FATAL("Snapshot path [%s] does not exists.", snapshot.c_str());
        return snapshort_files;
    }
    try {
        for (auto& pos: fs::directory_iterator(snapshot)) {
            if (pos.is_directory()) {
                continue;
            }
            std::string fname = std::string(pos.path().filename());
            const std::regex r(snapshot_prefix() + R"((\d+)_(\w+)\.(\w+)\.(\d+))");
            if (std::smatch sm; std::regex_match(fname, sm, r)) {
                std::string version = sm[1], schema = sm[2], label = sm[3], datetime = sm[4];
                if (label == data_file_suffix) {
                    try {
                        snapshort_files[schema] = 
                            std::make_pair(
                                std::string(pos.path()), 
                                std::stoul(datetime.c_str())
                            );
                    } catch (std::exception& e) {
                        FATAL("Fail to get snapshort file %s", std::string(pos.path()).c_str());
                    }
                } else if (label == "done") {
                    done.insert(schema);
                }
            }
        }
        for (auto it = snapshort_files.begin(); it != snapshort_files.end();) {
            if (done.find(it->first) == done.end()) {
                WARNING("File [%s] is not done.", it->second.first.c_str());
                it = snapshort_files.erase(it);
            } else {
                ++it;
            }
        }
    } catch (std::exception & exp) {
        FATAL("get_lastest_snapshot fail for [%s]", exp.what());
    }
    return snapshort_files;
}


}