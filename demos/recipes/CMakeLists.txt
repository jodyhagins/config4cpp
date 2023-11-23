add_executable(recipes-demo
    main.cpp

    RecipeFileParser.cpp)

target_link_libraries(recipes-demo
    PUBLIC config4cpp_lib)

set_property(TARGET recipes-demo
    PROPERTY OUTPUT_NAME demo)
