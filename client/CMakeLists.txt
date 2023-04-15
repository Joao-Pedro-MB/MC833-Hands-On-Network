cmake_minimum_required(VERSION 3.10)

project(Server/Client-TCP)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(cJSON STATIC lib/cJSON.c)

target_include_directories(cJSON PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Set output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# Add source files for server executable
add_executable(server
    src/server/server.c
    src/server/server-socket.c
)

# Add source files for client executable
add_executable(client
    src/client/client.c
    src/client/client-socket.c
)

# Link cJSON library to the executables
target_link_libraries(server cJSON)
target_link_libraries(client cJSON)
