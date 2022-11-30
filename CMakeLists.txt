cmake_minimum_required(VERSION 3.16)

set(PROJECT_VERSION 0.1.0)

project(SFC_Routing
    DESCRIPTION "Resolve SFC Routing problem"
    LANGUAGES CXX C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(EXECUTE_FILE_NAME SFC_Routing)


add_subdirectory(src)