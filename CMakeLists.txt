cmake_minimum_required(VERSION 3.16)

set(PATCH_VERSION "1" CACHE STRING "Patch version number")
project(bulk VERSION 0.0.${PATCH_VERSION})

option(WITH_GTEST "Build with Google Test" ON)

add_executable(bulk main.cpp)

set_target_properties(bulk PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

if(WIN32)
    target_link_libraries(bulk PRIVATE ws2_32)
endif()

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

    add_executable(test_bulk test_bulk.cpp)
    target_link_libraries(test_bulk PRIVATE GTest::gtest_main)

    set_target_properties(test_bulk PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
    )

    include(GoogleTest)
    gtest_discover_tests(test_bulk)
endif()

if (MSVC)
    target_compile_options(bulk PRIVATE /W4)
    if(WITH_GTEST)
        target_compile_options(test_bulk PRIVATE /W4)
    endif()
else()
    target_compile_options(bulk PRIVATE -Wall -Wextra -pedantic -Werror)
    if(WITH_GTEST)
        target_compile_options(test_bulk PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endif()

install(TARGETS bulk RUNTIME DESTINATION bin)
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "Kirill Panishev")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Kirill Panishev")

include(CPack)
