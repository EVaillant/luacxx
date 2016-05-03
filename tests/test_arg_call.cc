#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/core/utility.hpp>

BOOST_AUTO_TEST_CASE( arg_call_int_ptr )
{
  typedef int* type;
  std::string msg;

  {
    toolsbox::any a(5);
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int y = 5;
    toolsbox::any a(std::ref(y));
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int y = 5;
    toolsbox::any a(&y);
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int  y = 5;
    int* o = &y;
    toolsbox::any a(std::ref(o));
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    std::shared_ptr<int> b = std::make_shared<int>(5);
    toolsbox::any a(b);
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    std::shared_ptr<int> b = std::make_shared<int>(5);
    toolsbox::any a(std::ref(b));
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }
}

BOOST_AUTO_TEST_CASE( arg_call_const_int_ptr )
{
  typedef const int* type;
  std::string msg;

  {
    int y = 5;
    toolsbox::any a(std::cref(y));
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int  y = 5;
    int* o = &y;
    toolsbox::any a(std::cref(o));

    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int  y = 5;
    const int* o = &y;
    toolsbox::any a(std::cref(o));

    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    std::shared_ptr<int> b = std::make_shared<int>(5);
    toolsbox::any a(std::cref(b));
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    toolsbox::any a(5);
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int y = 5;
    toolsbox::any a(std::ref(y));
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int y = 5;
    toolsbox::any a(&y);
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    const int y = 5;
    toolsbox::any a(&y);
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int  y = 5;
    int* o = &y;
    toolsbox::any a(std::ref(o));
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    std::shared_ptr<int> b = std::make_shared<int>(5);
    toolsbox::any a(b);
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    std::shared_ptr<int> b = std::make_shared<int>(5);
    toolsbox::any a(std::ref(b));
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }
}

BOOST_AUTO_TEST_CASE( arg_call_smart_int )
{
  typedef std::shared_ptr<int> type;
  std::string msg;

  {
    std::shared_ptr<int> y = std::make_shared<int>(5);
    toolsbox::any a = y;

    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    std::shared_ptr<int> y = std::make_shared<int>(5);
    toolsbox::any a = std::ref(y);

    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    std::shared_ptr<int> y = std::make_shared<int>(5);
    toolsbox::any a = std::cref(y);

    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(*luacxx::cast_arg_call<type>(a) == 5);
  }
}

BOOST_AUTO_TEST_CASE( arg_call_int )
{
  typedef int type;
  std::string msg;

  {
    int y = 5;
    toolsbox::any a(y);
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int y = 5;
    toolsbox::any a(std::ref(y));
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int  y = 5;
    int* o = &y;
    toolsbox::any a(o);
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int  y = 5;
    int* o = &y;
    toolsbox::any a(std::ref(o));
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    std::shared_ptr<int> y = std::make_shared<int>(5);
    toolsbox::any a = y;

    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    std::shared_ptr<int> y = std::make_shared<int>(5);
    toolsbox::any a = std::ref(y);

    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(luacxx::cast_arg_call<type>(a) == 5);
  }
}

BOOST_AUTO_TEST_CASE( arg_call_const_int )
{
  typedef const int type;
  std::string msg;

  {
    int y = 5;
    toolsbox::any a(y);
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int y = 5;
    toolsbox::any a(std::ref(y));
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int y = 5;
    toolsbox::any a(std::cref(y));
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int  y = 5;
    int* o = &y;
    toolsbox::any a(o);
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int  y = 5;
    int* o = &y;
    toolsbox::any a(std::ref(o));
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int  y = 5;
    int* o = &y;
    toolsbox::any a(std::cref(o));
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int        y = 5;
    const int* o = &y;
    toolsbox::any a(o);
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int        y = 5;
    const int* o = &y;
    toolsbox::any a(std::ref(o));
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    int        y = 5;
    const int* o = &y;
    toolsbox::any a(std::cref(o));
    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    std::shared_ptr<int> y = std::make_shared<int>(5);
    toolsbox::any a = y;

    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    std::shared_ptr<int> y = std::make_shared<int>(5);
    toolsbox::any a = std::ref(y);

    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(luacxx::cast_arg_call<type>(a) == 5);
  }

  {
    std::shared_ptr<int> y = std::make_shared<int>(5);
    toolsbox::any a = std::cref(y);

    BOOST_CHECK(!luacxx::check_arg_call<type>(msg, a));
    BOOST_CHECK(luacxx::cast_arg_call<type>(a) == 5);
  }
}
