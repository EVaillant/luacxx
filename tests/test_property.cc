#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/binding/class.hpp>
#include <luacxx/binding/function.hpp>
#include <luacxx/binding/engine.hpp>

#include <config.h>

namespace
{
  struct Data
  {
    Data() : field(0) {}
    int get_ref() const {return field;}

    const int& get_field2() const {return field; }
    void set_field2(const int&i) { field = i;}

    int get_field3() const {return field; }
    void set_field3(int i) { field = i;}

    int field;
  };

  int ext_get_data_field(const Data&d)
  {
    return d.field;
  }

  void ext_set_data_field(Data&d, int v)
  {
    d.field = v;
  }
}

BOOST_AUTO_TEST_CASE( property_01 )
{
  luacxx::engine engine;
  luacxx::make_class<Data>(engine, "Data")
      .ctor()
      .const_method("get_ref", &Data::get_ref)
      .property("field1", &Data::field)
      .property("field2", &Data::get_field2, &Data::set_field2)
      .property("field3", &Data::get_field3, &Data::set_field3)
      .property<int>("field4", ext_get_data_field, ext_set_data_field);
  BOOST_CHECK(engine.bind());

  std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "property_01.lua");
  if(!ret.first)
  {
    BOOST_FAIL(ret.second);
  }
}
