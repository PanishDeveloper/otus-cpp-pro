cmake_minimum_required(VERSION 3.16)

set(PATCH_VERSION "1" CACHE STRING "Patch version number")
project(bayan VERSION 0.0.${PATCH_VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Boost QUIET CONFIG COMPONENTS filesystem program_options regex)
if(NOT Boost_FOUND)
    find_package(Boost REQUIRED COMPONENTS filesystem program_options regex)
endif()

option(WITH_GTEST "Build with Google Test" ON)


add_executable(bayan src/main.cpp)
target_include_directories(bayan PRIVATE include)
target_link_libraries(bayan PRIVATE
        Boost::filesystem
        Boost::program_options
        Boost::regex
)


if(WITH_GTEST)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.17.0
            GIT_SHALLOW ON
    )
    
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(test_bayan tests/test_bayan.cpp)
    target_include_directories(test_bayan PRIVATE include)
    target_link_libraries(test_bayan PRIVATE
            GTest::gtest_main
            Boost::filesystem
            Boost::regex
    )

    include(GoogleTest)
    gtest_discover_tests(test_bayan)
endif()


if (MSVC)
    target_compile_options(bayan PRIVATE /W4)
    if(WITH_GTEST)
        target_compile_options(test_bayan PRIVATE /W4)
    endif()
else()
    target_compile_options(bayan PRIVATE -Wall -Wextra -pedantic -Werror)
    if(WITH_GTEST)
        target_compile_options(test_bayan PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endif()


install(TARGETS bayan RUNTIME DESTINATION bin)
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "Kirill Panishev")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Kirill Panishev")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)