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
  class ClassA;
  class ClassB;
  class ClassC;
  class ClassD;
  class ClassE;

  Cls1*   last_cls1;
  ClassA* last_clsA;
  ClassB* last_clsB;
  ClassC* last_clsC;
  ClassD* last_clsD;
  ClassE* last_clsE;

  int counter_A;
  int counter_B;
  int counter_C;
  int counter_D;
  int counter_E;

  void init()
  {
    last_cls1 = nullptr;
    last_clsA = nullptr;
    last_clsB = nullptr;
    last_clsC = nullptr;
    last_clsD = nullptr;
    last_clsE = nullptr;
    counter_A = 0;
    counter_B = 0;
    counter_C = 0;
    counter_D = 0;
    counter_E = 0;
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

  class ClassA
  {
    public:
      ClassA()
      {
        last_clsA = this;
      }

      int methdA()
      {
        return ((last_clsA == this) ?  counter_A++ : -1);
      }
  };

  class ClassB : public ClassA
  {
    public:
      ClassB()
      {
        last_clsB = this;
      }

      int methdB()
      {
        return ((last_clsB == this) ?  counter_B++ : -1);
      }
  };

  class ClassC: public ClassB
  {
    public:
      ClassC()
      {
        last_clsC = this;
      }

      int methdC()
      {
        return ((last_clsC == this) ?  counter_C++ : -1);
      }
  };

  class ClassD
  {
    public:
      ClassD()
      {
        last_clsD = this;
      }

      int methdD()
      {
        return ((last_clsD == this) ?  counter_D++ : -1);
      }
  };

  class ClassE : public ClassD, public ClassC
  {
    public:
      ClassE()
      {
        last_clsE = this;
      }

      int methdE()
      {
        return ((last_clsE == this) ?  counter_E++ : -1);
      }
  };
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

BOOST_AUTO_TEST_CASE( method_class_02 )
{
  init();
  {
    luacxx::engine engine;
    luacxx::make_class<ClassA>(engine, "ClassA")
        .ctor()
        .method("methdA", &ClassA::methdA);
    luacxx::make_class<ClassB>(engine, "ClassB")
        .ctor()
        .inheritance<ClassA>()
        .method("methdB", &ClassB::methdB);
    luacxx::make_class<ClassC>(engine, "ClassC")
        .ctor()
        .inheritance<ClassB>()
        .method("methdC", &ClassC::methdC);
    BOOST_CHECK(engine.bind());

    std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "method_class_02.lua");
    if(!ret.first)
    {
      BOOST_FAIL(ret.second);
    }
    else
    {
      BOOST_CHECK_EQUAL(counter_A, 3);
      BOOST_CHECK_EQUAL(counter_B, 2);
      BOOST_CHECK_EQUAL(counter_C, 1);
    }
  }
}

BOOST_AUTO_TEST_CASE( method_class_03 )
{
  init();
  {
    luacxx::engine engine;
    luacxx::make_class<ClassA>(engine, "ClassA")
        .ctor()
        .method("methdA", &ClassA::methdA);
    luacxx::make_class<ClassB>(engine, "ClassB")
        .ctor()
        .inheritance<ClassA>()
        .method("methdB", &ClassB::methdB);
    luacxx::make_class<ClassC>(engine, "ClassC")
        .ctor()
        .inheritance<ClassB>()
        .method("methdC", &ClassC::methdC);
    luacxx::make_class<ClassD>(engine, "ClassD")
        .ctor()
        .method("methdD", &ClassD::methdD);
    luacxx::make_class<ClassE>(engine, "ClassE")
        .ctor()
        .inheritance<ClassD>()
        .inheritance<ClassC>()
        .method("methdE", &ClassE::methdE);
    BOOST_CHECK(engine.bind());

    std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "method_class_03.lua");
    if(!ret.first)
    {
      BOOST_FAIL(ret.second);
    }
    else
    {
      BOOST_CHECK_EQUAL(counter_A, 1);
      BOOST_CHECK_EQUAL(counter_B, 1);
      BOOST_CHECK_EQUAL(counter_C, 1);
      BOOST_CHECK_EQUAL(counter_D, 1);
      BOOST_CHECK_EQUAL(counter_E, 1);
    }
  }
}


