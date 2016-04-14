#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/utility.hpp>

BOOST_AUTO_TEST_CASE(test_arg_call_int_01)
{
  toolsbox::any v = 5;

  std::string msg;
  luacxx::check_arg_call<int>(msg, v);
  BOOST_REQUIRE(msg.empty());
  BOOST_CHECK_EQUAL(5, luacxx::cast_arg_call<int>(v));
}

BOOST_AUTO_TEST_CASE(test_arg_call_int_02)
{
  toolsbox::any v = 5;

  std::string msg;
  luacxx::check_arg_call<const int&>(msg, v);
  BOOST_REQUIRE(msg.empty());
  BOOST_CHECK_EQUAL(5, luacxx::cast_arg_call<const int&>(v));
}

BOOST_AUTO_TEST_CASE(test_arg_call_int_03)
{
  toolsbox::any v = 5;

  std::string msg;
  luacxx::check_arg_call<const int*>(msg, v);
  BOOST_REQUIRE(msg.empty());
  BOOST_CHECK_EQUAL(5, *luacxx::cast_arg_call<const int*>(v));
}

BOOST_AUTO_TEST_CASE(test_arg_call_int_04)
{
  toolsbox::any v = 5;

  std::string msg;
  luacxx::check_arg_call<int*>(msg, v);
  BOOST_REQUIRE(msg.empty());
  BOOST_CHECK_EQUAL(5, *luacxx::cast_arg_call<int*>(v));
}

BOOST_AUTO_TEST_CASE(test_arg_call_int_05)
{
  toolsbox::any v = 5;

  std::string msg;
  luacxx::check_arg_call<const int>(msg, v);
  BOOST_REQUIRE(msg.empty());
  BOOST_CHECK_EQUAL(5, luacxx::cast_arg_call<const int>(v));
}

BOOST_AUTO_TEST_CASE(test_arg_call_pint_01)
{
  int tmp = 5;
  toolsbox::any v = &tmp;

  std::string msg;
  luacxx::check_arg_call<int*>(msg, v);
  BOOST_REQUIRE(msg.empty());
  BOOST_CHECK_EQUAL(5, *luacxx::cast_arg_call<int*>(v));
}

BOOST_AUTO_TEST_CASE(test_arg_call_pint_02)
{
  int tmp = 5;
  toolsbox::any v = &tmp;

  std::string msg;
  luacxx::check_arg_call<const int*>(msg, v);
  BOOST_REQUIRE(msg.empty());
  BOOST_CHECK_EQUAL(5, *luacxx::cast_arg_call<const int*>(v));
}

BOOST_AUTO_TEST_CASE(test_arg_call_pint_03)
{
  int tmp = 5;
  toolsbox::any v = &tmp;

  std::string msg;
  luacxx::check_arg_call<int>(msg, v);
  BOOST_REQUIRE(msg.empty());
  BOOST_CHECK_EQUAL(5, luacxx::cast_arg_call<int>(v));
}

BOOST_AUTO_TEST_CASE(test_arg_call_pint_04)
{
  int tmp = 5;
  toolsbox::any v = &tmp;

  std::string msg;
  luacxx::check_arg_call<int&>(msg, v);
  BOOST_REQUIRE(msg.empty());
  BOOST_CHECK_EQUAL(5, luacxx::cast_arg_call<int&>(v));
}

BOOST_AUTO_TEST_CASE(test_arg_call_pint_05)
{
  int tmp = 5;
  toolsbox::any v = &tmp;

  std::string msg;
  luacxx::check_arg_call<const int&>(msg, v);
  BOOST_REQUIRE(msg.empty());
  BOOST_CHECK_EQUAL(5, luacxx::cast_arg_call<const int&>(v));
}

BOOST_AUTO_TEST_CASE(test_arg_call_sint_01)
{
  toolsbox::any v = std::make_shared<int>(5);

  std::string msg;
  luacxx::check_arg_call<int*>(msg, v);
  BOOST_REQUIRE(msg.empty());
  BOOST_CHECK_EQUAL(5, *luacxx::cast_arg_call<int*>(v));
}

BOOST_AUTO_TEST_CASE(test_arg_call_sint_02)
{
  toolsbox::any v = std::make_shared<int>(5);

  std::string msg;
  luacxx::check_arg_call<int>(msg, v);
  BOOST_REQUIRE(msg.empty());
  BOOST_CHECK_EQUAL(5, luacxx::cast_arg_call<int>(v));
}

BOOST_AUTO_TEST_CASE(test_arg_call_sint_03)
{
  toolsbox::any v = std::make_shared<int>(5);

  std::string msg;
  luacxx::check_arg_call<int&>(msg, v);
  BOOST_REQUIRE(msg.empty());
  BOOST_CHECK_EQUAL(5, luacxx::cast_arg_call<int&>(v));
}

BOOST_AUTO_TEST_CASE(test_arg_call_sint_04)
{
  toolsbox::any v = std::make_shared<int>(5);

  std::string msg;
  luacxx::check_arg_call<const int*>(msg, v);
  BOOST_REQUIRE(msg.empty());
  BOOST_CHECK_EQUAL(5, *luacxx::cast_arg_call<const int*>(v));
}

BOOST_AUTO_TEST_CASE(test_arg_call_sint_05)
{
  toolsbox::any v = std::make_shared<int>(5);

  std::string msg;
  luacxx::check_arg_call<const int>(msg, v);
  BOOST_REQUIRE(msg.empty());
  BOOST_CHECK_EQUAL(5, luacxx::cast_arg_call<const int>(v));
}

BOOST_AUTO_TEST_CASE(test_arg_call_sint_06)
{
  toolsbox::any v = std::make_shared<int>(5);

  std::string msg;
  luacxx::check_arg_call<const int&>(msg, v);
  BOOST_REQUIRE(msg.empty());
  BOOST_CHECK_EQUAL(5, luacxx::cast_arg_call<const int&>(v));
}
