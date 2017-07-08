add_library(lmbs2kit
  seminaive.c FST_semi_memo.c primitive.c cospmls.c pmls.c naive_synthesis.c
  makeweights.c csecond.c seminaive.h FST_semi_memo.h primitive.h cospmls.h
  pmls.h naive_synthesis.h makeweights.h csecond.h )
set(lmbs2kit_VERSION_MAJOR 1)
set(lmbs2kit_VERSION_MINOR 0)
set(lmbs2kit_VERSION_PATCH 0)
set(lmbs2kit_VERSION ${lmbs2kit_VERSION_MAJOR}.${lmbs2kit_VERSION_MINOR}.${lmbs2kit_VERSION_PATCH})
set_target_properties(lmbs2kit PROPERTIES VERSION ${lmbs2kit_VERSION} SOVERSION ${lmbs2kit_VERSION_MAJOR})

target_include_directories(lmbs2kit INTERFACE
  ${FFTW3_INCLUDE_DIRS} ${FFTW3F_INCLUDE_DIRS} )
target_link_libraries(lmbs2kit ${FFTW3_LIBRARIES} ${FFTW3F_LIBRARIES} )

if(UNIX)
  target_link_libraries(lmbs2kit m)
endif()

# Install development headers
install(DIRECTORY . DESTINATION include/lmbs2kit FILES_MATCHING PATTERN "*.h" )

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS lmbs2kit
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS lmbs2kit
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries
# are requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(lmbs2kit_static STATIC
    seminaive.c FST_semi_memo.c primitive.c cospmls.c pmls.c naive_synthesis.c
    makeweights.c csecond.c seminaive.h FST_semi_memo.h primitive.h cospmls.h
    pmls.h naive_synthesis.h makeweights.h csecond.h )
  target_include_directories(lmbs2kit_static INTERFACE
    ${FFTW3_INCLUDE_DIRS} ${FFTW3F_INCLUDE_DIRS} )
  target_link_libraries(lmbs2kit_static ${FFTW3_LIBRARIES} ${FFTW3F_LIBRARIES} )

  if(UNIX)
    target_link_libraries(lmbs2kit_static m)
  endif()

  install(TARGETS lmbs2kit_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(lmbs2kit_static PROPERTIES
      OUTPUT_NAME lmbs2kit CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)

endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
