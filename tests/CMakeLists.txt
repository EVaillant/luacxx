if (Boost_FOUND)
	enable_testing()

	#
	# basic_type
	add_executable(
		test_basic_type
		test_basic_type.cc
	)
	target_link_libraries(
		test_basic_type
		luacxx
		${LUA_LIBRARIES}
		${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
	)
	add_test(test_basic_type test_basic_type)

	#
	# class
	add_executable(
		test_class
		test_class.cc
	)
	target_link_libraries(
		test_class
		luacxx
		${LUA_LIBRARIES}
		${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
	)
	add_test(test_class test_class)

	#
	# enum
	add_executable(
		test_enum
		test_enum.cc
	)
	target_link_libraries(
		test_enum
		luacxx
		${LUA_LIBRARIES}
		${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
	)
	add_test(test_enum test_enum)

	#
	# module
	add_executable(
		test_module
		test_module.cc
	)
	target_link_libraries(
		test_module
		luacxx
		${LUA_LIBRARIES}
		${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
	)
	add_test(test_module test_module)

	#
	# caller
	add_executable(
		test_caller
		test_caller.cc
	)
	target_link_libraries(
		test_caller
		luacxx
		${LUA_LIBRARIES}
		${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
	)
	add_test(test_caller test_caller)

	#
	# function
	add_executable(
		test_function
		test_function.cc
	)
	target_link_libraries(
		test_function
		luacxx
		${LUA_LIBRARIES}
		${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
	)
	add_test(test_function test_function)

endif (Boost_FOUND)
