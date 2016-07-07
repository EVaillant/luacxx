if (Boost_FOUND)
  enable_testing()

  add_executable(
    test_core
    test_core.cc
    test_policy.cc
    test_arg_call.cc
    test_lookup_type.cc
    test_convert.cc
    test_request_from.cc
    test_request_to.cc
  )
  target_link_libraries(
    test_core
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${LUA_LIBRARY}
  )
  add_test(test_core test_core)

  add_executable(
    test_binding
    test_binding.cc
    test_module.cc
    test_global.cc
    test_enum.cc
    test_function.cc
    test_class.cc
    test_inheritance_class.cc
    test_method_class.cc
    test_incomplete.cc
    incomplete_data.cc
    test_closure.cc
    test_caller.cc
    test_property.cc
  )
  target_link_libraries(
    test_binding
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${LUA_LIBRARY}
  )
  add_test(test_binding test_binding)

  add_executable(
    test_helper
    test_helper.cc
    test_is_same_class_type.cc
    test_instance_of.cc
    test_cast_to.cc
    test_logger.cc
  )
  target_link_libraries(
    test_helper
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${LUA_LIBRARY}
  )
  add_test(test_helper test_helper)

endif (Boost_FOUND)
