#define BOOST_TEST_DYN_LINK
#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <string>

#include <luacxx/enum.hpp>
#include <luacxx/class.hpp>
#include <luacxx/module.hpp>

#include <luacxx/debug.hpp>

#include "test_luacxx.hpp"

#include <config.h>

namespace
{
  enum class Enum1 : int
  {
    VAL1,
    VAL2,
    VAL3
  };

  enum class Enum2 : int
  {
    VAL1,
    VAL2,
    VAL3
  };

  std::ostream& operator<<(std::ostream& stream, const Enum2 &p)
  {
    switch(p)
    {
      case Enum2::VAL1:
        stream << "VAL1";
        break;

      case Enum2::VAL2:
        stream << "VAL2";
        break;

      case Enum2::VAL3:
        stream << "VAL3";
        break;
    }
    return stream;
  }

  class Cls01
  {
    public:
      Cls01()
        : e_(-1)
      {
      }

      void set_enum1(const Enum1 &e)
      {
        e_ = static_cast<int>(e);
      }

      void set_enum2(Enum1 e)
      {
        e_ = static_cast<int>(e);
      }

      void set_enum3(const Enum1 *e)
      {
        e_ = static_cast<int>(*e);
      }

      int get_e() const
      {
        return e_;
      }

    private:
      int e_;
  };

  class Cls02
  {
    public:
      Cls02()
      {
      }

      Enum1 get_enum1() const
      {
        return Enum1::VAL1;
      }

      void get_enum2(Enum1 *e)
      {
        *e = Enum1::VAL2;
      }

      void get_enum3(Enum1 &e)
      {
        e = Enum1::VAL3;
      }
  };
}

BOOST_AUTO_TEST_CASE( enum_01 )
{
  lua_State *state = luaL_newstate();
  luaL_openlibs(state);

  luacxx::RegisterType registry;
  luacxx::GlobalModule module;

  luacxx::Enum<Enum1> &enum1_wrapper = helpers::make_enum<Enum1>(registry, module, "Enum1");
  enum1_wrapper
      .value("VAL1", Enum1::VAL1)
      .value("VAL2", Enum1::VAL2)
      .value("VAL3", Enum1::VAL3);

  luacxx::Enum<Enum2, std::string>& enum2_wrapper = helpers::make_enum<Enum2, std::string>(registry, module, "Enum2");
  enum2_wrapper
      .value("VAL1", Enum2::VAL1)
      .value("VAL2", Enum2::VAL2)
      .value("VAL3", Enum2::VAL3);

  module.bind(state);

  if(luaL_dofile(state, LUACXX_TESTS_DIR "enum_01.lua") != LUA_OK)
  {
    BOOST_FAIL(lua_tostring(state, -1));
  }

  lua_close(state);
}

BOOST_AUTO_TEST_CASE( enum_02 )
{
  lua_State *state = luaL_newstate();
  luaL_openlibs(state);

  luacxx::RegisterType registry;
  luacxx::GlobalModule module;

  luacxx::Enum<Enum1>& enum1_wrapper = helpers::make_enum<Enum1>(registry, module, "Enum1");
  enum1_wrapper
      .value("VAL1", Enum1::VAL1)
      .value("VAL2", Enum1::VAL2)
      .value("VAL3", Enum1::VAL3);

  luacxx::Class<Cls01>& cls1_wrapper = helpers::make_class<Cls01>(registry, module, "Cls01");
  cls1_wrapper
      .ctor<>()
      .const_method("get_e",     &Cls01::get_e)
      .method("set_enum1", &Cls01::set_enum1)
      .method("set_enum2", &Cls01::set_enum2)
      .method("set_enum3", &Cls01::set_enum3);

  module.bind(state);

  if(luaL_dofile(state, LUACXX_TESTS_DIR "enum_02.lua") != LUA_OK)
  {
    BOOST_FAIL(lua_tostring(state, -1));
  }

  lua_close(state);
}

BOOST_AUTO_TEST_CASE( enum_03 )
{
  lua_State *state = luaL_newstate();
  luaL_openlibs(state);

  luacxx::RegisterType registry;
  luacxx::GlobalModule module;

  luacxx::Enum<Enum1>& enum1_wrapper = helpers::make_enum<Enum1>(registry, module, "Enum1");
  enum1_wrapper
      .value("VAL1", Enum1::VAL1)
      .value("VAL2", Enum1::VAL2)
      .value("VAL3", Enum1::VAL3);

  luacxx::Class<Cls02>& cls1_wrapper = helpers::make_class<Cls02>(registry, module, "Cls02");
  cls1_wrapper
      .ctor<>()
      .const_method("get_enum1", &Cls02::get_enum1)
      .method("get_enum2", &Cls02::get_enum2)
      .method("get_enum3", &Cls02::get_enum3);

  module.bind(state);

  if(luaL_dofile(state, LUACXX_TESTS_DIR "enum_03.lua") != LUA_OK)
  {
    BOOST_FAIL(lua_tostring(state, -1));
  }

  lua_close(state);
}
