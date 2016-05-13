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
  class BaseA;
  class BaseB;
  BaseA* last_a;
  BaseB* last_b;

  void init()
  {
    last_a  = nullptr;
    last_b  = nullptr;
  }

  class BaseA
  {
    public:
      BaseA()
      {
        last_a = this;
      }

      virtual ~BaseA()
      {
      }
  };

  class BaseB
  {
    public:
      BaseB()
      {
        last_b = this;
      }

      virtual ~BaseB()
      {
      }
  };

  class DerivatedA : public BaseA
  {
    public:
      DerivatedA()
      {
      }
  };

  class DerivatedDerivatedA : public DerivatedA
  {
    public:
      DerivatedDerivatedA()
      {
      }
  };

  class DerivatedDerivatedAB : public DerivatedA, public BaseB
  {
    public:
      DerivatedDerivatedAB()
      {
      }
  };

  bool fct1(BaseA* a)
  {
    return a == last_a;
  }

  bool fct2(DerivatedA* a)
  {
    return a == last_a;
  }

  bool fct3(DerivatedDerivatedA* a)
  {
    return a == last_a;
  }

  bool fct4(BaseB* b)
  {
    return b == last_b;
  }
}

BOOST_AUTO_TEST_CASE( inheritance_class_01 )
{
  init();

  luacxx::engine engine;
  luacxx::make_class<BaseA>(engine, "BaseA");
  luacxx::make_class<DerivatedA>(engine, "DerivatedA")
      .ctor()
      .inheritance<BaseA>();
  luacxx::make_class<DerivatedDerivatedA>(engine, "DerivatedDerivatedA")
      .inheritance<DerivatedA>()
      .ctor();
  luacxx::make_function(engine, "fct1", fct1);
  luacxx::make_function(engine, "fct2", fct2);
  luacxx::make_function(engine, "fct3", fct3);
  BOOST_CHECK(engine.bind());

  std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "inheritance_class_01.lua");
  if(!ret.first)
  {
    BOOST_FAIL(ret.second);
  }
}

BOOST_AUTO_TEST_CASE( inheritance_class_02 )
{
  init();

  luacxx::engine engine;
  luacxx::make_class<BaseA>(engine, "BaseA");
  luacxx::make_class<BaseB>(engine, "BaseB");
  luacxx::make_class<DerivatedA>(engine, "DerivatedA")
      .ctor()
      .inheritance<BaseA>();
  luacxx::make_class<DerivatedDerivatedA>(engine, "DerivatedDerivatedA")
      .inheritance<DerivatedA>()
      .ctor();
  luacxx::make_class<DerivatedDerivatedAB>(engine, "DerivatedDerivatedAB")
      .inheritance<DerivatedA>()
      .inheritance<BaseB>()
      .ctor();
  luacxx::make_function(engine, "fct1", fct1);
  luacxx::make_function(engine, "fct2", fct2);
  luacxx::make_function(engine, "fct3", fct3);
  luacxx::make_function(engine, "fct4", fct4);
  BOOST_CHECK(engine.bind());

  std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "inheritance_class_02.lua");
  if(!ret.first)
  {
    BOOST_FAIL(ret.second);
  }
}

