cmake_minimum_required(VERSION 3.15...3.27)

# The original version that this repo is based on was downloaded from
# www.config4star.org, and had the following in the VERSION.txt file.
#
# Version 1.2
# 30 September 2021
#
# There is no real repository that I can find that I can fork, and I have no
# idea if/when other releases will be made.  I want to use this project, and
# I'll maintain my own version, hopefully with minimal changes to the existing
# source code files.  I could have made almost none, if I were willing to
# ignore the compiler warnings.
#
# I don't think I want to keep the version number the same, and I'm not sure
# how I want to proceed.  So, I think I'll do something that, while a bit
# unconventional, should be distinct and understandable.  The release 1.2
# will become 102.x.y.  If another release is provided by the original author,
# say 1.3, and I move to it, then I'll make that one be 103.x.y.
project(config4cpp
    VERSION 102.0
    DESCRIPTION "Config4Cpp from http://www.config4star.org"
    LANGUAGES CXX)

add_subdirectory(src)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    option(CONFIG4CPP_BUILD_DEMOS "whether or not demos should be built" ON)
    option(CONFIG4CPP_BUILD_TESTS "whether or not tests should be built" ON)

    if (CONFIG4CPP_BUILD_DEMOS)
        add_subdirectory(demos)
    endif()

    if (CONFIG4CPP_BUILD_TESTS)
        enable_testing()
        add_subdirectory(tests)
    endif()
endif()
