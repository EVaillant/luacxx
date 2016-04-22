#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/convert.hpp>

#include <luacxx/lookup_type.hpp>
#include <luacxx/type_info.hpp>
#include <luacxx/policy.hpp>

#include <iostream>

namespace
{
  class lua_state_guard
  {
    public:
      lua_state_guard()
      {
        state_ = luaL_newstate();
      }

      operator lua_State*()
      {
        return state_;
      }

      ~lua_state_guard()
      {
        lua_close(state_);
      }
    private:
      lua_State* state_;
  };
}

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
