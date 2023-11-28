add_executable(ConfigScope_ut
    ConfigScope_ut.cpp)

target_link_libraries(ConfigScope_ut
    PRIVATE config4cpp_lib)
target_include_directories(ConfigScope_ut
    PRIVATE "${PROJECT_SOURCE_DIR}")

add_test(NAME "ConfigScope Tests"
    COMMAND ConfigScope_ut)
