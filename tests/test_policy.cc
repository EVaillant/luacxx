#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <string>

#include <luacxx/core/policy.hpp>

namespace
{
  struct Data
  {
  };
}

BOOST_AUTO_TEST_CASE(policy_01)
{
  Data data;
  luacxx::policy_node node;
  luacxx::policy<Data, void, int, const std::string&> policy(data, node);

  BOOST_REQUIRE(node.has_node("0"));
  BOOST_REQUIRE(node.has_node("1"));
  BOOST_REQUIRE(node.has_node("2"));

  const luacxx::policy_node& n0 = node.get_sub_node("0");
  const luacxx::policy_node& n1 = node.get_sub_node("1");
  const luacxx::policy_node& n2 = node.get_sub_node("2");

  BOOST_REQUIRE( n0.has_parameter() );
  BOOST_REQUIRE( n1.has_parameter() );
  BOOST_REQUIRE( n2.has_parameter() );

  const luacxx::parameter_policy& param0 = n0.get_parameter();
  const luacxx::parameter_policy& param1 = n1.get_parameter();
  const luacxx::parameter_policy& param2 = n2.get_parameter();

  //
  // default

  // void (return)
  BOOST_CHECK( !param0.is_input() );
  BOOST_CHECK( !param0.is_output() );

  // int (arg1)
  BOOST_CHECK(  param1.is_input() );
  BOOST_CHECK( !param1.is_output() );

  // const std::string& (arg2)
  BOOST_CHECK(  param2.is_input() );
  BOOST_CHECK( !param2.is_output() );
}

BOOST_AUTO_TEST_CASE(policy_02)
{
  Data data;
  luacxx::policy_node node;
  luacxx::policy<Data, void, int, const std::string&> policy(data, node);

  const luacxx::parameter_policy& param1 = node.get_sub_node("1").get_parameter();
  const luacxx::parameter_policy& param2 = node.get_sub_node("2").get_parameter();

  BOOST_CHECK(!param1.has_default_value());
  BOOST_CHECK(!param2.has_default_value());

  policy.get(1).set_default_value(5);
  policy.get(2).set_default_value<std::string>("blabla");

  BOOST_CHECK( param1.has_default_value());
  BOOST_CHECK( param2.has_default_value());

  const luacxx::parameter_policy::variable_type& default_value1 = param1.get_default_value();
  const luacxx::parameter_policy::variable_type& default_value2 = param2.get_default_value();

  BOOST_REQUIRE(default_value1.is<int>());
  BOOST_REQUIRE(default_value2.is<std::string>());

  BOOST_CHECK_EQUAL(default_value1.as<int>(), 5);
  BOOST_CHECK_EQUAL(default_value2.as<std::string>(), "blabla");
}
