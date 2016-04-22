#define BOOST_TEST_DYN_LINK
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>

#include <luacxx/lookup_type.hpp>

BOOST_TEST_DONT_PRINT_LOG_VALUE(luacxx::common_type_info::underlying_type);

namespace
{
  class blabla
  {
  };

  class blabla_type_info : public luacxx::type_info<blabla>
  {
    public:
      blabla_type_info()
        : type_info<blabla>(luacxx::common_type_info::underlying_type::Class)
      {
      }

      virtual void to_lua(luacxx::state_type, luacxx::common_type_info::variable_type&, std::string &, const luacxx::policy_node&) const override
      {
      }

      virtual void from_lua(luacxx::state_type, std::size_t, common_type_info::variable_type& , std::string&, const luacxx::policy_node&) const override
      {
      }
  };

  class int_type_info : public luacxx::type_info<int>
  {
    public:
      int_type_info()
        : type_info<int>(luacxx::common_type_info::underlying_type::Native)
      {
      }

      virtual void to_lua(luacxx::state_type, luacxx::common_type_info::variable_type&, std::string &, const luacxx::policy_node&) const override
      {
      }

      virtual void from_lua(luacxx::state_type, std::size_t, common_type_info::variable_type& , std::string&, const luacxx::policy_node&) const override
      {
      }
  };
}

BOOST_AUTO_TEST_CASE(lookup_type_01)
{
  luacxx::lookup_type lookup;

  BOOST_CHECK(!lookup.exist<blabla>());
  BOOST_CHECK(!lookup.exist<int>());

  std::shared_ptr<blabla_type_info> ti1 = std::make_shared<blabla_type_info>();
  std::shared_ptr<int_type_info>    ti2 = std::make_shared<int_type_info>();

  void* p1 = ti1.get();
  void* p2 = ti2.get();

  lookup.set<blabla>(std::move(ti1));
  lookup.set<int>(std::move(ti2));

  BOOST_CHECK(lookup.exist<blabla>());
  BOOST_CHECK(lookup.exist<int>());

  BOOST_CHECK_EQUAL(p1, &lookup.get<blabla>());
  BOOST_CHECK_EQUAL(p2, &lookup.get<int>());

  BOOST_CHECK_EQUAL(luacxx::common_type_info::underlying_type::Native, lookup.get<int>().get_underlying_type());
  BOOST_CHECK_EQUAL(luacxx::common_type_info::underlying_type::Class,  lookup.get<blabla>().get_underlying_type());

  BOOST_CHECK(lookup.get<int>().valid());
  BOOST_CHECK(lookup.get<blabla>().valid());

  lookup.reset<blabla>();
  lookup.reset<int>();

  BOOST_CHECK(!lookup.exist<blabla>());
  BOOST_CHECK(!lookup.exist<int>());
}

BOOST_AUTO_TEST_CASE(lookup_type_02)
{
  luacxx::lookup_type lookup;

  BOOST_CHECK(!lookup.exist<int>());
  BOOST_CHECK_EQUAL(luacxx::common_type_info::underlying_type::Unknown, lookup.get<int>().get_underlying_type());
  BOOST_CHECK(!lookup.get<int>().valid());
}
