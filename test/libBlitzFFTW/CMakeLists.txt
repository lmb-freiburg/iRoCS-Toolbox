macro(buildTest TEST_NAME)
  add_executable(${TEST_NAME} ${TEST_NAME}.cc )
  target_link_libraries(${TEST_NAME} LINK_PUBLIC BlitzFFTW )
  add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} )
endmacro()

buildTest(testBlitzFFTW)
buildTest(testBlitzFFTWFloat)
