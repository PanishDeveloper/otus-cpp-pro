cmake_minimum_required(VERSION 3.16)

set(PATCH_VERSION "1" CACHE STRING "Patch version number")
project(matrix VERSION 0.0.${PATCH_VERSION})

option(WITH_GTEST "Build with Google Test" ON)

add_executable(matrix main.cpp)

set_target_properties(matrix PROPERTIES
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

    add_executable(test_matrix test_matrix.cpp)
    target_link_libraries(test_matrix PRIVATE GTest::gtest_main)

    set_target_properties(test_matrix PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
    )

    include(GoogleTest)
    gtest_discover_tests(test_matrix)
endif()

if (MSVC)
    target_compile_options(matrix PRIVATE /W4)
    if(WITH_GTEST)
        target_compile_options(test_matrix PRIVATE /W4)
    endif()
else()
    target_compile_options(matrix PRIVATE -Wall -Wextra -pedantic -Werror)
    if(WITH_GTEST)
        target_compile_options(test_matrix PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endif()

install(TARGETS matrix RUNTIME DESTINATION bin)
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Kirill Panishev")

include(CPack)
