#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/convert.hpp>

#include <luacxx/lookup_type.hpp>
#include <luacxx/type_info.hpp>
#include <luacxx/policy.hpp>

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

  std::string lua_stack_dump(lua_State *state);
  void lua_stack_dump(lua_State *state, std::ostream& stream);
  void lua_stack_dump(lua_State *state, int i, std::ostream& stream);

  void lua_stack_dump(lua_State *state, int i, std::ostream& stream)
  {
    int t = lua_type(state, i);
    switch (t)
    {
      case LUA_TSTRING:  /* strings */
        stream << "s'" << lua_tostring(state, i) << "'";
        break;

      case LUA_TBOOLEAN:  /* booleans */
        stream << "b'" << (lua_toboolean(state, i) ? "true" : "false") << "'";
        break;

      case LUA_TNUMBER:  /* numbers or integer */
        if(lua_isinteger(state, i))
        {
          stream << "i'" << lua_tointeger(state, i) << "'";
        }
        else if(lua_isnumber(state, i))
        {
          stream << "n'" << lua_tonumber(state, i) << "'";
        }
        break;

      case LUA_TTABLE:  /* table */
        stream << "t'";
        lua_pushnil(state);
        while(lua_next(state, i))
        {
          lua_pushvalue(state, -2);
          stream << "(";
          lua_stack_dump(state, lua_gettop(state), stream);
          stream << ",";
          lua_stack_dump(state, lua_gettop(state) - 1, stream);
          stream << ")";
          lua_pop(state, 2);
        }
        stream << "'";
        break;

      default:  /* other values */
        stream << "?'" << lua_typename(state, t) << "'";
        break;

    }
  }

  void lua_stack_dump(lua_State *state, std::ostream& stream)
  {
    int i;
    int top = lua_gettop(state);
    for (i = 1; i <= top; i++)
    {
      lua_stack_dump(state, i, stream);
      if( i != top)
      {
        stream << " ";
      }
    }
  }

  std::string lua_stack_dump(lua_State *state)
  {
    std::ostringstream stream;
    lua_stack_dump(state, stream);
    return stream.str();
  }
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

  template <class T> using default_vector = std::vector<T>;
  template <class T> using default_list   = std::list<T>;
  template <class T> using default_set    = std::set<T>;
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
