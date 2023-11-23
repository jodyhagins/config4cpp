cmake_minimum_required(VERSION 3.1...3.27)

project(config4cpp
    VERSION 1.2
    DESCRIPTION "Config4Cpp from http://www.config4star.org"
    LANGUAGES CXX)

include(CTest)

# set(CMAKE_CXX_STANDARD "20")

add_subdirectory(src)
add_subdirectory(demos)
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()
