add_executable(log-level-demo
    main.cpp

    FooConfiguration.cpp
    Logger.cpp
    A.cpp
    B.cpp)
target_link_libraries(log-level-demo
    PRIVATE config4cpp_lib)
target_include_directories(log-level-demo
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
set_property(TARGET log-level-demo
    PROPERTY OUTPUT_NAME demo)
