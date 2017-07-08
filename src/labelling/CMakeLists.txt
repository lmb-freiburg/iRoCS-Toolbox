add_executable(labelling labelling.cc)
target_link_libraries(labelling LINK_PUBLIC Labelling_qt4 cmdline)

install(TARGETS labelling RUNTIME DESTINATION bin)
