PROJECT(gdatabus)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-Bsymbolic")
MESSAGE("CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -std=c++17 -fno-strict-aliasing -Wno-register")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -std=c++17 -fno-strict-aliasing -Wno-register")
SET(CMAKE_BUILD_TYPE "Release")
#set(CMAKE_BUILD_TYPE "Debug")

find_package(Boost REQUIRED COMPONENTS log)
if(NOT Boost_FOUND)
    message("Can not find Boost")
endif()
message(STATUS "Found Boost IncPath ${Boost_INCLUDE_DIRS}")
message(STATUS "Found Boost LibPath ${Boost_LIBRARY_DIRS}")
message(STATUS "Found Boost LIBRARIES ${Boost_LIBRARIES}")

add_definitions(-DBOOST_SPIRIT_THREADSAFE)
add_definitions(-DBOOST_ALL_DYN_LINK)
#add_definitions(-D_DEBUG)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/common/util/include")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/common/include")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/common/util/thread-pool")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/gformat/include")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/loader")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}/common")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}/loader")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}/common/dbschema")
FIND_PACKAGE(Protobuf REQUIRED)
INCLUDE_DIRECTORIES(${Protobuf_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

EXECUTE_PROCESS(COMMAND protoc 
    -I=${CMAKE_SOURCE_DIR}
    --cpp_out=${PROJECT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/common/dbschema/freyja/databus_event.proto)

EXECUTE_PROCESS(COMMAND protoc 
    -I=${CMAKE_SOURCE_DIR}
    --cpp_out=${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/loader/ignore.proto)

SET(LOADER_SRCS
"${CMAKE_CURRENT_BINARY_DIR}/common/dbschema/freyja/databus_event.pb.cc"
"${CMAKE_CURRENT_BINARY_DIR}/loader/ignore.pb.cc"
"${PROJECT_SOURCE_DIR}/test/loader_main.cpp" 
"${CMAKE_SOURCE_DIR}/common/util/src/log.cpp"
"${CMAKE_SOURCE_DIR}/gformat/src/checksum.cpp"
)

ADD_EXECUTABLE(gdatabus_test_loader ${LOADER_SRCS})
target_link_libraries(gdatabus_test_loader ${Protobuf_LIBRARIES} boost_log-mt)

SET(GENERATOR_SRCS
"${CMAKE_CURRENT_BINARY_DIR}/common/dbschema/freyja/databus_event.pb.cc"
"${PROJECT_SOURCE_DIR}/test/generator_main.cpp" 
"${CMAKE_SOURCE_DIR}/common/util/src/log.cpp"
"${CMAKE_SOURCE_DIR}/gformat/src/checksum.cpp"
)

ADD_EXECUTABLE(gdatabus_test_generator ${GENERATOR_SRCS})
target_link_libraries(gdatabus_test_generator ${Protobuf_LIBRARIES} boost_log-mt)
