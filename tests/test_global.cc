#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/binding/global.hpp>
#include <luacxx/binding/engine.hpp>

#include <config.h>

BOOST_AUTO_TEST_CASE( global_01 )
{
  luacxx::engine engine;
  luacxx::make_global(engine, "truc", 5);
  luacxx::make_global(engine, "some.thing", "truc", 10);
  engine.bind();
  std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "global_01.lua");
  if(!ret.first)
  {
    BOOST_FAIL(ret.second);
  }
}
