#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/binding/function.hpp>
#include <luacxx/binding/engine.hpp>

#include <config.h>

namespace
{
  bool fct1(int a, const std::string& s)
  {
    return (a == 5) && (s == "blabla");
  }
}

BOOST_AUTO_TEST_CASE( function_01 )
{
  luacxx::engine engine;
  luacxx::make_function(engine, "fct1", fct1);
  luacxx::make_function(engine, "some.thing", "fct2", fct1);
  BOOST_CHECK(engine.bind());

  std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "function_01.lua");
  if(!ret.first)
  {
    BOOST_FAIL(ret.second);
  }
}


