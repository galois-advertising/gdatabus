#include <string>
#include <iostream>
#include <vector>
#include "pack_header.h"
#include "dbschema/freyja/databus_event.pb.h"
#include "log.h"
#include "pb_file_writer.h"


struct user_data_t {
    uint32_t user_id;
    uint32_t user_stat;
    std::string region;
};

std::vector<user_data_t> users{
    {1,11,"beijing"},
    {2,22,"shanghai"},
    {3,13,"guangzhou"},
};


int main(int argc, char* argv[])
{
    if (argc != 2) {
        FATAL("./%s outputfile", argv[0]);
        return -1;
    }
    INFO("[%s]", argv[1]);
    std::shared_ptr<FILE> outfile(fopen(argv[1], "wb"), [](FILE* f){if(f)fclose(f);});
    if (outfile == nullptr) {
        FATAL("open file[%s] failed.", argv[1]);
        return -1;
    }
    const size_t MAX_SIZE = 1024 * 1024;
    char buf[MAX_SIZE];
    galois::gformat::pb_file_writer<galois::gformat::pack_header_t> writer;
    size_t cnt = 0;
    for (auto & user : users) {
        galois::gformat::pack_header_t header; 
        galois::freyja::user_event user_event;
        user_event.set_id(cnt++);
        user_event.set_type(galois::freyja::event_type::update);
        user_event.mutable_update()->mutable_key()->set_user_id(user.user_id);
        user_event.mutable_update()->set_user_stat(user.user_stat);
        user_event.mutable_update()->set_region(user.region);
        auto size = user_event.SerializeToArray(buf, MAX_SIZE);
        writer.write_record(header, buf, user_event.ByteSizeLong(), outfile.get());
        galois::freyja::user_event user_event_2;
        user_event_2.ParseFromArray(buf, user_event.ByteSizeLong());
        std::cout<<"size:"<<size<<"|"<<user_event_2.id()
            <<":"<<user_event_2.update().key().user_id()
            <<":"<<user_event_2.update().region()
            <<"     "<<user_event.ByteSizeLong()
            <<std::endl;
    }
    return 0;   
}