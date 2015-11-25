#define BOOST_TEST_DYN_LINK
#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <string>

#include <luacxx/module.hpp>
#include <luacxx/function.hpp>
#include <luacxx/debug.hpp>

#include <vector>
#include <list>
#include <set>
#include <map>
#include <functional>

#include <config.h>

#include "test_luacxx.hpp"

#include <luacxx/debug.hpp>

namespace
{
  lua_State*  save_state;
  std::string save_string;

  void reset()
  {
    save_state  = nullptr;
    save_string = "";
  }

  bool fct1(bool b, const std::string& s, int i, double d, float f)
  {
    return b
        && (s == "fct1")
        && (i == 5)
        && (std::fabs(d - 2.3) <= 1e-6)
        && (std::fabs(f - 4.3) <= 1e-6);
  }

  void fct2(bool& b, std::string& s, int &i, double &d, float &f)
  {
    b = false;
    s = "fct2";
    i = 7;
    d = 5.6;
    f = 8.9;
  }

  void fct3(std::list<int> &i, std::vector<int>& v, std::set<int> & s)
  {
    i = {1, 2, 5, 9};
    v = {2, 3};
    s = {1, 9, 10, 50};
  }

  bool fct4(const std::list<int> &i, const std::vector<int>& v, const std::set<int> & s)
  {
    std::list<int>   c_i = {9, 20, 3};
    std::vector<int> c_v = {2};
    std::set<int>    c_s = {1, 2, 9};

    return (i == c_i) && (c_v == v) && (c_s == s);
  }

  bool fct5(const std::string&, lua_State* s1, const std::string&, lua_State* s2)
  {
    return (s1 == s2) && (s1 == save_state);
  }

  void fct6(std::map<int, std::string> & m)
  {
    m = { {4, "aaa"}, {9, "yy"} };
  }

  bool fct7(const std::map<std::string, std::string>& i)
  {
    std::map<std::string, std::string> want = {{"eeeee", "58585"}, {"tttttt", "r"}};
    return i == want;
  }

  bool fct8(const std::function<int(const std::string&)>&f)
  {
    return f("ooo") == 5;
  }

  bool fct9_internal_(const std::string& l, const std::string& r)
  {
    return l == r;
  }

  std::function<bool (const std::string&)> fct9(const std::string& v)
  {
    using namespace std::placeholders;
    return std::bind(fct9_internal_, _1, v);
  }

  void fct10_internal_(const std::string &v)
  {
    save_string = v;
  }

  std::function<void (const std::string&)> fct10()
  {
    return std::function<void (const std::string&)>(fct10_internal_);
  }

  template <class R, class ... ARGS> std::unique_ptr<luacxx::Module> make_bind_function(luacxx::RegisterType& registry, lua_State *state, const std::string &name, R (*fct)(ARGS...))
  {
    std::unique_ptr<luacxx::Module> module(new luacxx::GlobalModule());
    std::unique_ptr<luacxx::Function<R, ARGS...>>bind_fct(new luacxx::Function<R, ARGS...>(registry, name, fct));
    module->add(name, std::move(bind_fct));
    module->bind(state);
    return std::move(module);
  }
}

BOOST_AUTO_TEST_CASE( basic_type_01 )
{
  lua_State *state = luaL_newstate();
  luaL_openlibs(state);

  luacxx::RegisterType registry;
  luacxx::GlobalModule module;

  helpers::make_function(registry, module, "fct1", &::fct1);

  module.bind(state);

  if(luaL_dofile(state, LUACXX_TESTS_DIR "basic_type_01.lua") != LUA_OK)
  {
    BOOST_FAIL(lua_tostring(state, -1));
  }

  lua_close(state);
}

BOOST_AUTO_TEST_CASE( basic_type_02 )
{
  lua_State *state = luaL_newstate();
  luaL_openlibs(state);

  luacxx::GlobalModule module;
  luacxx::RegisterType registry;

  helpers::make_function(registry, module, "fct2", &::fct2);

  module.bind(state);

  if(luaL_dofile(state, LUACXX_TESTS_DIR "basic_type_02.lua") != LUA_OK)
  {
    BOOST_FAIL(lua_tostring(state, -1));
  }

  lua_close(state);
}

BOOST_AUTO_TEST_CASE( basic_type_03 )
{
  lua_State *state = luaL_newstate();
  luaL_openlibs(state);

  luacxx::GlobalModule module;
  luacxx::RegisterType registry;

  helpers::make_function(registry, module, "fct3", &::fct3);

  module.bind(state);

  if(luaL_dofile(state, LUACXX_TESTS_DIR "basic_type_03.lua") != LUA_OK)
  {
    BOOST_FAIL(lua_tostring(state, -1));
  }

  lua_close(state);
}

BOOST_AUTO_TEST_CASE( basic_type_04 )
{
  lua_State *state = luaL_newstate();
  luaL_openlibs(state);

  luacxx::GlobalModule module;
  luacxx::RegisterType registry;

  helpers::make_function(registry, module, "fct4", &::fct4);

  module.bind(state);

  if(luaL_dofile(state, LUACXX_TESTS_DIR "basic_type_04.lua") != LUA_OK)
  {
    BOOST_FAIL(lua_tostring(state, -1));
  }

  lua_close(state);
}

BOOST_AUTO_TEST_CASE( basic_type_05 )
{
  reset();
  lua_State *state = luaL_newstate();
  save_state = state;
  luaL_openlibs(state);

  luacxx::GlobalModule module;
  luacxx::RegisterType registry;

  helpers::make_function(registry, module, "fct5", &::fct5);

  module.bind(state);

  if(luaL_dofile(state, LUACXX_TESTS_DIR "basic_type_05.lua") != LUA_OK)
  {
    BOOST_FAIL(lua_tostring(state, -1));
  }

  lua_close(state);
}

BOOST_AUTO_TEST_CASE( basic_type_06 )
{
  lua_State *state = luaL_newstate();
  luaL_openlibs(state);

  luacxx::GlobalModule module;
  luacxx::RegisterType registry;

  helpers::make_function(registry, module, "fct6", &::fct6);

  module.bind(state);

  if(luaL_dofile(state, LUACXX_TESTS_DIR "basic_type_06.lua") != LUA_OK)
  {
    BOOST_FAIL(lua_tostring(state, -1));
  }

  lua_close(state);
}

BOOST_AUTO_TEST_CASE( basic_type_07 )
{
  lua_State *state = luaL_newstate();
  luaL_openlibs(state);

  luacxx::GlobalModule module;
  luacxx::RegisterType registry;

  helpers::make_function(registry, module, "fct7", &::fct7);

  module.bind(state);

  if(luaL_dofile(state, LUACXX_TESTS_DIR "basic_type_07.lua") != LUA_OK)
  {
    BOOST_FAIL(lua_tostring(state, -1));
  }

  lua_close(state);
}

BOOST_AUTO_TEST_CASE( basic_type_08 )
{
  lua_State *state = luaL_newstate();
  luaL_openlibs(state);

  luacxx::GlobalModule module;
  luacxx::RegisterType registry;

  helpers::make_function(registry, module, "fct8", &::fct8);

  module.bind(state);

  if(luaL_dofile(state, LUACXX_TESTS_DIR "basic_type_08.lua") != LUA_OK)
  {
    BOOST_FAIL(lua_tostring(state, -1));
  }

  lua_close(state);
}

BOOST_AUTO_TEST_CASE( basic_type_09 )
{
  lua_State *state = luaL_newstate();
  luaL_openlibs(state);

  luacxx::GlobalModule module;
  luacxx::RegisterType registry;

  helpers::make_function(registry, module, "fct9", &::fct9);

  module.bind(state);

  if(luaL_dofile(state, LUACXX_TESTS_DIR "basic_type_09.lua") != LUA_OK)
  {
    BOOST_FAIL(lua_tostring(state, -1));
  }

  lua_close(state);
}

BOOST_AUTO_TEST_CASE( basic_type_10 )
{
  reset();
  lua_State *state = luaL_newstate();
  luaL_openlibs(state);

  luacxx::GlobalModule module;
  luacxx::RegisterType registry;

  helpers::make_function(registry, module, "fct10", &::fct10);

  module.bind(state);

  if(luaL_dofile(state, LUACXX_TESTS_DIR "basic_type_10.lua") != LUA_OK)
  {
    BOOST_FAIL(lua_tostring(state, -1));
  }

  BOOST_CHECK_EQUAL(save_string, "ee");

  lua_close(state);
}
