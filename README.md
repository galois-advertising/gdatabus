gDatabus

# What's gDatabus

gDatabus is a large-scale real-time data transmission library. It is especially suitable for the transmission of advertising data in online advertising system. Due to the high response speed of `Ad Retrieval System`, data is usually synchronized from disk database(such as MySQL) to memory database(gTable) of `retrieval end`. This is just what gdatabus does.

# How does it works

gDatabus synchronizes data through formatted files. There are two kind of data flow: `snapshot` and `stream`. `Snapshot` is the total data exported from the database, which is generated once or twice a day. `Stream` is a real-time data flow for transmit incremental data, which generally generates a file every five to ten minutes.

In engineering applications, we need to adapt the CDC module of database, such as [debezium](https://github.com/debezium/debezium) to generate `stream` flow.

# How to use

Compilation dependency:

* A C++17 compiler(such as GCC 8.3.0)
* Protobuf3

To run demo:

```
$ git clone https://github.com/galois-advertising/gdatabus
$ cd gdatabus
$ git submodule init
$ git submodule update
$ mkdir build & cd build
$ cmake ../ & make
$ ./gdatabus_test_generator ../test/shared/data/GALOIS/snapshot/snapshot_GALOIS_DATA_0_user.data.202002092030
$ ./gdatabus_test_loader 
```

For more details, see [http://www.galois-advertising.com](http://www.galois-advertising.com)

# How to contribute

Pull request and contacts solopointer1202@gmail.com
