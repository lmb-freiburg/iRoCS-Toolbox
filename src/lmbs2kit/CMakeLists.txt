set(lmbs2kit_VERSION_MAJOR 1)
set(lmbs2kit_VERSION_MINOR 0)
set(lmbs2kit_VERSION_PATCH 1)
set(lmbs2kit_VERSION ${lmbs2kit_VERSION_MAJOR}.${lmbs2kit_VERSION_MINOR}.${lmbs2kit_VERSION_PATCH})

set(lmbs2kit_HEADERS
  seminaive.h FST_semi_memo.h primitive.h cospmls.h pmls.h naive_synthesis.h
  makeweights.h csecond.h)
set(lmbs2kit_SOURCES
  seminaive.c FST_semi_memo.c primitive.c cospmls.c pmls.c naive_synthesis.c
  makeweights.c csecond.c)

if (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS)
  # Install development headers
  install(FILES ${lmbs2kit_HEADERS} DESTINATION include/${PACKAGE}/lmbs2kit)
endif()

# Build shared library if requested
if (BUILD_SHARED_LIBS)
  add_library(lmbs2kit SHARED ${lmbs2kit_SOURCES} ${lmbs2kit_HEADERS})
  set_target_properties(lmbs2kit PROPERTIES
    VERSION ${lmbs2kit_VERSION} SOVERSION ${lmbs2kit_VERSION_MAJOR})
  target_include_directories(lmbs2kit
    PUBLIC ${FFTW3_INCLUDE_DIRS} ${FFTW3F_INCLUDE_DIRS})
  target_link_libraries(lmbs2kit PUBLIC ${FFTW3_LIBRARIES} ${FFTW3F_LIBRARIES})
  if(UNIX)
    target_link_libraries(lmbs2kit PUBLIC ${LIBM_LIBRARIES})
  endif()
  install(TARGETS lmbs2kit
    EXPORT iRoCS-ToolboxTargets
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/${PACKAGE}/lmbs2kit)
endif()

# Build static library if requested
if(BUILD_STATIC_LIBS)
  add_library(lmbs2kit_static STATIC ${lmbs2kit_SOURCES} ${lmbs2kit_HEADERS})
  target_include_directories(lmbs2kit_static
    PUBLIC ${FFTW3_INCLUDE_DIRS} ${FFTW3F_INCLUDE_DIRS})
  target_link_libraries(lmbs2kit_static
    PUBLIC ${FFTW3_LIBRARIES} ${FFTW3F_LIBRARIES})
  if(UNIX)
    target_link_libraries(lmbs2kit_static PUBLIC ${LIBM_LIBRARIES})
  endif()
  install(TARGETS lmbs2kit_static
    EXPORT iRoCS-ToolboxTargets
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/${PACKAGE}/lmbs2kit)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(lmbs2kit_static PROPERTIES OUTPUT_NAME lmbs2kit)
  endif()
endif()

# Explicitly add static library build for static binaries
if(BUILD_STATIC_TOOLS)
  add_library(lmbs2kit_static_tools STATIC
    ${lmbs2kit_SOURCES} ${lmbs2kit_HEADERS})
  target_include_directories(lmbs2kit_static_tools
    PUBLIC ${FFTW3_INCLUDE_DIRS} ${FFTW3F_INCLUDE_DIRS})
  target_link_libraries(lmbs2kit_static_tools
    PUBLIC ${FFTW3_STATIC_LIBRARIES} ${FFTW3F_STATIC_LIBRARIES})
  if(UNIX)
    target_link_libraries(lmbs2kit_static_tools PUBLIC ${LIBM_STATIC_LIBRARIES})
  endif()
endif()
