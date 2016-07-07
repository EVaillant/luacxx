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
  class Father
  {
    public:
      Father(int id) : id_(id) {}
      int get_id() const {return id_;}

    private:
      int id_;
  };

  class Child1 : public Father
  {
    public:
      Child1() : Father(1) {}
  };

  class Child2 : public Father
  {
    public:
      Child2() : Father(2) {}
  };

  std::shared_ptr<Father> make(int id)
  {
    if(id == 1)
      return std::make_shared<Child1>();
    else
      return std::make_shared<Child2>();
  }
}

BOOST_AUTO_TEST_CASE( cast_to_01 )
{
  luacxx::engine engine;
  luacxx::make_class<Father>(engine, "Father")
      .instance_of()
      .const_method("get_id", &Father::get_id);
  luacxx::make_class<Child1>(engine, "Child1")
      .instance_of()
      .ctor()
      .cast_to<Father>()
      .inheritance<Father>();
  luacxx::make_class<Child2>(engine, "Child2")
      .instance_of()
      .ctor()
      .cast_to<Father>()
      .inheritance<Father>();
  luacxx::make_function(engine, "make", make);
  BOOST_CHECK(engine.bind());

  std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "cast_to_01.lua");
  if(!ret.first)
  {
    BOOST_FAIL(ret.second);
  }
}


