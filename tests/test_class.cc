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
  class Class1
  {
    public:
      Class1()
      {
        last = this;
        ++nb_init;
      }

      ~Class1()
      {
        ++nb_destroy;
      }

      static size_t  nb_init;
      static size_t  nb_destroy;
      static Class1* last;
  };

  Class1* Class1::last =  nullptr;
  size_t Class1::nb_init = 0;
  size_t Class1::nb_destroy = 0;
  std::shared_ptr<Class1> global_class1;

  bool fct1(const std::shared_ptr<Class1>& c)
  {
    return c.get() == Class1::last;
  }

  bool fct2(const Class1& c)
  {
    return &c == Class1::last;
  }

  bool fct3(const Class1* c)
  {
    return c == Class1::last;
  }

  bool fct4(Class1* c)
  {
    return c == Class1::last;
  }

  std::shared_ptr<Class1> fct5()
  {
    return std::make_shared<Class1>();
  }

  Class1* fct6()
  {
    return new Class1();
  }

  Class1* fct7()
  {
    return global_class1.get();
  }

  bool is_global(const Class1* c)
  {
    return global_class1.get() == c;
  }

  void setup(bool global)
  {
    Class1::last =  nullptr;
    Class1::nb_init = 0;
    Class1::nb_destroy = 0;

    if(global)
    {
      global_class1 = std::make_shared<Class1>();
    }
  }

  void teardown()
  {
    global_class1.reset();
  }
}

BOOST_AUTO_TEST_CASE( class_01 )
{
  setup(false);
  {
    luacxx::engine engine;
    luacxx::make_class<Class1>(engine, "Class1")
        .ctor();
    luacxx::make_function(engine, "fct1", fct1);
    luacxx::make_function(engine, "fct2", fct2);
    luacxx::make_function(engine, "fct3", fct3);
    luacxx::make_function(engine, "fct4", fct4);
    BOOST_CHECK(engine.bind());

    std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "class_01.lua");
    if(!ret.first)
    {
      BOOST_FAIL(ret.second);
    }
  }
  teardown();

  BOOST_CHECK(Class1::nb_init    == 1);
  BOOST_CHECK(Class1::nb_destroy == 1);
}

BOOST_AUTO_TEST_CASE( class_02 )
{
  setup(true);
  {
    luacxx::engine engine;
    luacxx::make_class<Class1>(engine, "Class1")
        .ctor();
    luacxx::make_function(engine, "fct1", fct1);
    luacxx::make_function(engine, "fct4", fct4);
    luacxx::make_function(engine, "fct5", fct5);
    luacxx::make_function(engine, "fct6", fct6)
        .get_policy()
        .get(0).delegate_owner();
    luacxx::make_function(engine, "fct7", fct7);
    luacxx::make_function(engine, "is_global", is_global);
    BOOST_CHECK(engine.bind());

    std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "class_02.lua");
    if(!ret.first)
    {
      BOOST_FAIL(ret.second);
    }
  }
  teardown();

  BOOST_CHECK(Class1::nb_init    == 3);
  BOOST_CHECK(Class1::nb_destroy == 3);
}

BOOST_AUTO_TEST_CASE( class_03 )
{
  setup(false);
  {
    luacxx::engine engine;
    luacxx::make_class<Class1>(engine, "Class1")
        .ctor();
    luacxx::make_function(engine, "fct1", fct1);
    luacxx::make_function(engine, "fct2", fct2);
    luacxx::make_function(engine, "fct3", fct3);
    luacxx::make_function(engine, "fct4", fct4);
    luacxx::make_function(engine, "fct7", fct7);
    BOOST_CHECK(engine.bind());

    std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "class_03.lua");
    if(!ret.first)
    {
      BOOST_FAIL(ret.second);
    }
  }
  teardown();

  BOOST_CHECK(Class1::nb_init    == 0);
  BOOST_CHECK(Class1::nb_destroy == 0);
}

