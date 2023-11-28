add_executable(ConfigScope_ut
    ConfigScope_ut.cpp)

target_link_libraries(ConfigScope_ut
    PRIVATE config4cpp_lib)
target_include_directories(ConfigScope_ut
    PRIVATE "${PROJECT_SOURCE_DIR}")

add_test(NAME "ConfigScope Tests"
    COMMAND ConfigScope_ut)


add_executable(Configuration_ut
    Configuration_ut.cpp)

target_link_libraries(Configuration_ut
    PRIVATE config4cpp_lib)
target_include_directories(Configuration_ut
    PRIVATE "${PROJECT_SOURCE_DIR}")

add_test(NAME "Configuration Tests"
    COMMAND Configuration_ut)
