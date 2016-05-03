#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/core/request.hpp>
#include <luacxx/core/lookup_type.hpp>
#include <luacxx/core/basic_type_info.hpp>
#include <luacxx/core/policy.hpp>

#include "helper.hpp"

BOOST_AUTO_TEST_CASE( request_from_01 )
{
  luacxx::lookup_type lookup;
  lookup.set<bool>(std::make_shared<luacxx::bool_type_info>());
  lookup.set<std::string>(std::make_shared<luacxx::string_type_info<std::string>>());
  lookup.set<const char*>(std::make_shared<luacxx::string_type_info<const char*>>());
  lookup.set<int>(std::make_shared<luacxx::integer_type_info<int>>());
  lookup.set<double>(std::make_shared<luacxx::number_type_info<double>>());

  {
    typedef luacxx::request_from_lua<void, int> request_from;

    lua_state_guard     state;
    luacxx::policy_node policy;

    policy.get_or_create_sub_node("0").get_or_create_parameter<void>();
    policy.get_or_create_sub_node("1").get_or_create_parameter<int>();

    lua_pushinteger(state, 5);

    bool status = false;
    request_from        request(state, lookup, policy, 1);
    std::string         ret = request.invoke([&status](int i)
    {
      status = (i == 5);
    });

    BOOST_CHECK_EQUAL(ret, "");
    BOOST_CHECK_EQUAL(lua_stack_dump(state), "");
    BOOST_CHECK(status);
  }

  {
    typedef luacxx::request_from_lua<void, int, int> request_from;

    lua_state_guard     state;
    luacxx::policy_node policy;

    policy.get_or_create_sub_node("0").get_or_create_parameter<void>();
    policy.get_or_create_sub_node("1").get_or_create_parameter<int>();
    policy.get_or_create_sub_node("2").get_or_create_parameter<int>();

    lua_pushinteger(state, 5);
    lua_pushinteger(state, 6);

    bool status = false;
    request_from        request(state, lookup, policy, 1);
    std::string         ret = request.invoke([&status](int i, int j)
    {
      status = (i == 5) && (j == 6);
    });

    BOOST_CHECK_EQUAL(ret, "");
    BOOST_CHECK_EQUAL(lua_stack_dump(state), "");
    BOOST_CHECK(status);
  }

  {
    typedef luacxx::request_from_lua<void, bool, int, double, const std::string&, const char*> request_from;

    lua_state_guard     state;
    luacxx::policy_node policy;

    policy.get_or_create_sub_node("0").get_or_create_parameter<void>();
    policy.get_or_create_sub_node("1").get_or_create_parameter<bool>();
    policy.get_or_create_sub_node("2").get_or_create_parameter<int>();
    policy.get_or_create_sub_node("3").get_or_create_parameter<double>();
    policy.get_or_create_sub_node("4").get_or_create_parameter<std::string>();
    policy.get_or_create_sub_node("5").get_or_create_parameter<const char*>();

    lua_pushboolean(state, true);
    lua_pushinteger(state, 5);
    lua_pushnumber(state,  3.5);
    lua_pushstring(state,  "azedf");
    lua_pushstring(state,  "azdft");

    bool status = false;
    request_from        request(state, lookup, policy, 1);
    std::string         ret = request.invoke([&status](bool b, int i, double d, const std::string& s, const char* cstr)
    {
      status = b && (i == 5) && (d == 3.5) && (s == "azedf") && (std::string(cstr) == "azdft");
    });

    BOOST_CHECK_EQUAL(ret, "");
    BOOST_CHECK_EQUAL(lua_stack_dump(state), "");
    BOOST_CHECK(status);
  }
}

BOOST_AUTO_TEST_CASE( request_from_02 )
{
  luacxx::lookup_type lookup;
  lookup.set<bool>(std::make_shared<luacxx::bool_type_info>());
  lookup.set<std::string>(std::make_shared<luacxx::string_type_info<std::string>>());
  lookup.set<const char*>(std::make_shared<luacxx::string_type_info<const char*>>());
  lookup.set<int>(std::make_shared<luacxx::integer_type_info<int>>());
  lookup.set<double>(std::make_shared<luacxx::number_type_info<double>>());

  {
    typedef luacxx::request_from_lua<void, int&> request_from;

    lua_state_guard     state;
    luacxx::policy_node policy;

    policy.get_or_create_sub_node("0").get_or_create_parameter<void>();
    luacxx::parameter_policy& arg_policy = policy.get_or_create_sub_node("1").get_or_create_parameter<int&>();
    arg_policy.input(false);
    arg_policy.output(true);


    bool status = false;
    request_from        request(state, lookup, policy, 1);
    std::string         ret = request.invoke([&status](int &i)
    {
      status = true;
      i      = 5;
    });

    BOOST_CHECK_EQUAL(ret, "");
    BOOST_CHECK_EQUAL(lua_stack_dump(state), "i'5'");
    BOOST_CHECK(status);
  }

  {
    typedef luacxx::request_from_lua<void, int&, std::string&> request_from;

    lua_state_guard     state;
    luacxx::policy_node policy;

    policy.get_or_create_sub_node("0").get_or_create_parameter<void>();
    luacxx::parameter_policy& arg_policy1 = policy.get_or_create_sub_node("1").get_or_create_parameter<int&>();
    arg_policy1.input(false);
    arg_policy1.output(true);
    luacxx::parameter_policy& arg_policy2 = policy.get_or_create_sub_node("2").get_or_create_parameter<std::string&>();
    arg_policy2.input(false);
    arg_policy2.output(true);


    bool status = false;
    request_from        request(state, lookup, policy, 1);
    std::string         ret = request.invoke([&status](int &i, std::string&s)
    {
      status = true;
      i      = 5;
      s      = "ddd";
    });

    BOOST_CHECK_EQUAL(ret, "");
    BOOST_CHECK_EQUAL(lua_stack_dump(state), "i'5's'ddd'");
    BOOST_CHECK(status);
  }

  {
    typedef luacxx::request_from_lua<bool, int&, std::string&> request_from;

    lua_state_guard     state;
    luacxx::policy_node policy;

    policy.get_or_create_sub_node("0").get_or_create_parameter<void>();
    luacxx::parameter_policy& arg_policy1 = policy.get_or_create_sub_node("1").get_or_create_parameter<int&>();
    arg_policy1.input(false);
    arg_policy1.output(true);
    luacxx::parameter_policy& arg_policy2 = policy.get_or_create_sub_node("2").get_or_create_parameter<std::string&>();
    arg_policy2.input(false);
    arg_policy2.output(true);


    bool status = false;
    request_from        request(state, lookup, policy, 1);
    std::string         ret = request.invoke([&status](int &i, std::string&s) -> bool
    {
      status = true;
      i      = 5;
      s      = "ddd";
      return false;
    });

    BOOST_CHECK_EQUAL(ret, "");
    BOOST_CHECK_EQUAL(lua_stack_dump(state), "b'false'i'5's'ddd'");
    BOOST_CHECK(status);
  }

  {
    typedef luacxx::request_from_lua<bool, int&, std::string&> request_from;

    lua_state_guard     state;
    luacxx::policy_node policy;

    luacxx::parameter_policy& arg_policy0 = policy.get_or_create_sub_node("0").get_or_create_parameter<void>();
    arg_policy0.discard_return();
    luacxx::parameter_policy& arg_policy1 = policy.get_or_create_sub_node("1").get_or_create_parameter<int&>();
    arg_policy1.input(false);
    arg_policy1.output(true);
    luacxx::parameter_policy& arg_policy2 = policy.get_or_create_sub_node("2").get_or_create_parameter<std::string&>();
    arg_policy2.input(false);
    arg_policy2.output(true);


    bool status = false;
    request_from        request(state, lookup, policy, 1);
    std::string         ret = request.invoke([&status](int &i, std::string&s) -> bool
    {
      status = true;
      i      = 5;
      s      = "ddd";
      return false;
    });

    BOOST_CHECK_EQUAL(ret, "");
    BOOST_CHECK_EQUAL(lua_stack_dump(state), "i'5's'ddd'");
    BOOST_CHECK(status);
  }
}

BOOST_AUTO_TEST_CASE( request_from_03 )
{
  luacxx::lookup_type lookup;
  lookup.set<bool>(std::make_shared<luacxx::bool_type_info>());
  lookup.set<std::string>(std::make_shared<luacxx::string_type_info<std::string>>());
  lookup.set<const char*>(std::make_shared<luacxx::string_type_info<const char*>>());
  lookup.set<int>(std::make_shared<luacxx::integer_type_info<int>>());
  lookup.set<double>(std::make_shared<luacxx::number_type_info<double>>());

  {
    typedef luacxx::request_from_lua<void, int, const std::string&> request_from;

    lua_state_guard     state;
    luacxx::policy_node policy;

    policy.get_or_create_sub_node("0").get_or_create_parameter<void>();
    luacxx::parameter_policy& arg_policy1 = policy.get_or_create_sub_node("1").get_or_create_parameter<int>();
    arg_policy1.set_default_value<int>(9);
    luacxx::parameter_policy& arg_policy2 = policy.get_or_create_sub_node("2").get_or_create_parameter<const std::string&>();
    arg_policy2.set_default_value<std::string>("truc");

    {
      bool status = false;
      request_from        request(state, lookup, policy, 1);
      std::string         ret = request.invoke([&status](int i, const std::string&s)
      {
        status = (i == 9) && (s == "truc");
      });

      BOOST_CHECK_EQUAL(ret, "");
      BOOST_CHECK_EQUAL(lua_stack_dump(state), "");
      BOOST_CHECK(status);
    }

    {
      lua_pushinteger(state, 5);

      bool status = false;
      request_from        request(state, lookup, policy, 1);
      std::string         ret = request.invoke([&status](int i, const std::string&s)
      {
        status = (i == 5) && (s == "truc");
      });

      BOOST_CHECK_EQUAL(ret, "");
      BOOST_CHECK_EQUAL(lua_stack_dump(state), "");
      BOOST_CHECK(status);
    }

    {
      lua_pushinteger(state, 5);
      lua_pushstring(state, "blabla");

      bool status = false;
      request_from        request(state, lookup, policy, 1);
      std::string         ret = request.invoke([&status](int i, const std::string&s)
      {
        status = (i == 5) && (s == "blabla");
      });

      BOOST_CHECK_EQUAL(ret, "");
      BOOST_CHECK_EQUAL(lua_stack_dump(state), "");
      BOOST_CHECK(status);
    }
  }
}



