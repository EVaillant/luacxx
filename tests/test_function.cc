#define BOOST_TEST_DYN_LINK
#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <string>

#include <luacxx/function.hpp>
#include <luacxx/debug.hpp>

#include "test_luacxx.hpp"

#include <config.h>

namespace
{
  int counter_fct1 = 0;

  void reset_counter()
  {
    counter_fct1 = 0;
  }

  void fct1()
  {
    ++counter_fct1;
  }
}

BOOST_AUTO_TEST_CASE( function_01 )
{
  reset_counter();

  lua_State *state = luaL_newstate();
  luaL_openlibs(state);

  luacxx::GlobalModule module;
  luacxx::RegisterType registry;

  helpers::make_function(registry, module, "fct1", &::fct1);

  module.bind(state);

  if(luaL_dofile(state, LUACXX_TESTS_DIR "function_01.lua") != LUA_OK)
  {
    BOOST_FAIL(lua_tostring(state, -1));
  }

  BOOST_CHECK_EQUAL(1, counter_fct1);

  lua_close(state);
}

BOOST_AUTO_TEST_CASE( function_02 )
{
  reset_counter();

  lua_State *state = luaL_newstate();
  luaL_openlibs(state);

  luacxx::SubModule module("popo.oioi");
  luacxx::RegisterType registry;

  helpers::make_function(registry, module, "fct1", &::fct1);

  module.bind(state);

  if(luaL_dofile(state, LUACXX_TESTS_DIR "function_02.lua") != LUA_OK)
  {
    BOOST_FAIL(lua_tostring(state, -1));
  }

  BOOST_CHECK_EQUAL(1, counter_fct1);

  lua_close(state);
}

