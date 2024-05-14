include(ExternalProject)
include(FetchContent)

FetchContent_Declare(glfw3
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 3.4
        GIT_SHALLOW

        OVERRIDE_FIND_PACKAGE
)

set(GLFW_BUILD_WAYLAND true)
set(GLFW_BUILD_X11 true)
set(GLFW_BUILD_DOCS false)

FetchContent_Declare(Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2
        GIT_TAG v3.5.3
        GIT_SHALLOW v2.4.11
        OVERRIDE_FIND_PACKAGE
)

FetchContent_Declare(glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad
        GIT_TAG v2.0.6
        GIT_SHALLOW
        SOURCE_SUBDIR  cmake
        OVERRIDE_FIND_PACKAGE
)
