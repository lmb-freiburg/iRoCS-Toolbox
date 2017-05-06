cmake_minimum_required (VERSION 2.8.11)

project (iRoCSToolbox)

option(BUILD_STATIC_LIBS "Build the static library" ON)
option(BUILD_SHARED_LIBS "Build the shared library" ON)
option(BUILD_TESTS "Build test programs" OFF)

include_directories(${iRoCSToolbox_SOURCE_DIR}/src)

add_subdirectory(src)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif(BUILD_TESTS)
