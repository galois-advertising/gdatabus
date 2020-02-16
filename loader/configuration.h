#pragma once
#include <string>
#include <tuple>
namespace galois::gdatabus {
struct configuration {
    typedef std::string file_path_t;
    virtual file_path_t databus_root_path() const = 0;
    virtual file_path_t stream_path() const = 0;
    virtual std::string stream_prefix() const = 0;
    virtual file_path_t snapshot_path() const = 0;
    virtual std::string snapshot_prefix() const = 0;
};
}