#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/binding/function.hpp>
#include <luacxx/binding/class.hpp>
#include <luacxx/binding/engine.hpp>

#include <config.h>

namespace
{
  bool fct1(const std::function<int(const std::string&)>&f)
  {
    return f("ooo") == 5;
  }

  bool fct2_internal_(const std::string& l, const std::string& r)
  {
    return l == r;
  }

  std::function<bool (const std::string&)> fct2(const std::string& v)
  {
    using namespace std::placeholders;
    return std::bind(fct2_internal_, _1, v);
  }
}

BOOST_AUTO_TEST_CASE( closure_01 )
{
  luacxx::engine engine;
  luacxx::make_function(engine, "fct1", fct1);
  luacxx::make_function(engine, "fct2", fct2);
  BOOST_CHECK(engine.bind());

  std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "closure_01.lua");
  if(!ret.first)
  {
    BOOST_FAIL(ret.second);
  }
}




