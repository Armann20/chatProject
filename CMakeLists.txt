cmake_minimum_required(VERSION 3.16)
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)
project(Chat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the path to the vspkg.json file
set(VSPKG_JSON_PATH "${CMAKE_SOURCE_DIR}/vcpkg.json")

# Include the vcpkg toolchain file
if(EXISTS "${VSPKG_JSON_PATH}")
    include("${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

find_program(CMAKE_CXX_CLANG_TIDY_EXECUTABLE NAMES clang-tidy)
if (NOT CMAKE_CXX_CLANG_TIDY_EXECUTABLE)
  message("clang-tidy not found")
else()
  message("clang-tidy found")
  set(CMAKE_CXX_CLANG_TIDY ${CMAKE_CXX_CLANG_TIDY_EXECUTABLE})
endif()

find_package(Boost REQUIRED COMPONENTS system thread)
add_executable(server server.cpp)
target_link_libraries(server
 PRIVATE
 Boost::system
 Boost::thread
 )

 add_executable(client client.cpp)
 target_link_libraries(client
 PRIVATE
 Boost::system
 )
