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
  class Cls1;
  Cls1* last_cls1;

  void init()
  {
    last_cls1 = nullptr;
  }

  class Cls1
  {
    public:
      Cls1()
      {
        last_cls1 = this;
      }

      bool meth1(int a, const std::string& b)
      {
        return (a == 5 && b == "blabla" && this == last_cls1);
      }

      bool meth2(int a, const std::string& b) const
      {
        return (a == 7 && b == "blabla" && this == last_cls1);
      }
  };

  bool extern_meth1(Cls1* cls, int a, const std::string& b)
  {
    return (a ==  9 && b == "blabla"  && cls == last_cls1);
  }
}

BOOST_AUTO_TEST_CASE( method_class_01 )
{
  init();
  {
    luacxx::engine engine;
    luacxx::make_class<Cls1>(engine, "Cls1")
        .ctor()
        .method("meth1", &Cls1::meth1)
        .const_method("meth2", &Cls1::meth2)
        .extern_method("meth3", extern_meth1)
        .extern_method("meth3", extern_meth1)
        .extern_method("meth4", std::function< bool (Cls1*, int, const std::string&)>(extern_meth1));
    BOOST_CHECK(engine.bind());

    std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "method_class_01.lua");
    if(!ret.first)
    {
      BOOST_FAIL(ret.second);
    }
  }
}

