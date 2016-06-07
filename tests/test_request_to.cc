#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/core/request.hpp>
#include <luacxx/core/lookup_type.hpp>
#include <luacxx/core/basic_type_info.hpp>
#include <luacxx/core/policy.hpp>

#include <luacxx/helper/stack.hpp>

#include "helper.hpp"

#include <iostream>

BOOST_AUTO_TEST_CASE( request_to_01 )
{
  luacxx::lookup_type lookup;
  lookup.set<bool>(std::make_shared<luacxx::bool_type_info>());
  lookup.set<std::string>(std::make_shared<luacxx::string_type_info<std::string>>());
  lookup.set<const char*>(std::make_shared<luacxx::string_type_info<const char*>>());
  lookup.set<int>(std::make_shared<luacxx::integer_type_info<int>>());
  lookup.set<double>(std::make_shared<luacxx::number_type_info<double>>());

  {
    typedef luacxx::request_to_lua<void, int> request_to;

    lua_state_guard     state;
    luacxx::policy_node policy;

    luaopen_base(state);

    policy.get_or_create_sub_node("0").get_or_create_parameter<void>();
    policy.get_or_create_sub_node("1").get_or_create_parameter<int>();

    luaL_dostring(state, "_ENV[\"fct\"] = function(x)assert(x == 5); end");
    lua_getglobal(state, "fct");

    request_to          request(lookup, policy, 1);
    std::string         ret = request.invoke(state, 5);

    BOOST_CHECK_EQUAL(ret, "");
  }
}

BOOST_AUTO_TEST_CASE( request_to_02 )
{
  luacxx::lookup_type lookup;
  lookup.set<bool>(std::make_shared<luacxx::bool_type_info>());
  lookup.set<std::string>(std::make_shared<luacxx::string_type_info<std::string>>());
  lookup.set<const char*>(std::make_shared<luacxx::string_type_info<const char*>>());
  lookup.set<int>(std::make_shared<luacxx::integer_type_info<int>>());
  lookup.set<double>(std::make_shared<luacxx::number_type_info<double>>());

  {
    typedef luacxx::request_to_lua<std::string, int> request_to;

    lua_state_guard     state;
    luacxx::policy_node policy;

    luaopen_base(state);

    policy.get_or_create_sub_node("0").get_or_create_parameter<std::string>();
    policy.get_or_create_sub_node("1").get_or_create_parameter<int>();

    luaL_dostring(state, "_ENV[\"fct\"] = function(x) return \"blabla\"..x; end");
    lua_getglobal(state, "fct");

    request_to          request(lookup, policy, 1);
    std::string         ret = request.invoke(state, 5);

    BOOST_CHECK_EQUAL(ret, "");
    BOOST_CHECK_EQUAL(request.get_return(), "blabla5");
  }
}

BOOST_AUTO_TEST_CASE( request_to_03 )
{
  luacxx::lookup_type lookup;
  lookup.set<bool>(std::make_shared<luacxx::bool_type_info>());
  lookup.set<std::string>(std::make_shared<luacxx::string_type_info<std::string>>());
  lookup.set<const char*>(std::make_shared<luacxx::string_type_info<const char*>>());
  lookup.set<int>(std::make_shared<luacxx::integer_type_info<int>>());
  lookup.set<double>(std::make_shared<luacxx::number_type_info<double>>());

  {
    typedef luacxx::request_to_lua<std::string, int, int&> request_to;

    lua_state_guard     state;
    luacxx::policy_node policy;

    luaopen_base(state);

    policy.get_or_create_sub_node("0").get_or_create_parameter<std::string>();
    policy.get_or_create_sub_node("1").get_or_create_parameter<int>();
    luacxx::parameter_policy& p2_policy = policy.get_or_create_sub_node("2").get_or_create_parameter<int&>();
    p2_policy.output(true);
    p2_policy.input(false);

    luaL_dostring(state, "_ENV[\"fct\"] = function(x) return \"blabla\"..x,x; end");
    lua_getglobal(state, "fct");

    int y = -1;

    request_to          request(lookup, policy, 1);
    std::string         ret = request.invoke(state, 5, y);

    BOOST_CHECK_EQUAL(ret, "");
    BOOST_CHECK_EQUAL(request.get_return(), "blabla5");
    BOOST_CHECK_EQUAL(y, 5);
  }
}
