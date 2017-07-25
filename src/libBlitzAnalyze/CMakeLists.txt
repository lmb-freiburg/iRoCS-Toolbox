set(BlitzAnalyze_VERSION_MAJOR 1)
set(BlitzAnalyze_VERSION_MINOR 0)
set(BlitzAnalyze_VERSION_PATCH 1)
set(BlitzAnalyze_VERSION ${BlitzAnalyze_VERSION_MAJOR}.${BlitzAnalyze_VERSION_MINOR}.${BlitzAnalyze_VERSION_PATCH})

set(BlitzAnalyze_HEADERS
  BlitzAnalyzeError.hh BlitzAnalyzeFile.hh BlitzAnalyzeFile.icc
  BlitzAnalyzeTraits.hh BlitzAnalyzeTraits.icc BlitzAnalyzeType.hh)
set(BlitzAnalyze_SOURCES BlitzAnalyzeTraits.cc BlitzAnalyzeFile.cc)

if (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS)
  # Install development headers
  install(FILES ${BlitzAnalyze_HEADERS}
    DESTINATION include/${PACKAGE}/libBlitzAnalyze)
endif()

# Build shared library if requested
if (BUILD_SHARED_LIBS)
  add_library(BlitzAnalyze SHARED
    ${BlitzAnalyze_SOURCES} ${BlitzAnalyze_HEADERS})
  set_target_properties(BlitzAnalyze PROPERTIES
    VERSION ${BlitzAnalyze_VERSION} SOVERSION ${BlitzAnalyze_VERSION_MAJOR})
  target_include_directories(BlitzAnalyze PUBLIC ${BLITZ_INCLUDE_DIRS})
  target_link_libraries(BlitzAnalyze PUBLIC ${BLITZ_LIBRARIES})
  install(TARGETS BlitzAnalyze
    EXPORT iRoCS-ToolboxTargets
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/${PACKAGE}/libBlitzAnalyze)
endif()

# Build static library if requested
if(BUILD_STATIC_LIBS)
  add_library(BlitzAnalyze_static STATIC
    ${BlitzAnalyze_SOURCES} ${BlitzAnalyze_HEADERS})
  target_include_directories(BlitzAnalyze_static PUBLIC ${BLITZ_INCLUDE_DIRS})
  target_link_libraries(BlitzAnalyze_static PUBLIC ${BLITZ_LIBRARIES})
  install(TARGETS BlitzAnalyze_static
    EXPORT iRoCS-ToolboxTargets
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/${PACKAGE}/libBlitzAnalyze)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(BlitzAnalyze_static PROPERTIES
      OUTPUT_NAME BlitzAnalyze)
  endif()
endif()

# Explicitly add static library build for static binaries
if(BUILD_STATIC_TOOLS)
  add_library(BlitzAnalyze_static_tools STATIC
    ${BlitzAnalyze_SOURCES} ${BlitzAnalyze_HEADERS})
  target_include_directories(BlitzAnalyze_static_tools
    PUBLIC ${BLITZ_INCLUDE_DIRS})
  target_link_libraries(BlitzAnalyze_static_tools
    PUBLIC ${BLITZ_STATIC_LIBRARIES})
endif()
