add_executable(test-schema-types
    main.cpp)

target_link_libraries(test-schema-types
    PRIVATE config4cpp_lib)

add_test(NAME "Schema Types Test"
    COMMAND test-schema-types
        -cfg ${CMAKE_CURRENT_SOURCE_DIR}/schema-type-tests.cfg)
