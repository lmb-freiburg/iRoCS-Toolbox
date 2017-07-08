add_executable(testlmbs2kit testlmbs2kit.cc)
target_link_libraries(testlmbs2kit LINK_PUBLIC lmbs2kit)
add_test(NAME testlmbs2kit COMMAND testlmbs2kit)
