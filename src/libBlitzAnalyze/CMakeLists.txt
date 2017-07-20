set(BlitzAnalyze_VERSION_MAJOR 1)
set(BlitzAnalyze_VERSION_MINOR 0)
set(BlitzAnalyze_VERSION_PATCH 1)
set(BlitzAnalyze_VERSION ${BlitzAnalyze_VERSION_MAJOR}.${BlitzAnalyze_VERSION_MINOR}.${BlitzAnalyze_VERSION_PATCH})

set(BlitzAnalyze_HEADERS
  BlitzAnalyzeError.hh BlitzAnalyzeFile.hh BlitzAnalyzeFile.icc
  BlitzAnalyzeTraits.hh BlitzAnalyzeTraits.icc BlitzAnalyzeType.hh)
set(BlitzAnalyze_SOURCES BlitzAnalyzeTraits.cc BlitzAnalyzeFile.cc)

add_library(BlitzAnalyze ${BlitzAnalyze_SOURCES} ${BlitzAnalyze_HEADERS})
set_target_properties(BlitzAnalyze PROPERTIES
  VERSION ${BlitzAnalyze_VERSION} SOVERSION ${BlitzAnalyze_VERSION_MAJOR})

target_include_directories(BlitzAnalyze PUBLIC ${BLITZ_INCLUDE_DIRS} )
target_link_libraries(BlitzAnalyze ${BLITZ_LIBRARIES} )

# Install development headers
install(FILES ${BlitzAnalyze_HEADERS}
  DESTINATION include/${PACKAGE}/libBlitzAnalyze)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS BlitzAnalyze
    LIBRARY DESTINATION bin
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS BlitzAnalyze
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries
# are requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(BlitzAnalyze_static STATIC
    ${BlitzAnalyze_SOURCES} ${BlitzAnalyze_HEADERS})
  target_include_directories(BlitzAnalyze_static PUBLIC ${BLITZ_INCLUDE_DIRS})
  target_link_libraries(BlitzAnalyze_static ${BLITZ_LIBRARIES})
  install(TARGETS BlitzAnalyze_static ARCHIVE DESTINATION lib)

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
  target_link_libraries(BlitzAnalyze_static_tools ${BLITZ_STATIC_LIBRARIES})
endif()
