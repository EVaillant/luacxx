#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/binding/module.hpp>
#include <luacxx/binding/bindable.hpp>

#include <luacxx/core/lookup_type.hpp>
#include <luacxx/core/basic_type_info.hpp>

#include "helper.hpp"
#include <config.h>

namespace
{
  class const_integer_field : public luacxx::bindable
  {
    public:
      const_integer_field(int value)
        : value_(value)
      {
      }

      virtual void bind(luacxx::state_type state) override
      {
        lua_pushinteger(state, value_);
      }

    private:
      int value_;
  };
}

BOOST_AUTO_TEST_CASE( module_01 )
{
  luacxx::lookup_type lookup;
  lookup.set<bool>(std::make_shared<luacxx::bool_type_info>());
  lookup.set<std::string>(std::make_shared<luacxx::string_type_info<std::string>>());
  lookup.set<int>(std::make_shared<luacxx::integer_type_info<int>>());
  lookup.set<double>(std::make_shared<luacxx::number_type_info<double>>());

  {
    lua_state_guard       state;
    luacxx::global_module module;

    luaopen_base(state);
    luaopen_package(state);

    std::unique_ptr<const_integer_field> c1 = std::make_unique<const_integer_field>(5);
    std::unique_ptr<const_integer_field> c2 = std::make_unique<const_integer_field>(20);
    std::unique_ptr<const_integer_field> c3 = std::make_unique<const_integer_field>(30);

    module.get_module("").add("bla1", std::move(c1));
    module.get_module("some.thing").add("bla2", std::move(c2));
    module.get_module("some.thing").add("bla3", std::move(c3));
    module.bind(state);

    if(luaL_dofile(state, LUACXX_TESTS_DIR "module_01.lua") != LUA_OK)
    {
      BOOST_FAIL(lua_tostring(state, -1));
    }
  }
}
