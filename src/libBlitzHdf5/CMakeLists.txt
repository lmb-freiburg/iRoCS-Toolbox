set(BlitzHdf5_VERSION_MAJOR 4)
set(BlitzHdf5_VERSION_MINOR 0)
set(BlitzHdf5_VERSION_PATCH 0)
set(BlitzHdf5_VERSION ${BlitzHdf5_VERSION_MAJOR}.${BlitzHdf5_VERSION_MINOR}.${BlitzHdf5_VERSION_PATCH})

set(BlitzHdf5_HEADERS
  BlitzH5Traits.hh BlitzHdf5Light.hh BlitzHdf5Light.icc
  FileIOWrapper.hh FileIOWrapper.icc)
set(BlitzHdf5_SOURCES BlitzH5Traits.cc BlitzHdf5Light.cc)

if (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS)
  # Install development headers
  install(FILES ${BlitzHdf5_HEADERS}
    DESTINATION include/${PACKAGE}/libBlitzHdf5)
endif()

# Build shared library if requested
if (BUILD_SHARED_LIBS)
  add_library(BlitzHdf5 SHARED ${BlitzHdf5_SOURCES} ${BlitzHdf5_HEADERS})
  set_target_properties(BlitzHdf5 PROPERTIES
    VERSION ${BlitzHdf5_VERSION} SOVERSION ${BlitzHdf5_VERSION_MAJOR})
  target_include_directories(BlitzHdf5
    PUBLIC ${BLITZ_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS})
  target_link_libraries(BlitzHdf5
    PUBLIC ${BLITZ_LIBRARIES} ${HDF5_C_LIBRARIES} ProgressReporter)
  install(TARGETS BlitzHdf5
    EXPORT iRoCS-ToolboxTargets
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/${PACKAGE}/libBlitzHdf5)
endif()

# Build static library if requested
if(BUILD_STATIC_LIBS)
  add_library(BlitzHdf5_static STATIC ${BlitzHdf5_SOURCES} ${BlitzHdf5_HEADERS})
  target_include_directories(BlitzHdf5_static
    PUBLIC ${BLITZ_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS})
  target_link_libraries(BlitzHdf5_static
    PUBLIC ${BLITZ_LIBRARIES} ${HDF5_C_LIBRARIES} ProgressReporter_static)
  install(TARGETS BlitzHdf5_static
    EXPORT iRoCS-ToolboxTargets
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/${PACKAGE}/libBlitzHdf5)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(BlitzHdf5_static PROPERTIES OUTPUT_NAME BlitzHdf5)
  endif()
endif()

# Explicitly add static library build for static binaries
if(BUILD_STATIC_TOOLS)
  add_library(BlitzHdf5_static_tools STATIC
    ${BlitzHdf5_SOURCES} ${BlitzHdf5_HEADERS})
  target_include_directories(BlitzHdf5_static_tools
    PUBLIC ${BLITZ_INCLUDE_DIRS} ${HDF5_STATIC_C_INCLUDE_DIRS})
  target_link_libraries(BlitzHdf5_static_tools
    PUBLIC ${BLITZ_STATIC_LIBRARIES} ${HDF5_STATIC_C_LIBRARIES}
    ProgressReporter_static_tools)
  add_dependencies(BlitzHdf5_static_tools hdf5_static)
endif()
