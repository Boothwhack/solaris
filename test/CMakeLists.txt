find_package(Catch2)

add_executable(test source/ecs_test.cpp
        source/runtime_object_tests.cpp
        source/runtime_struct_tests.cpp
        source/runtime_vector_tests.cpp
        source/test_components.hpp)
target_link_libraries(test PRIVATE solaris Catch2::Catch2WithMain)
