add_library(BlitzAnalyze
  BlitzAnalyzeTraits.cc BlitzAnalyzeFile.cc BlitzAnalyzeError.hh
  BlitzAnalyzeFile.hh BlitzAnalyzeTraits.hh BlitzAnalyzeFile.icc
  BlitzAnalyzeTraits.icc BlitzAnalyzeType.hh )
set(BlitzAnalyze_VERSION_MAJOR 1)
set(BlitzAnalyze_VERSION_MINOR 0)
set(BlitzAnalyze_VERSION_PATCH 0)
set(BlitzAnalyze_VERSION ${BlitzAnalyze_VERSION_MAJOR}.${BlitzAnalyze_VERSION_MINOR}.${BlitzAnalyze_VERSION_PATCH})
set_target_properties(BlitzAnalyze PROPERTIES VERSION ${BlitzAnalyze_VERSION} SOVERSION ${BlitzAnalyze_VERSION_MAJOR})

target_include_directories(BlitzAnalyze INTERFACE ${BLITZ_INCLUDE_DIRS} )
target_link_libraries(BlitzAnalyze ${BLITZ_LIBRARIES} )

# Install development headers
install(DIRECTORY . DESTINATION include/libBlitzAnalyze
  FILES_MATCHING PATTERN "*.hh" PATTERN "*.icc")

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS BlitzAnalyze
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS BlitzAnalyze
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries are requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(BlitzAnalyze_static STATIC
    BlitzAnalyzeTraits.cc BlitzAnalyzeFile.cc BlitzAnalyzeError.hh
    BlitzAnalyzeFile.hh BlitzAnalyzeTraits.hh BlitzAnalyzeFile.icc
    BlitzAnalyzeTraits.icc BlitzAnalyzeType.hh )
  target_include_directories(BlitzAnalyze_static INTERFACE ${BLITZ_INCLUDE_DIRS} )
  target_link_libraries(BlitzAnalyze_static ${BLITZ_LIBRARIES} )
  install(TARGETS BlitzAnalyze_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(BlitzAnalyze_static PROPERTIES OUTPUT_NAME BlitzAnalyze CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
