cmake_minimum_required(VERSION 3.16)

set(PATCH_VERSION "1" CACHE STRING "Patch version number")
project(allocator_demo VERSION 0.0.${PATCH_VERSION})

option(WITH_GTEST "Build with Google Test" ON)

add_executable(allocator_demo main.cpp)

set_target_properties(allocator_demo PROPERTIES
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

    add_executable(test_allocator test_allocator.cpp)
    target_link_libraries(test_allocator PRIVATE GTest::gtest_main)

    set_target_properties(test_allocator PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
    )

    include(GoogleTest)
    gtest_discover_tests(test_allocator)
endif()

if (MSVC)
    target_compile_options(allocator_demo PRIVATE /W4)
    if(WITH_GTEST)
        target_compile_options(test_allocator PRIVATE /W4)
    endif()
else()
    target_compile_options(allocator_demo PRIVATE -Wall -Wextra -pedantic -Werror)
    if(WITH_GTEST)
        target_compile_options(test_allocator PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endif()

install(TARGETS allocator_demo RUNTIME DESTINATION bin)
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Kirill Panishev")

include(CPack)
