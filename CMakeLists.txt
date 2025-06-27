cmake_minimum_required(VERSION 3.10)
project(hash_table_project C)

set(CMAKE_C_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

include_directories(${PROJECT_SOURCE_DIR}/src)

add_compile_options(-Wall -Wextra -Werror)

set(CMAKE_C_FLAGS_DEBUG "-g -O0")

set(CMAKE_C_FLAGS_RELEASE "-O3")

add_library(hashtable STATIC
    src/hash_table.c
)

add_library(prime STATIC
    src/prime.c
)

target_link_libraries(hashtable prime m)

add_executable(main
    src/main.c
)

target_link_libraries(main hashtable prime)
