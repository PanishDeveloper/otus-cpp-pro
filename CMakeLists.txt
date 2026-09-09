cmake_minimum_required(VERSION 3.16)

set(PATCH_VERSION "1" CACHE STRING "Patch version number")
project(print_ip VERSION 0.0.${PATCH_VERSION})

option(WITH_GTEST "Build with Google Test" ON)

add_executable(print_ip main.cpp)

set_target_properties(print_ip PROPERTIES
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

    add_executable(test_print_ip test_print_ip.cpp)
    target_link_libraries(test_print_ip PRIVATE GTest::gtest_main)

    set_target_properties(test_print_ip PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
    )

    include(GoogleTest)
    gtest_discover_tests(test_print_ip)
endif()

if (MSVC)
    target_compile_options(print_ip PRIVATE /W4)
    if(WITH_GTEST)
        target_compile_options(test_print_ip PRIVATE /W4)
    endif()
else()
    target_compile_options(print_ip PRIVATE -Wall -Wextra -pedantic -Werror)
    if(WITH_GTEST)
        target_compile_options(test_print_ip PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endif()

install(TARGETS print_ip RUNTIME DESTINATION bin)
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Kirill Panishev")

include(CPack)
