#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/helper/class.hpp>
#include <luacxx/binding/class.hpp>
#include <luacxx/binding/engine.hpp>

#include <config.h>

namespace
{
  struct Data1
  {
    Data1() {}
  };

  struct Data2
  {
    Data2() {}
  };
}

BOOST_AUTO_TEST_CASE( is_same_type_01 )
{
  luacxx::engine engine;
  luacxx::make_is_same_class_type(engine, "is_same_type");
  luacxx::make_class<Data1>(engine, "Data1")
      .ctor();
  luacxx::make_class<Data2>(engine, "Data2")
      .ctor();
  BOOST_CHECK(engine.bind());

  std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "is_same_class_type_01.lua");
  if(!ret.first)
  {
    BOOST_FAIL(ret.second);
  }
}



