#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/binding/enum.hpp>
#include <luacxx/binding/engine.hpp>

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
}

BOOST_AUTO_TEST_CASE( enum_01 )
{
  luacxx::engine engine;
  luacxx::make_enum<Enum1>(engine, "Enum1")
      .value("VAL1", Enum1::VAL1)
      .value("VAL2", Enum1::VAL2)
      .value("VAL3", Enum1::VAL3);
  luacxx::make_enum<Enum2, std::string>(engine, "some.thing", "Enum2")
      .value("VAL1", Enum2::VAL1)
      .value("VAL2", Enum2::VAL2)
      .value("VAL3", Enum2::VAL3);
  BOOST_CHECK(engine.bind());

  std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "enum_01.lua");
  if(!ret.first)
  {
    BOOST_FAIL(ret.second);
  }
}

