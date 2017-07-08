add_library(BlitzHdf5
  BlitzH5Traits.cc BlitzHdf5Light.cc BlitzH5Traits.hh BlitzHdf5Light.hh
  BlitzHdf5Light.icc FileIOWrapper.hh FileIOWrapper.icc )
set(BlitzHdf5_VERSION_MAJOR 3)
set(BlitzHdf5_VERSION_MINOR 0)
set(BlitzHdf5_VERSION_PATCH 2)
set(BlitzHdf5_VERSION ${BlitzHdf5_VERSION_MAJOR}.${BlitzHdf5_VERSION_MINOR}.${BlitzHdf5_VERSION_PATCH})
set_target_properties(BlitzHdf5 PROPERTIES VERSION ${BlitzHdf5_VERSION} SOVERSION ${BlitzHdf5_VERSION_MAJOR})

target_include_directories(BlitzHdf5
  PUBLIC ${BLITZ_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS} )
target_link_libraries(BlitzHdf5
  ${BLITZ_LIBRARIES} ${HDF5_LIBRARIES} ProgressReporter)

# Install development headers
install(DIRECTORY . DESTINATION include/libBlitzHdf5
  FILES_MATCHING PATTERN "*.hh" PATTERN "*.icc")

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS BlitzHdf5
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS BlitzHdf5
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries are requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(BlitzHdf5_static STATIC
    BlitzH5Traits.cc BlitzHdf5Light.cc BlitzH5Traits.hh BlitzHdf5Light.hh
    BlitzHdf5Light.icc FileIOWrapper.hh FileIOWrapper.icc )
  target_include_directories(BlitzHdf5_static
    PUBLIC ${BLITZ_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS} )
  target_link_libraries(BlitzHdf5_static
    ${BLITZ_LIBRARIES} ${HDF5_LIBRARIES} ProgressReporter )
  install(TARGETS BlitzHdf5_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(BlitzHdf5_static PROPERTIES OUTPUT_NAME BlitzHdf5 CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
