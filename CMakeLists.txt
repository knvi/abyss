# make a simple CMake
cmake_minimum_required(VERSION 3.0)

project(abyss C)

# add address sanitizer
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb3 -fno-omit-frame-pointer")

add_executable(
    abyss
    src/main.c
    src/db/database.c
    src/server/server.c
    src/server/endpoint.c
    )

target_link_libraries(
    abyss
    sqlite3
    ulfius
    jansson
    )


