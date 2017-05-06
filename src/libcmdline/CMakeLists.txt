add_library(cmdline CmdArg.cc CmdLine.cc CmdLineError.cc Compiler.cc ArgIter.hh ArgvIter.hh CmdArg.hh CmdArg.icc CmdArgs.hh CmdLine.hh CmdLine.icc CmdLineError.hh Compiler.hh Compilers.hh Errors.hh IStreamIter.hh)
set(cmdline_VERSION_MAJOR 1)
set(cmdline_VERSION_MINOR 0)
set(cmdline_VERSION_PATCH 0)
set(cmdline_VERSION ${cmdline_VERSION_MAJOR}.${cmdline_VERSION_MINOR}.${cmdline_VERSION_PATCH})
set_target_properties(cmdline PROPERTIES VERSION ${cmdline_VERSION} SOVERSION ${cmdline_VERSION_MAJOR})

# Install development headers
install(DIRECTORY . DESTINATION include/libcmdline
  FILES_MATCHING PATTERN "*.hh" PATTERN "*.icc")

# Install libraries
if(WIN32)
  # On windows store DLLs alongside the binaries
  install(TARGETS cmdline
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib)  
else()
  install(TARGETS cmdline
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif(WIN32)

# Explicitly add static library build if both static and shared libraries are requested
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  add_library(cmdline_static STATIC CmdArg.cc CmdLine.cc CmdLineError.cc Compiler.cc ArgIter.hh ArgvIter.hh CmdArg.hh CmdArg.icc CmdArgs.hh CmdLine.hh CmdLine.icc CmdLineError.hh Compiler.hh Compilers.hh Errors.hh IStreamIter.hh)
  install(TARGETS cmdline_static ARCHIVE DESTINATION lib)
  
  # On Linux rename static library to match shared library name
  if(UNIX)
    SET_TARGET_PROPERTIES(cmdline_static PROPERTIES OUTPUT_NAME cmdline CLEAN_DIRECT_OUTPUT 1)
  endif(UNIX)
  
endif(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
