add_library(solaris source/solaris.cpp
        include/solaris/core/bus.hpp
        include/solaris/core/dispatcher.hpp
        include/solaris/core/layer.hpp
        include/solaris/core/layer_stack.hpp
        include/solaris/core/queue.hpp
        include/solaris/framework/allocation.hpp
        include/solaris/framework/resources.hpp
        include/solaris/framework/runtime_object.hpp
        include/solaris/framework/runtime_struct.hpp
        include/solaris/framework/runtime_vector.hpp
)

target_include_directories(solaris PUBLIC include)
target_compile_features(solaris PUBLIC cxx_std_20)
