if (Boost_FOUND)
  enable_testing()

  #
  # variant
  add_executable(
    test_request_from
    test_request_from.cc
  )
  target_link_libraries(
    test_request_from
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  )
  add_test(test_request_from test_request_from)

endif (Boost_FOUND)
