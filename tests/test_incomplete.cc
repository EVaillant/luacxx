#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/binding/function.hpp>
#include <luacxx/binding/class.hpp>
#include <luacxx/binding/engine.hpp>

#include <config.h>
#include "incomplete_data.hpp"

namespace
{
  luacxx::state_type global_state;

  bool fct1(int a, luacxx::state_type state, int b)
  {
    return (a == 5) && (b == 10) && state == global_state;
  }

  bool fct2(luacxx::state_type state)
  {
    return state == global_state;
  }
}

namespace luacxx
{
  template <> struct is_incomplete<opaque_data> : ::std::true_type
  {
  };
}

BOOST_AUTO_TEST_CASE( incomplete_01 )
{
  luacxx::engine engine;
  luacxx::make_function(engine, "fct1", fct1);
  luacxx::make_function(engine, "fct2", fct2);
  BOOST_CHECK(engine.bind());
  global_state = engine.get_state();

  std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "incomplete_01.lua");
  if(!ret.first)
  {
    BOOST_FAIL(ret.second);
  }
}

BOOST_AUTO_TEST_CASE( incomplete_02 )
{
  luacxx::engine engine;
  luacxx::make_class<opaque_data>(engine, "Data");
  luacxx::make_function(engine, "create_opaque_data", create_opaque_data);
  luacxx::make_function(engine, "delate_paque_data",  delate_paque_data);
  luacxx::make_function(engine, "inc_opaque_data",    inc_opaque_data);
  luacxx::make_function(engine, "dec_opaque_data",    dec_opaque_data);
  luacxx::make_function(engine, "get_opaque_data",    get_opaque_data);
  BOOST_CHECK(engine.bind());

  std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "incomplete_02.lua");
  if(!ret.first)
  {
    BOOST_FAIL(ret.second);
  }
}




