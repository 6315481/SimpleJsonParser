include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(
    unittests
    ${CMAKE_CURRENT_SOURCE_DIR}/unittests/test_value.cpp
)
target_include_directories(unittests PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(
    unittests
    PUBLIC GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unittests)