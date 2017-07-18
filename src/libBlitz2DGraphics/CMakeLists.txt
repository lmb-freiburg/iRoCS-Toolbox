set(Blitz2DGraphics_VERSION_MAJOR 1)
set(Blitz2DGraphics_VERSION_MINOR 1)
set(Blitz2DGraphics_VERSION_PATCH 0)
set(Blitz2DGraphics_VERSION ${Blitz2DGraphics_VERSION_MAJOR}.${Blitz2DGraphics_VERSION_MINOR}.${Blitz2DGraphics_VERSION_PATCH})

set(Blitz2DGraphics_HEADERS
  Blitz2DGraphics.hh ImageAccessWrapper.hh ImageTools.hh ImageTools.icc
  BlitzImageReader.hh BlitzImageWriter.hh ColorConvert.hh BlitzRGBColor.hh
  Blitz2Ddraw.hh Blitz2Ddraw.icc Blitz2Dgeometry.hh Blitz2Dgeometry.icc)
set(Blitz2DGraphics_SOURCES
  ImageAccessWrapper.cc BlitzImageReader.cc BlitzImageWriter.cc
  BlitzRGBColor.cc ColorConvert.cc)

add_library(Blitz2DGraphics
  ${Blitz2DGraphics_SOURCES} ${Blitz2DGraphics_HEADERS})
set_target_properties(Blitz2DGraphics PROPERTIES
  VERSION ${Blitz2DGraphics_VERSION} SOVERSION ${Blitz2DGraphics_VERSION_MAJOR})
target_include_directories(Blitz2DGraphics PUBLIC
  ${BLITZ_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS} ${TIFF_INCLUDE_DIRS}
  ${JPEG_INCLUDE_DIRS} )
target_link_libraries(Blitz2DGraphics
  ${BLITZ_LIBRARIES} ${PNG_LIBRARIES} ${TIFF_LIBRARIES} ${JPEG_LIBRARIES} )

# Install development headers
install(FILES ${Blitz2DGraphics_HEADERS}
  DESTINATION include/${PACKAGE}/libBlitz2DGraphics)

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS Blitz2DGraphics
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib)
else()
  install(TARGETS Blitz2DGraphics
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries
# are requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(Blitz2DGraphics_static STATIC
    ${Blitz2DGraphics_SOURCES} ${Blitz2DGraphics_HEADERS})
  target_include_directories(Blitz2DGraphics_static PUBLIC
    ${BLITZ_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS} ${TIFF_INCLUDE_DIRS}
    ${JPEG_INCLUDE_DIRS})
  target_link_libraries(Blitz2DGraphics_static
    ${BLITZ_LIBRARIES} ${PNG_LIBRARIES} ${TIFF_LIBRARIES} ${JPEG_LIBRARIES})
  install(TARGETS Blitz2DGraphics_static ARCHIVE DESTINATION lib)

  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(Blitz2DGraphics_static PROPERTIES
      OUTPUT_NAME Blitz2DGraphics)
  endif()
endif()

# Explicitly add static library build for static binaries
if(BUILD_STATIC_TOOLS)
  add_library(Blitz2DGraphics_static_tools STATIC
    ${Blitz2DGraphics_SOURCES} ${Blitz2DGraphics_HEADERS})
  target_include_directories(Blitz2DGraphics_static_tools PUBLIC
    ${BLITZ_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS} ${TIFF_INCLUDE_DIRS}
    ${JPEG_INCLUDE_DIRS})
  target_link_libraries(Blitz2DGraphics_static_tools
    ${BLITZ_STATIC_LIBRARIES} ${PNG_STATIC_LIBRARIES} ${TIFF_STATIC_LIBRARIES}
    ${JPEG_STATIC_LIBRARIES})
endif()
