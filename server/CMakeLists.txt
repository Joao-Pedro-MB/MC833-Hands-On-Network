cmake_minimum_required(VERSION 3.10)

project(Server/server-TCP)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(cJSON STATIC lib/cJSON.c)

target_include_directories(cJSON PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Set output directory for executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)


# Add source files for server executable
add_executable(server
    src/server/server.c
    src/server/server-socket.c
)

# Link cJSON library to the executable
target_link_libraries(server cJSON)
