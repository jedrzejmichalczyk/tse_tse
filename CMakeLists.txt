cmake_minimum_required(VERSION 3.16)
project(tse_tse)

set(CMAKE_CXX_STANDARD 26)

include_directories("c:/git/eigen")
add_executable(tse_tse main.cpp t_tut.h)



include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()
add_executable(test_run OperationsTest.cpp t_tut.h)
target_link_libraries(test_run PUBLIC gtest_main)
include(GoogleTest)

add_executable(test_derivs DerivativesTest.cpp t_tut.h)
include(GoogleTest)
target_link_libraries(test_derivs PUBLIC gtest_main)
