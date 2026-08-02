cmake_minimum_required(VERSION 3.16)

set(PATCH_VERSION "1" CACHE STRING "Patch version number")
project(helloworld VERSION 0.0.${PATCH_VERSION})

option(WITH_GTEST "Build with Google Test" ON)

configure_file(version.h.in version.h)

add_executable(helloworld_cli main.cpp)
add_library(helloworld lib.cpp)

set_target_properties(helloworld_cli helloworld PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

target_include_directories(helloworld PUBLIC "${CMAKE_BINARY_DIR}")
target_include_directories(helloworld_cli PUBLIC "${CMAKE_BINARY_DIR}")

target_link_libraries(helloworld_cli PRIVATE helloworld)

if(WITH_GTEST)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
            URL_HASH SHA256=65fab701d9829d38cb77c14acdc431d2108bfdbf8979e40eb8ae567edf10b27c
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(test_version test_version.cpp)
    target_compile_definitions(test_version PRIVATE PATCH_VERSION=${PATCH_VERSION})
    target_link_libraries(test_version GTest::gtest_main helloworld)

    set_target_properties(test_version PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
    )

    include(GoogleTest)
    gtest_discover_tests(test_version)
endif()

if (MSVC)
    target_compile_options(helloworld_cli PRIVATE /W4)
    target_compile_options(helloworld PRIVATE /W4)
    if(WITH_GTEST)
        target_compile_options(test_version PRIVATE /W4)
    endif()
else()
    target_compile_options(helloworld_cli PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(helloworld PRIVATE -Wall -Wextra -pedantic -Werror)
    if(WITH_GTEST)
        target_compile_options(test_version PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endif()

include(CPack)
