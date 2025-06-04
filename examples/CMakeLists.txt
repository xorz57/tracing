add_executable(factorial factorial.cpp)
target_compile_features(factorial PRIVATE cxx_std_11)
target_link_libraries(factorial PRIVATE ${PROJECT_NAME})

add_executable(fibonacci fibonacci.cpp)
target_compile_features(fibonacci PRIVATE cxx_std_11)
target_link_libraries(fibonacci PRIVATE ${PROJECT_NAME})

include(StrictWarnings)

enable_strict_warnings(factorial)
enable_strict_warnings(fibonacci)
