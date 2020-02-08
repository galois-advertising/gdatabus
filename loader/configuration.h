#pragma once
#include <string>
namespace galois::gdatabus {
struct configuration {
    virtual std::string batch_root_path() = 0;
    virtual std::string stream_root_path() = 0;
};
}