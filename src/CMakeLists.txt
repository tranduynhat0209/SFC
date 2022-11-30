add_executable(${EXECUTE_FILE_NAME} main.cpp)
file(GLOB_RECURSE SOURCE_FILES *.cpp)
list(FILTER SOURCE_FILES EXCLUDE REGEX ".*\.main.cpp$")

target_link_libraries(${EXECUTE_FILE_NAME} PUBLIC ${SOURCE_FILES})
