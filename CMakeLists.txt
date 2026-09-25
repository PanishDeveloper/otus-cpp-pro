cmake_minimum_required(VERSION 3.16)

set(PATCH_VERSION "1" CACHE STRING "Patch version number")
project(editor VERSION 0.0.${PATCH_VERSION})

option(WITH_GTEST "Build with Google Test" ON)

add_executable(editor main.cpp)

set_target_properties(editor PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

if(WITH_GTEST)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.17.0
            GIT_SHALLOW ON
    )
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(test_editor test_editor.cpp)
    target_link_libraries(test_editor PRIVATE GTest::gtest_main)

    set_target_properties(test_editor PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
    )

    include(GoogleTest)
    gtest_discover_tests(test_editor)
endif()

if (MSVC)
    target_compile_options(editor PRIVATE /W4)
    if(WITH_GTEST)
        target_compile_options(test_editor PRIVATE /W4)
    endif()
else()
    target_compile_options(editor PRIVATE -Wall -Wextra -pedantic -Werror)
    if(WITH_GTEST)
        target_compile_options(test_editor PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endif()

install(TARGETS editor RUNTIME DESTINATION bin)
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Kirill Panishev")

include(CPack)
