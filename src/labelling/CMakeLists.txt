add_executable(labelling labelling.cc)

target_link_libraries(labelling PRIVATE Labelling_qt4 cmdline)

# generate proper GUI program on Windows
if(WIN32 AND MSVC)
  set_target_properties(labelling PROPERTIES WIN32_EXECUTABLE YES
    LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

install(TARGETS labelling RUNTIME DESTINATION bin)
