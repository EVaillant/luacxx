#define BOOST_TEST_DYN_LINK
#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <string>

#include <luacxx/class.hpp>
#include <luacxx/module.hpp>
#include <luacxx/debug.hpp>

#include "test_luacxx.hpp"

#include <config.h>

namespace
{
  class Cls01
  {
    public:
      static int instance_count;

      Cls01()
        : x_(0)
      {
        ++instance_count;
      }

      Cls01(int x)
        : x_(x)
      {
        ++instance_count;
      }

      ~Cls01()
      {
        --instance_count;
      }

      const int& get_x() const
      {
        return x_;
      }

      void set_x(int x)
      {
        x_ = x;
      }

      std::string get_str() const
      {
        return str_;
      }

      void set_str(const std::string& str)
      {
        str_ = str;
      }

      int super_mth1(int a, int b, const std::string &c)
      {
        str_ = c;
        return a + b;
      }

      int super_mth2(int a, int b, std::string &c)
      {
        c = "truc";
        return a + b;
      }

      int super_mth3(std::string &c)
      {
        c = "truc";
        return 5;
      }

      int x_;
      std::string str_;
  };
  int Cls01::instance_count = 0;

  std::unique_ptr<Cls01> global_ref;
  class Cls02
  {
    public:
      static int instance_count;

      Cls02()
      {
        ++instance_count;
      }

      ~Cls02()
      {
        --instance_count;
      }

      std::shared_ptr<Cls01> create1()
      {
        return std::shared_ptr<Cls01>(new Cls01);
      }

      void create2(std::shared_ptr<Cls01>& out)
      {
        out = std::shared_ptr<Cls01>(new Cls01);
      }

      void create3(Cls01&)
      {
      }

      void create4(Cls01*)
      {
      }

      Cls01* out_ptr1()
      {
        if(!global_ref) global_ref.reset(new Cls01);
        return global_ref.get();
      }

      const Cls01* out_ptr2()
      {
        if(!global_ref) global_ref.reset(new Cls01);
        return global_ref.get();
      }

      Cls01* out_ptr3()
      {
        return new Cls01();
      }

      Cls01& out_ref1()
      {
        if(!global_ref) global_ref.reset(new Cls01);
        return *global_ref;
      }

      const Cls01& out_ref2()
      {
        if(!global_ref) global_ref.reset(new Cls01);
        return *global_ref;
      }

      Cls01& out_ref3()
      {
        return *new Cls01();
      }
  };
  int Cls02::instance_count = 0;

  class Cls03
  {
    public:
      static int instance_count;

      Cls03()
        : ref_(nullptr)
      {
        ++instance_count;
      }

      ~Cls03()
      {
        --instance_count;
      }

      void in_ptr1(const std::shared_ptr<Cls01> &p)
      {
        ref_ = p.get();
      }

      void in_ptr2(const Cls01 &p)
      {
        ref_ = &p;
      }

      void in_ptr3(const Cls01* p)
      {
        ref_ = p;
      }

      bool is_ref(const Cls01* cls) const
      {
        return ref_ == cls;
      }

      const Cls01* ref_;
  };
  int Cls03::instance_count = 0;

  void init_instance_counter()
  {
    Cls01::instance_count = 0;
    Cls02::instance_count = 0;
    Cls03::instance_count = 0;
  }

  void check_all_cleanup()
  {
    global_ref.reset();

    BOOST_CHECK_EQUAL(0, Cls01::instance_count);
    BOOST_CHECK_EQUAL(0, Cls02::instance_count);
    BOOST_CHECK_EQUAL(0, Cls03::instance_count);
  }
}

BOOST_AUTO_TEST_CASE( class_01 )
{
  init_instance_counter();
  {
    lua_State *state = luaL_newstate();
    luaL_openlibs(state);

    luacxx::RegisterType registry;
    luacxx::GlobalModule module;

    luacxx::Class<Cls01>& cls_wrapper = helpers::make_class<Cls01>(registry, module, "Cls01");
    cls_wrapper
        .ctor<>()
        .const_method("get_x", &Cls01::get_x)
        .method("set_x",       &Cls01::set_x);

    module.bind(state);

    if(luaL_dofile(state, LUACXX_TESTS_DIR "class_01.lua") != LUA_OK)
    {
      BOOST_FAIL(lua_tostring(state, -1));
    }

    lua_close(state);
  }
  check_all_cleanup();
}

BOOST_AUTO_TEST_CASE( class_02 )
{
  init_instance_counter();
  {
    lua_State *state = luaL_newstate();
    luaL_openlibs(state);

    luacxx::RegisterType registry;
    luacxx::GlobalModule module;

    luacxx::Class<Cls01>& cls_wrapper = helpers::make_class<Cls01>(registry, module, "Cls01");
    cls_wrapper
        .ctor<>()
        .const_method("get_x",   &Cls01::get_x)
        .method("set_x",         &Cls01::set_x)
        .const_method("get_str", &Cls01::get_str)
        .method("set_str",       &Cls01::set_str)
        .method("super_mth1",    &Cls01::super_mth1, luacxx::custum_policy())
          .get<1>().set_default_value(5)
          .get<2>().set_default_value(15)
          .get<3>().set_default_value("blabla")
          .get_owner()
        .method("super_mth1_wo_return", &Cls01::super_mth1, luacxx::custum_policy())
          .get<0>().discard_return()
          .get_owner()
        .method("super_mth2", &Cls01::super_mth2)
        .method("super_mth3", &Cls01::super_mth3);

    module.bind(state);

    if(luaL_dofile(state, LUACXX_TESTS_DIR "class_02.lua") != LUA_OK)
    {
      BOOST_FAIL(lua_tostring(state, -1));
    }

    lua_close(state);
  }
  check_all_cleanup();
}

BOOST_AUTO_TEST_CASE( class_03 )
{
  init_instance_counter();
  {
    lua_State *state = luaL_newstate();
    luaL_openlibs(state);

    luacxx::RegisterType registry;
    luacxx::GlobalModule module;

    luacxx::Class<Cls01>& cls_wrapper1 = helpers::make_class<Cls01>(registry, module, "Cls01");
    cls_wrapper1
        .ctor<>()
        .const_method("get_x", &Cls01::get_x)
        .method("set_x",       &Cls01::set_x);

    luacxx::Class<Cls02>& cls_wrapper2 = helpers::make_class<Cls02>(registry, module, "Cls02");
    cls_wrapper2
        .ctor<>()
        .method("create1",  &Cls02::create1)
        .method("create2",  &Cls02::create2)
        .method("create3",  &Cls02::create3)
        .method("create4",  &Cls02::create4)
        .method("out_ptr1", &Cls02::out_ptr1)
        .method("out_ptr2", &Cls02::out_ptr2)
        .method("out_ptr3", &Cls02::out_ptr3, luacxx::custum_policy())
          .get<0>().delegate_owner()
          .get_owner()
        .method("out_ref1", &Cls02::out_ref1)
        .method("out_ref2", &Cls02::out_ref2)
        .method("out_ref3", &Cls02::out_ref3, luacxx::custum_policy())
          .get<0>().delegate_owner()
          .get_owner();

    module.bind(state);

    if(luaL_dofile(state, LUACXX_TESTS_DIR "class_03.lua") != LUA_OK)
    {
      BOOST_FAIL(lua_tostring(state, -1));
    }

    lua_close(state);
  }
  check_all_cleanup();
}

BOOST_AUTO_TEST_CASE( class_04 )
{
  init_instance_counter();
  {
    lua_State *state = luaL_newstate();
    luaL_openlibs(state);

    luacxx::RegisterType registry;
    luacxx::GlobalModule module;

    luacxx::Class<Cls01>& cls_wrapper1 = helpers::make_class<Cls01>(registry, module, "Cls01");
    cls_wrapper1
        .ctor<>()
        .const_method("get_x", &Cls01::get_x)
        .method("set_x", &Cls01::set_x);

    luacxx::Class<Cls03>& cls_wrapper2 = helpers::make_class<Cls03>(registry, module, "Cls03");
    cls_wrapper2
        .ctor<>()
        .method("in_ptr1",      &Cls03::in_ptr1)
        .method("in_ptr2",      &Cls03::in_ptr2)
        .method("in_ptr3",      &Cls03::in_ptr3)
        .const_method("is_ref", &Cls03::is_ref);

    module.bind(state);

    if(luaL_dofile(state, LUACXX_TESTS_DIR "class_04.lua") != LUA_OK)
    {
      BOOST_FAIL(lua_tostring(state, -1));
    }

    lua_close(state);
  }
  check_all_cleanup();
}
