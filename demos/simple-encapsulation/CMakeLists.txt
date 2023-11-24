add_custom_command(
    OUTPUT FallbackConfiguration.cpp FallbackConfiguration.h
    COMMAND config2cpp
        -cfg ${CMAKE_CURRENT_SOURCE_DIR}/FallbackConfiguration.cfg
        -class FallbackConfiguration
        -singleton
        -outdir "${CMAKE_CURRENT_BINARY_DIR}/"
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/FallbackConfiguration.cfg
    VERBATIM)

add_executable(simple-encapsulation-demo
    main.cpp

    FooConfiguration.cpp
    FallbackConfiguration.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/FallbackConfiguration.cpp")
target_link_libraries(simple-encapsulation-demo
    PRIVATE config4cpp_lib)
target_include_directories(simple-encapsulation-demo
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
set_property(TARGET simple-encapsulation-demo
    PROPERTY OUTPUT_NAME demo)
