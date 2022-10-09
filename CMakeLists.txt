cmake_minimum_required(VERSION 3.2)
project(obooksrv)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")

# Begin GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
# End GoogleTest

# add pthread for httplib
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

file(GLOB_RECURSE LIB_SOURCE_FILES src/lib/*.cpp)
file(GLOB_RECURSE SOURCE_FILES src/main/*.cpp)

include_directories(src/lib/include)

add_executable(obooksrv ${LIB_SOURCE_FILES} ${SOURCE_FILES})
IF (WIN32)
  target_link_libraries(obooksrv PRIVATE Threads::Threads "gdi32" "wsock32" "ws2_32")
ELSE()
  target_link_libraries(obooksrv PRIVATE Threads::Threads)
ENDIF()

file(GLOB_RECURSE TEST_SOURCE_FILES src/test/*.cpp)

enable_testing()
add_executable(unittests ${LIB_SOURCE_FILES} ${TEST_SOURCE_FILES})
target_link_libraries(unittests GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(unittests)
