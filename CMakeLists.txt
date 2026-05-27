cmake_minimum_required(VERSION 4.2)
project(tasktwo C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

include_directories(include)

file(GLOB SOURCES "src/*.c")

add_executable(cmatch ${SOURCES})

target_link_libraries(cmatch pthread)
