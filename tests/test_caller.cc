#define BOOST_TEST_DYN_LINK
#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <string>

#include <luacxx/function.hpp>
#include <luacxx/caller.hpp>
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

BOOST_AUTO_TEST_CASE( caller_01 )
{
  reset_counter();

  lua_State *state = luaL_newstate();
  luaL_openlibs(state);

  luacxx::RegisterType registry;
  luacxx::GlobalModule module;

  helpers::make_function(registry, module, "fct1", &::fct1);

  module.bind(state);

  luacxx::Caller<void> caller1(registry, "fct1", state);
  caller1();

  BOOST_CHECK(!caller1.has_error());
  BOOST_CHECK_EQUAL("", caller1.get_msg_error());
  BOOST_CHECK_EQUAL(1, counter_fct1);

  lua_close(state);
}

BOOST_AUTO_TEST_CASE( caller_02 )
{
  reset_counter();

  lua_State *state = luaL_newstate();
  luaL_openlibs(state);

  luacxx::SubModule module("popo.truc");
  luacxx::RegisterType registry;

  helpers::make_function(registry, module, "fct1", &::fct1);

  module.bind(state);

  luaL_dostring(state, "p = require(\"popo.truc\");");

  luacxx::Caller<void> caller1(registry, "p.fct1", state);
  caller1();

  BOOST_CHECK(!caller1.has_error());
  BOOST_CHECK_EQUAL("", caller1.get_msg_error());
  BOOST_CHECK_EQUAL(1, counter_fct1);

  lua_close(state);
}
