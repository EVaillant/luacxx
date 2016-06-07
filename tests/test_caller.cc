#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/binding/function.hpp>
#include <luacxx/binding/caller.hpp>
#include <luacxx/binding/engine.hpp>

#include <config.h>

namespace
{
  int counter_fct1 = 0;
  int counter_fct2 = 0;

  void reset_counter()
  {
    counter_fct1 = 0;
    counter_fct2 = 0;
  }

  void fct1()
  {
    ++counter_fct1;
  }

  int fct2()
  {
    ++counter_fct2;
    return 5;
  }
}

BOOST_AUTO_TEST_CASE( caller_01 )
{
  reset_counter();
  luacxx::engine engine;
  luacxx::make_function(engine, "fct", &::fct1);
  BOOST_CHECK(engine.bind());

  auto caller = luacxx::make_caller<void>(engine, "fct");
  std::string msg_error = caller();
  BOOST_CHECK_EQUAL("", msg_error);
  BOOST_CHECK_EQUAL(1, counter_fct1);
  BOOST_CHECK_EQUAL(0, counter_fct2);
}

BOOST_AUTO_TEST_CASE( caller_02 )
{
  reset_counter();
  luacxx::engine engine;
  luacxx::make_function(engine, "fct", &::fct2);
  BOOST_CHECK(engine.bind());

  auto caller = luacxx::make_caller<int>(engine, "fct");
  std::string msg_error = caller();
  BOOST_CHECK_EQUAL("", msg_error);
  BOOST_CHECK_EQUAL(0, counter_fct1);
  BOOST_CHECK_EQUAL(1, counter_fct2);
  BOOST_CHECK_EQUAL(5, caller.get_return());
}
