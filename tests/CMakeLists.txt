if (Boost_FOUND)
  enable_testing()

  add_executable(
    test_core
    test_core.cc
    test_policy.cc
    test_request_from.cc
    test_arg_call.cc
    test_lookup_type.cc
  )
  target_link_libraries(
    test_core
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  )
  add_test(test_core test_core)

endif (Boost_FOUND)
