add_executable(testBaseEnvironment testBaseEnvironment.cc )
target_link_libraries(testBaseEnvironment LINK_PUBLIC BaseFunctions )
add_test(NAME testBaseEnvironment COMMAND testBaseEnvironment )

add_executable(testBaseFile testBaseFile.cc )
target_compile_definitions(testBaseFile PRIVATE
  -DTOP_BUILD_DIR="${iRoCSToolbox_BINARY_DIR}" )
target_link_libraries(testBaseFile LINK_PUBLIC BaseFunctions)
add_test(NAME testBaseFile COMMAND testBaseFile )
