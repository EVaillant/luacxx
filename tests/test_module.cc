#define BOOST_TEST_DYN_LINK
#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <string>

#include <luacxx/module.hpp>
#include <luacxx/debug.hpp>

#include "test_luacxx.hpp"

#include <config.h>

BOOST_AUTO_TEST_CASE( module_01 )
{
  std::tuple<std::string, std::string> t_split_01 = luacxx::split_full_type_name("Popo");
  BOOST_CHECK_EQUAL("",     std::get<0>(t_split_01));
  BOOST_CHECK_EQUAL("Popo", std::get<1>(t_split_01));

  std::tuple<std::string, std::string> t_split_02 = luacxx::split_full_type_name("popo.kkkk.Popo");
  BOOST_CHECK_EQUAL("popo.kkkk", std::get<0>(t_split_02));
  BOOST_CHECK_EQUAL("Popo",      std::get<1>(t_split_02));
}
