#pragma once
namespace galois::gdatabus {
struct ignore_event {
    typedef int update_event;
    typedef int del_event;
    typedef int custom_update_event;
    typedef int custom_del_event;
};
}