#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/helper/logger.hpp>

#include <config.h>

BOOST_AUTO_TEST_CASE( logger_01 )
{
  std::ostringstream stream;
  luacxx::engine engine;
  luacxx::make_logger(engine, "info",  luacxx::make_basic_stream_logger(stream, "info "));
  luacxx::make_logger(engine, "error", luacxx::make_basic_stream_logger(stream, "error"));
  BOOST_CHECK(engine.bind());

  std::pair<bool, std::string> ret = engine.do_file(LUACXX_TESTS_DIR "logger_01.lua");
  if(!ret.first)
  {
    BOOST_FAIL(ret.second);
  }

  std::ostringstream ref_stream;
  ref_stream << "info :" << LUACXX_TESTS_DIR << "logger_01.lua(1):test ty tgtg" << std::endl;
  ref_stream << "error:" << LUACXX_TESTS_DIR << "logger_01.lua(2):agggggg" << std::endl;

  BOOST_CHECK_EQUAL(stream.str(), ref_stream.str());
}


