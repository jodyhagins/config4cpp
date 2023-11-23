add_custom_command(
    OUTPUT DefaultSecurity.cpp DefaultSecurity.h
    COMMAND config2cpp-nocheck
        -cfg ${CMAKE_CURRENT_SOURCE_DIR}/DefaultSecurity.cfg
        -class DefaultSecurity
        -namespace CONFIG4CPP_NAMESPACE
        -outdir "${CMAKE_CURRENT_BINARY_DIR}/"
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/DefaultSecurity.cfg
    VERBATIM)

add_library(config4cpp_lib

    SchemaLex.cpp
    SchemaParser.cpp
    MBChar.cpp
    SchemaValidator.cpp
    platform.cpp
    util.cpp
    Configuration.cpp
    AsciiPatternMatch.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/DefaultSecurity.cpp"
    DefaultSecurityConfiguration.cpp
    ConfigurationException.cpp
    ConfigurationImpl.cpp
    ConfigParser.cpp
    UidIdentifierProcessor.cpp
    ConfigScope.cpp
    ConfigScopeEntry.cpp
    ConfigItem.cpp
    LexToken.cpp
    LexBase.cpp
    ConfigLex.cpp
    StringBuffer.cpp
    StringVector.cpp
    SchemaType.cpp
    SchemaTypeBoolean.cpp
    SchemaTypeDurationMicroseconds.cpp
    SchemaTypeDurationMilliseconds.cpp
    SchemaTypeDurationSeconds.cpp
    SchemaTypeEnum.cpp
    SchemaTypeFloat.cpp
    SchemaTypeFloatWithUnits.cpp
    SchemaTypeInt.cpp
    SchemaTypeIntWithUnits.cpp
    SchemaTypeMemorySizeBytes.cpp
    SchemaTypeMemorySizeKB.cpp
    SchemaTypeMemorySizeMB.cpp
    SchemaTypeScope.cpp
    SchemaTypeString.cpp
    SchemaTypeUnitsWithFloat.cpp
    SchemaTypeUnitsWithInt.cpp
    SchemaTypeList.cpp
    SchemaTypeTable.cpp
    SchemaTypeTuple.cpp
    SchemaTypeTypedef.cpp)
target_include_directories(config4cpp_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(config4cpp_lib PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
set_property(TARGET config4cpp_lib PROPERTY OUTPUT_NAME config4cpp)
target_compile_options(config4cpp_lib PRIVATE -Werror -Wall -Wextra)

add_executable(config2cpp-nocheck
    config2cpp-nocheck-main.cpp
    Config2Cpp.cpp)
target_include_directories(config2cpp-nocheck PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(config4cpp
    config4cpp.cpp)
target_link_libraries(config4cpp PUBLIC config4cpp_lib)

add_executable(config2cpp
    config2cpp-main.cpp
    Config2Cpp.cpp)
target_link_libraries(config2cpp PUBLIC config4cpp_lib)
