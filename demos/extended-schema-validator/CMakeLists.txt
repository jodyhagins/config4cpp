add_executable(extended-schema-validator-demo
    main.cpp

    FooConfiguration.cpp
    SchemaTypeHex.cpp)
target_link_libraries(extended-schema-validator-demo
    PUBLIC config4cpp_lib)
target_include_directories(extended-schema-validator-demo
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
set_property(TARGET extended-schema-validator-demo
    PROPERTY OUTPUT_NAME demo)
