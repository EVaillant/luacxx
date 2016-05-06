#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/binding/enum.hpp>
#include <luacxx/binding/function.hpp>
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

  int global_e1 = -1;

  void set_enum1(const Enum1 &e)
  {
    global_e1 = static_cast<int>(e);
  }

  void set_enum2(Enum1 e)
  {
    global_e1 = static_cast<int>(e);
  }

  void set_enum3(const Enum1 *e)
  {
    global_e1 = static_cast<int>(*e);
  }

  int get_e()
  {
    return global_e1;
  }

  Enum1 get_enum1()
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

BOOST_AUTO_TEST_CASE( enum_02 )
{
  luacxx::engine engine;
  luacxx::make_enum<Enum1>(engine, "Enum1")
      .value("VAL1", Enum1::VAL1)
      .value("VAL2", Enum1::VAL2)
      .value("VAL3", Enum1::VAL3);
  luacxx::make_function(engine, "set_enum1", set_enum1);
  luacxx::make_function(engine, "set_enum2", set_enum2);
  luacxx::make_function(engine, "set_enum3", set_enum3);
  luacxx::make_function(engine, "get_e", get_e);
  BOOST_CHECK(engine.bind());

  std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "enum_02.lua");
  if(!ret.first)
  {
    BOOST_FAIL(ret.second);
  }
}

BOOST_AUTO_TEST_CASE( enum_03 )
{
  luacxx::engine engine;
  luacxx::make_enum<Enum1>(engine, "Enum1")
      .value("VAL1", Enum1::VAL1)
      .value("VAL2", Enum1::VAL2)
      .value("VAL3", Enum1::VAL3);
  luacxx::make_function(engine, "get_enum1", get_enum1);
  luacxx::make_function(engine, "get_enum2", get_enum2)
      .get_policy()
      .get(1).input(false)
      .get(1).output(true);
  luacxx::make_function(engine, "get_enum3", get_enum3)
      .get_policy()
      .get(1).input(false)
      .get(1).output(true);
  BOOST_CHECK(engine.bind());

  std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "enum_03.lua");
  if(!ret.first)
  {
    BOOST_FAIL(ret.second);
  }
}

