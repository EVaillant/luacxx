#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/binding/class.hpp>
#include <luacxx/binding/engine.hpp>

#include <config.h>

namespace
{
  class Father
  {
    public:
      Father() {}
  };

  class Child : public Father
  {
    public:
      Child() {}
  };

  class Other
  {
    public:
      Other() {}
  };
}

BOOST_AUTO_TEST_CASE( instance_of_01 )
{
  luacxx::engine engine;
  luacxx::make_class<Father>(engine, "Father")
      .ctor()
      .instance_of();
  luacxx::make_class<Child>(engine, "Child")
      .ctor()
      .instance_of()
      .inheritance<Father>();
  luacxx::make_class<Other>(engine, "Other")
      .ctor()
      .instance_of();
  BOOST_CHECK(engine.bind());

  std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "instance_of_01.lua");
  if(!ret.first)
  {
    BOOST_FAIL(ret.second);
  }
}

