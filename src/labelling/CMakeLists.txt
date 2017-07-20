add_executable(labelling labelling.cc)
target_link_libraries(labelling LINK_PUBLIC Labelling_qt4 cmdline)

# generate proper GUI program on Windows
if(WIN32) # Check if we are on Windows
  if(MSVC) # Check if we are using the Visual Studio compiler
    set_target_properties(labelling PROPERTIES
      WIN32_EXECUTABLE YES
      LINK_FLAGS "/ENTRY:mainCRTStartup")
  else()
    message(WARNING "Only MSVC supported for GUI Application")
  endif()
endif()

install(TARGETS labelling RUNTIME DESTINATION bin)
