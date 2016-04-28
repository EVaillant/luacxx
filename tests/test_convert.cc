#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/convert.hpp>

#include <luacxx/lookup_type.hpp>
#include <luacxx/type_info.hpp>
#include <luacxx/policy.hpp>

#include "helper.hpp"

# include <iostream>

BOOST_AUTO_TEST_CASE(convert_from_basic_type)
{
  luacxx::lookup_type lookup;
  lookup.set<bool>(std::make_shared<luacxx::bool_type_info>());
  lookup.set<std::string>(std::make_shared<luacxx::string_type_info<std::string>>());
  lookup.set<int>(std::make_shared<luacxx::integer_type_info<int>>());
  lookup.set<double>(std::make_shared<luacxx::number_type_info<double>>());

  {
    lua_state_guard     state;
    luacxx::policy_node policy;
    toolsbox::any       any;
    std::string         error_msg;

    lua_pushboolean(state, true);
    BOOST_CHECK(luacxx::convert_from<bool>(state, lookup, 1, any, error_msg, policy));
    BOOST_CHECK(error_msg.empty());
    BOOST_REQUIRE(any.is<bool>());
    BOOST_CHECK(any.as<bool>());
  }

  {
    lua_state_guard     state;
    luacxx::policy_node policy;
    toolsbox::any       any;
    std::string         error_msg;

    lua_pushboolean(state, false);
    BOOST_CHECK(luacxx::convert_from<bool>(state, lookup, 1, any, error_msg, policy));
    BOOST_CHECK(error_msg.empty());
    BOOST_REQUIRE(any.is<bool>());
    BOOST_CHECK(!any.as<bool>());
  }

  {
    lua_state_guard     state;
    luacxx::policy_node policy;
    toolsbox::any       any;
    std::string         error_msg;

    std::string         test_msg = "rfrguihjlki";

    lua_pushstring(state, test_msg.c_str());
    BOOST_CHECK(luacxx::convert_from<std::string>(state, lookup, 1, any, error_msg, policy));
    BOOST_CHECK(error_msg.empty());
    BOOST_REQUIRE(any.is<std::string>());
    BOOST_CHECK(test_msg == any.as<std::string>());
  }

  {
    lua_state_guard     state;
    luacxx::policy_node policy;
    toolsbox::any       any;
    std::string         error_msg;

    lua_pushinteger(state,  5);
    BOOST_CHECK(luacxx::convert_from<int>(state, lookup, 1, any, error_msg, policy));
    BOOST_CHECK(error_msg.empty());
    BOOST_REQUIRE(any.is<int>());
    BOOST_CHECK(5 == any.as<int>());
  }

  {
    lua_state_guard     state;
    luacxx::policy_node policy;
    toolsbox::any       any;
    std::string         error_msg;

    lua_pushnumber(state,  4.5);
    BOOST_CHECK(luacxx::convert_from<double>(state, lookup, 1, any, error_msg, policy));
    BOOST_CHECK(error_msg.empty());
    BOOST_REQUIRE(any.is<double>());
    BOOST_CHECK(4.5 == any.as<double>());
  }
}

BOOST_AUTO_TEST_CASE(convert_to_basic_type)
{
  luacxx::lookup_type lookup;
  lookup.set<bool>(std::make_shared<luacxx::bool_type_info>());
  lookup.set<std::string>(std::make_shared<luacxx::string_type_info<std::string>>());
  lookup.set<const char*>(std::make_shared<luacxx::string_type_info<const char*>>());
  lookup.set<int>(std::make_shared<luacxx::integer_type_info<int>>());
  lookup.set<double>(std::make_shared<luacxx::number_type_info<double>>());

  {
    lua_state_guard     state;
    luacxx::policy_node policy;
    toolsbox::any       any = true;
    std::string         error_msg;

    BOOST_CHECK(luacxx::convert_to<bool>(state, lookup, any, error_msg, policy));
    BOOST_CHECK(error_msg.empty());
    BOOST_REQUIRE(lua_isboolean(state, 1));
    BOOST_CHECK(lua_toboolean(state, 1));
  }

  {
    lua_state_guard     state;
    luacxx::policy_node policy;
    toolsbox::any       any = false;
    std::string         error_msg;

    BOOST_CHECK(luacxx::convert_to<bool>(state, lookup, any, error_msg, policy));
    BOOST_CHECK(error_msg.empty());
    BOOST_REQUIRE(lua_isboolean(state, 1));
    BOOST_CHECK(!lua_toboolean(state, 1));
  }

  {
    lua_state_guard     state;
    luacxx::policy_node policy;
    std::string         msg = "hyhyhyhyh";
    toolsbox::any       any = msg.c_str();
    std::string         error_msg;

    BOOST_CHECK(luacxx::convert_to<const char*>(state, lookup, any, error_msg, policy));
    BOOST_CHECK(error_msg.empty());
    BOOST_REQUIRE(lua_isstring(state, 1));
    BOOST_CHECK(lua_tostring(state, 1) == msg);
  }

  {
    lua_state_guard     state;
    luacxx::policy_node policy;
    std::string         msg = "hyhyhyhyh";
    toolsbox::any       any = msg;
    std::string         error_msg;

    BOOST_CHECK(luacxx::convert_to<std::string>(state, lookup, any, error_msg, policy));
    BOOST_CHECK(error_msg.empty());
    BOOST_REQUIRE(lua_isstring(state, 1));
    BOOST_CHECK(msg == lua_tostring(state, 1));
  }

  {
    lua_state_guard     state;
    luacxx::policy_node policy;
    toolsbox::any       any = 5;
    std::string         error_msg;

    BOOST_CHECK(luacxx::convert_to<int>(state, lookup, any, error_msg, policy));
    BOOST_CHECK(error_msg.empty());
    BOOST_REQUIRE(lua_isinteger(state, 1));
    BOOST_CHECK(5 == lua_tointeger(state, 1));
  }

  {
    lua_state_guard     state;
    luacxx::policy_node policy;
    toolsbox::any       any = (double)5.5;
    std::string         error_msg;

    BOOST_CHECK(luacxx::convert_to<double>(state, lookup, any, error_msg, policy));
    BOOST_CHECK(error_msg.empty());
    BOOST_REQUIRE(lua_isnumber(state, 1));
    BOOST_CHECK(5.5 == lua_tonumber(state, 1));
  }
}

namespace
{
  template <class T> using default_vector       = std::vector<T>;
  template <class T> using default_list         = std::list<T>;
  template <class T> using default_set          = std::set<T>;
  template <class K, class V> using default_map = std::map<K, V>;
}

namespace
{
  template <template <class T> class Container> void convert_to_unary_container()
  {
    luacxx::lookup_type lookup;
    lookup.set<bool>(std::make_shared<luacxx::bool_type_info>());
    lookup.set<std::string>(std::make_shared<luacxx::string_type_info<std::string>>());
    lookup.set<const char*>(std::make_shared<luacxx::string_type_info<const char*>>());
    lookup.set<int>(std::make_shared<luacxx::integer_type_info<int>>());
    lookup.set<double>(std::make_shared<luacxx::number_type_info<double>>());

    {
      lua_state_guard     state;
      luacxx::policy_node policy;
      toolsbox::any       any;
      std::string         error_msg;

      Container<int>    input_data = {1, 2, 4};
      any = input_data;

      policy.get_or_create_sub_node(luacxx::node_container_unary);

      BOOST_CHECK(luacxx::convert_to<Container<int>>(state, lookup, any, error_msg, policy));
      BOOST_CHECK(error_msg.empty());
      BOOST_CHECK_EQUAL("t'(i'1',i'1')(i'2',i'2')(i'3',i'4')'", lua_stack_dump(state));
    }

    {
      lua_state_guard     state;
      luacxx::policy_node policy;
      toolsbox::any       any;
      std::string         error_msg;

      Container<double>    input_data = {1.5, 2, 4.9};
      any = input_data;

      policy.get_or_create_sub_node(luacxx::node_container_unary);

      BOOST_CHECK(luacxx::convert_to<Container<double>>(state, lookup, any, error_msg, policy));
      BOOST_CHECK(error_msg.empty());
      BOOST_CHECK_EQUAL("t'(i'1',n'1.5')(i'2',n'2')(i'3',n'4.9')'", lua_stack_dump(state));
    }

    {
      lua_state_guard     state;
      luacxx::policy_node policy;
      toolsbox::any       any;
      std::string         error_msg;

      Container<bool>    input_data = {false, true};
      any = input_data;

      policy.get_or_create_sub_node(luacxx::node_container_unary);

      BOOST_CHECK(luacxx::convert_to<Container<bool>>(state, lookup, any, error_msg, policy));
      BOOST_CHECK(error_msg.empty());
      BOOST_CHECK_EQUAL("t'(i'1',b'false')(i'2',b'true')'", lua_stack_dump(state));
    }

    {
      lua_state_guard     state;
      luacxx::policy_node policy;
      toolsbox::any       any;
      std::string         error_msg;

      Container<std::string>    input_data = {"grgr", "poiu", "rr"};
      any = input_data;

      policy.get_or_create_sub_node(luacxx::node_container_unary);

      BOOST_CHECK(luacxx::convert_to<Container<std::string>>(state, lookup, any, error_msg, policy));
      BOOST_CHECK(error_msg.empty());
      BOOST_CHECK_EQUAL("t'(i'1',s'grgr')(i'2',s'poiu')(i'3',s'rr')'", lua_stack_dump(state));
    }

    {
      lua_state_guard     state;
      luacxx::policy_node policy;
      toolsbox::any       any;
      std::string         error_msg;

      Container<const char*>    input_data = {"grgr", "poiu", "rr"};
      any = input_data;

      policy.get_or_create_sub_node(luacxx::node_container_unary);

      BOOST_CHECK(luacxx::convert_to<Container<const char*>>(state, lookup, any, error_msg, policy));
      BOOST_CHECK(error_msg.empty());
      BOOST_CHECK_EQUAL("t'(i'1',s'grgr')(i'2',s'poiu')(i'3',s'rr')'", lua_stack_dump(state));
    }
  }
}

BOOST_AUTO_TEST_CASE(convert_to_vector)
{
  convert_to_unary_container<default_vector>();
}

BOOST_AUTO_TEST_CASE(convert_to_list)
{
  convert_to_unary_container<default_list>();
}

BOOST_AUTO_TEST_CASE(convert_to_set)
{
  convert_to_unary_container<default_set>();
}

namespace
{
  template <template <class T> class Container> void convert_from_unary_container()
  {
    luacxx::lookup_type lookup;
    lookup.set<bool>(std::make_shared<luacxx::bool_type_info>());
    lookup.set<std::string>(std::make_shared<luacxx::string_type_info<std::string>>());
    lookup.set<int>(std::make_shared<luacxx::integer_type_info<int>>());
    lookup.set<double>(std::make_shared<luacxx::number_type_info<double>>());

    {
      lua_state_guard     state;
      luacxx::policy_node policy;
      toolsbox::any       any;
      std::string         error_msg;

      Container<int>      wanted = {1, 2, 4};

      lua_newtable(state);
      int idx = 1;
      for(int v : wanted)
      {
        lua_pushinteger(state, idx++);
        lua_pushinteger(state, v);
        lua_settable(state, -3);
      }

      policy.get_or_create_sub_node(luacxx::node_container_unary);

      BOOST_CHECK(luacxx::convert_from<Container<int>>(state, lookup, 1, any, error_msg, policy));
      BOOST_CHECK(error_msg.empty());
      BOOST_REQUIRE(any.is<Container<int>>());
      BOOST_CHECK(any.as<Container<int>>() == wanted);
    }

    {
      lua_state_guard     state;
      luacxx::policy_node policy;
      toolsbox::any       any;
      std::string         error_msg;

      Container<double>      wanted = {1.5, 2, 4.9};

      lua_newtable(state);
      int idx = 1;
      for(double v : wanted)
      {
        lua_pushinteger(state, idx++);
        lua_pushnumber(state, v);
        lua_settable(state, -3);
      }

      policy.get_or_create_sub_node(luacxx::node_container_unary);

      BOOST_CHECK(luacxx::convert_from<Container<double>>(state, lookup, 1, any, error_msg, policy));
      BOOST_CHECK(error_msg.empty());
      BOOST_REQUIRE(any.is<Container<double>>());
      BOOST_CHECK(any.as<Container<double>>() == wanted);
    }

    {
      lua_state_guard     state;
      luacxx::policy_node policy;
      toolsbox::any       any;
      std::string         error_msg;

      Container<bool>      wanted = {false, true};

      lua_newtable(state);
      int idx = 1;
      for(bool v : wanted)
      {
        lua_pushinteger(state, idx++);
        lua_pushboolean(state, v);
        lua_settable(state, -3);
      }

      policy.get_or_create_sub_node(luacxx::node_container_unary);

      BOOST_CHECK(luacxx::convert_from<Container<bool>>(state, lookup, 1, any, error_msg, policy));
      BOOST_CHECK(error_msg.empty());
      BOOST_REQUIRE(any.is<Container<bool>>());
      BOOST_CHECK(any.as<Container<bool>>() == wanted);
    }

    {
      lua_state_guard     state;
      luacxx::policy_node policy;
      toolsbox::any       any;
      std::string         error_msg;

      Container<std::string>  wanted = {"grgr", "poiu", "rr"};

      lua_newtable(state);
      int idx = 1;
      for(std::string v : wanted)
      {
        lua_pushinteger(state, idx++);
        lua_pushstring(state, v.c_str());
        lua_settable(state, -3);
      }

      policy.get_or_create_sub_node(luacxx::node_container_unary);

      BOOST_CHECK(luacxx::convert_from<Container<std::string>>(state, lookup, 1, any, error_msg, policy));
      BOOST_CHECK(error_msg.empty());
      BOOST_REQUIRE(any.is<Container<std::string>>());
      BOOST_CHECK(any.as<Container<std::string>>() == wanted);
    }
  }
}

BOOST_AUTO_TEST_CASE(convert_from_vector)
{
  convert_from_unary_container<default_vector>();
}

BOOST_AUTO_TEST_CASE(convert_from_list)
{
  convert_from_unary_container<default_list>();
}

BOOST_AUTO_TEST_CASE(convert_from_set)
{
  convert_from_unary_container<default_set>();
}

namespace
{
  template <template <class K, class V> class Container> void convert_to_binary_container()
  {
    luacxx::lookup_type lookup;
    lookup.set<bool>(std::make_shared<luacxx::bool_type_info>());
    lookup.set<std::string>(std::make_shared<luacxx::string_type_info<std::string>>());
    lookup.set<const char*>(std::make_shared<luacxx::string_type_info<const char*>>());
    lookup.set<int>(std::make_shared<luacxx::integer_type_info<int>>());
    lookup.set<double>(std::make_shared<luacxx::number_type_info<double>>());

    {
      typedef Container<int, std::string> container_type;

      lua_state_guard     state;
      luacxx::policy_node policy;
      toolsbox::any       any;
      std::string         error_msg;

      container_type    input_data = {{1, "aaa"}, {5, "888"}, {45, "uiyui"}};
      any = input_data;

      policy.get_or_create_sub_node(luacxx::node_container_binary_key);
      policy.get_or_create_sub_node(luacxx::node_container_binary_value);

      BOOST_CHECK(luacxx::convert_to<container_type>(state, lookup, any, error_msg, policy));
      BOOST_CHECK(error_msg.empty());
      BOOST_CHECK_EQUAL("t'(i'5',s'888')(i'1',s'aaa')(i'45',s'uiyui')'", lua_stack_dump(state));
    }

    {
      typedef Container<double, std::string> container_type;

      lua_state_guard     state;
      luacxx::policy_node policy;
      toolsbox::any       any;
      std::string         error_msg;

      container_type    input_data = {{1.5, "aaa"}, {5.8, "888"}, {45.9, "uiyui"}};
      any = input_data;

      policy.get_or_create_sub_node(luacxx::node_container_binary_key);
      policy.get_or_create_sub_node(luacxx::node_container_binary_value);

      BOOST_CHECK(luacxx::convert_to<container_type>(state, lookup, any, error_msg, policy));
      BOOST_CHECK(error_msg.empty());
      BOOST_CHECK_EQUAL("t'(n'5.8',s'888')(n'1.5',s'aaa')(n'45.9',s'uiyui')'", lua_stack_dump(state));
    }

    {
      typedef Container<bool, std::string> container_type;

      lua_state_guard     state;
      luacxx::policy_node policy;
      toolsbox::any       any;
      std::string         error_msg;

      container_type    input_data = {{true, "aaa"}, {false, "888"}};
      any = input_data;

      policy.get_or_create_sub_node(luacxx::node_container_binary_key);
      policy.get_or_create_sub_node(luacxx::node_container_binary_value);

      BOOST_CHECK(luacxx::convert_to<container_type>(state, lookup, any, error_msg, policy));
      BOOST_CHECK(error_msg.empty());
      BOOST_CHECK_EQUAL("t'(b'false',s'888')(b'true',s'aaa')'", lua_stack_dump(state));
    }

    {
      typedef Container<std::string, std::string> container_type;

      lua_state_guard     state;
      luacxx::policy_node policy;
      toolsbox::any       any;
      std::string         error_msg;

      container_type    input_data = {{"1", "aaa"}, {"5", "888"}, {"45", "uiyui"}};
      any = input_data;

      policy.get_or_create_sub_node(luacxx::node_container_binary_key);
      policy.get_or_create_sub_node(luacxx::node_container_binary_value);

      BOOST_CHECK(luacxx::convert_to<container_type>(state, lookup, any, error_msg, policy));
      BOOST_CHECK(error_msg.empty());
      BOOST_CHECK_EQUAL("t'(s'5',s'888')(s'1',s'aaa')(s'45',s'uiyui')'", lua_stack_dump(state));
    }

    {
      typedef Container<const char*, std::string> container_type;

      lua_state_guard     state;
      luacxx::policy_node policy;
      toolsbox::any       any;
      std::string         error_msg;

      container_type    input_data = {{"1", "aaa"}, {"5", "888"}, {"45", "uiyui"}};
      any = input_data;

      policy.get_or_create_sub_node(luacxx::node_container_binary_key);
      policy.get_or_create_sub_node(luacxx::node_container_binary_value);

      BOOST_CHECK(luacxx::convert_to<container_type>(state, lookup, any, error_msg, policy));
      BOOST_CHECK(error_msg.empty());
      BOOST_CHECK_EQUAL("t'(s'5',s'888')(s'1',s'aaa')(s'45',s'uiyui')'", lua_stack_dump(state));
    }

    {
      typedef Container<int, const char*> container_type;

      lua_state_guard     state;
      luacxx::policy_node policy;
      toolsbox::any       any;
      std::string         error_msg;

      container_type    input_data = {{1, "aaa"}, {5, "888"}, {45, "uiyui"}};
      any = input_data;

      policy.get_or_create_sub_node(luacxx::node_container_binary_key);
      policy.get_or_create_sub_node(luacxx::node_container_binary_value);

      BOOST_CHECK(luacxx::convert_to<container_type>(state, lookup, any, error_msg, policy));
      BOOST_CHECK(error_msg.empty());
      BOOST_CHECK_EQUAL("t'(i'5',s'888')(i'1',s'aaa')(i'45',s'uiyui')'", lua_stack_dump(state));
    }

    {
      typedef Container<int, bool> container_type;

      lua_state_guard     state;
      luacxx::policy_node policy;
      toolsbox::any       any;
      std::string         error_msg;

      container_type    input_data = {{1, true}, {5, false}, {45, true}};
      any = input_data;

      policy.get_or_create_sub_node(luacxx::node_container_binary_key);
      policy.get_or_create_sub_node(luacxx::node_container_binary_value);

      BOOST_CHECK(luacxx::convert_to<container_type>(state, lookup, any, error_msg, policy));
      BOOST_CHECK(error_msg.empty());
      BOOST_CHECK_EQUAL("t'(i'5',b'false')(i'45',b'true')'", lua_stack_dump(state));
    }

    {
      typedef Container<int, int> container_type;

      lua_state_guard     state;
      luacxx::policy_node policy;
      toolsbox::any       any;
      std::string         error_msg;

      container_type    input_data = {{1, 9}, {5, 888}, {45, 66}};
      any = input_data;

      policy.get_or_create_sub_node(luacxx::node_container_binary_key);
      policy.get_or_create_sub_node(luacxx::node_container_binary_value);

      BOOST_CHECK(luacxx::convert_to<container_type>(state, lookup, any, error_msg, policy));
      BOOST_CHECK(error_msg.empty());
      BOOST_CHECK_EQUAL("t'(i'45',i'66')(i'5',i'888')(i'1',i'9')'", lua_stack_dump(state));
    }

    {
      typedef Container<int, double> container_type;

      lua_state_guard     state;
      luacxx::policy_node policy;
      toolsbox::any       any;
      std::string         error_msg;

      container_type    input_data = {{1, 5.9}, {5, 7.9}, {45, 8.5}};
      any = input_data;

      policy.get_or_create_sub_node(luacxx::node_container_binary_key);
      policy.get_or_create_sub_node(luacxx::node_container_binary_value);

      BOOST_CHECK(luacxx::convert_to<container_type>(state, lookup, any, error_msg, policy));
      BOOST_CHECK(error_msg.empty());
      BOOST_CHECK_EQUAL("t'(i'1',n'5.9')(i'5',n'7.9')(i'45',n'8.5')'", lua_stack_dump(state));
    }
  }
}

BOOST_AUTO_TEST_CASE(convert_to_map)
{
  convert_to_binary_container<default_map>();
}

namespace
{
  template <template <class K, class V> class Container> void convert_from_binary_container()
  {
    luacxx::lookup_type lookup;
    lookup.set<bool>(std::make_shared<luacxx::bool_type_info>());
    lookup.set<std::string>(std::make_shared<luacxx::string_type_info<std::string>>());
    lookup.set<int>(std::make_shared<luacxx::integer_type_info<int>>());
    lookup.set<double>(std::make_shared<luacxx::number_type_info<double>>());
  }
}

BOOST_AUTO_TEST_CASE(convert_from_map)
{
  convert_from_binary_container<default_map>();
}

