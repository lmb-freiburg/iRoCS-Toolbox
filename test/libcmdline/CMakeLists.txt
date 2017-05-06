add_executable(testCmdLine testCmdLine.cc)
target_link_libraries(testCmdLine LINK_PUBLIC cmdline)
add_test(NAME testCmdLine COMMAND testCmdLine)
