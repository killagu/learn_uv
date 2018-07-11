cmake_minimum_required(VERSION 3.10)
project(uvserver C)

add_subdirectory(libuv)

set(CMAKE_C_STANDARD 11)

include_directories(http-parser)
include_directories(libuv/include)

#add_definitions(-DDEBUG=1)

add_executable(uvserver
        main.c
        http-parser/http_parser.c
        server.c
        server.h
        vector.c
        vector.h
        request.c
        request.h
        util.h
        http/header.c
        http/header.h
        )

target_link_libraries(uvserver uv)