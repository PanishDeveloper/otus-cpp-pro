cmake_minimum_required(VERSION 3.16)

set(PATCH_VERSION "1" CACHE STRING "Patch version number")
project(ip_filter VERSION 0.0.${PATCH_VERSION})

option(WITH_GTEST "Build with Google Test" ON)

add_executable(ip_filter main.cpp)
add_library(ip_filter_lib ip_filter.cpp)

set_target_properties(ip_filter ip_filter_lib PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

target_link_libraries(ip_filter PRIVATE ip_filter_lib)

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

    add_executable(test_ip_filter test_ip_filter.cpp)
    target_link_libraries(test_ip_filter PRIVATE ip_filter_lib GTest::gtest_main)

    set_target_properties(test_ip_filter PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
    )

    include(GoogleTest)
    gtest_discover_tests(test_ip_filter)
endif()

if (MSVC)
    target_compile_options(ip_filter PRIVATE /W4)
    target_compile_options(ip_filter_lib PRIVATE /W4)
    if(WITH_GTEST)
        target_compile_options(test_ip_filter PRIVATE /W4)
    endif()
else()
    target_compile_options(ip_filter PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(ip_filter_lib PRIVATE -Wall -Wextra -pedantic -Werror)
    if(WITH_GTEST)
        target_compile_options(test_ip_filter PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endif()

install(TARGETS ip_filter RUNTIME DESTINATION bin)
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Kirill Panishev")

include(CPack)
