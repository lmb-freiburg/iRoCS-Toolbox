macro(buildTest TEST_NAME)
  add_executable(${TEST_NAME} ${TEST_NAME}.cc )
  target_compile_definitions(${TEST_NAME} PRIVATE
    -DTOP_BUILD_DIR="${PROJECT_BINARY_DIR}" )
  target_link_libraries(${TEST_NAME} LINK_PUBLIC ArrayToolbox )
  add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} )
endmacro()

buildTest(testArray)
buildTest(testATBLinAlg)
buildTest(testLocalSumFilter)
