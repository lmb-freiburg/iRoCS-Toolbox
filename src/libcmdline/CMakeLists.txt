set(cmdline_VERSION_MAJOR 1)
set(cmdline_VERSION_MINOR 0)
set(cmdline_VERSION_PATCH 0)
set(cmdline_VERSION ${cmdline_VERSION_MAJOR}.${cmdline_VERSION_MINOR}.${cmdline_VERSION_PATCH})

set(cmdline_HEADERS
  ArgIter.hh ArgvIter.hh CmdArg.hh CmdArg.icc CmdArgs.hh CmdLine.hh
  CmdLine.icc CmdLineError.hh Compiler.hh Compilers.hh Errors.hh IStreamIter.hh)
set(cmdline_SOURCES CmdArg.cc CmdLine.cc CmdLineError.cc Compiler.cc)

if (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS)
  # Install development headers
  install(FILES ${cmdline_HEADERS}
    DESTINATION include/${PACKAGE}/libcmdline)
endif()

# Build shared library if requested
if (BUILD_SHARED_LIBS)
  add_library(cmdline SHARED ${cmdline_SOURCES} ${cmdline_HEADERS})
  set_target_properties(cmdline PROPERTIES
    VERSION ${cmdline_VERSION} SOVERSION ${cmdline_VERSION_MAJOR})
  install(TARGETS cmdline
    EXPORT iRoCS-ToolboxTargets
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION "include/${PACKAGE}/libcmdline")
endif()

# Build static library if requested
if (BUILD_STATIC_LIBS)
  add_library(cmdline_static STATIC ${cmdline_SOURCES} ${cmdline_HEADERS})
  install(TARGETS cmdline_static
    EXPORT iRoCS-ToolboxTargets
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include/${PACKAGE}/libcmdline)

  # On Linux rename static library to match shared library name
  if (UNIX)
    SET_TARGET_PROPERTIES(cmdline_static PROPERTIES OUTPUT_NAME cmdline)
  endif()
endif()

# Explicitly add static library build for static binaries
if (BUILD_STATIC_TOOLS)
  add_library(cmdline_static_tools STATIC ${cmdline_SOURCES} ${cmdline_HEADERS})
endif()
