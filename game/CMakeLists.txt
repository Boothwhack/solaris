add_executable(game
        source/game.cpp
        source/game.hpp
        source/graphics.cpp
        source/graphics.hpp
        source/main.cpp
        source/window.cpp
        source/window.hpp
)

find_package(GLFW3 REQUIRED)
find_package(GLAD REQUIRED)

glad_add_library(glad SHARED API gl:core=4.4)

target_link_libraries(game PRIVATE solaris glfw glad dl)
target_compile_features(game PUBLIC cxx_std_23)
