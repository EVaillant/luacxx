#ifndef LUACXX_CORE_TYPE_INFO_HPP
# define LUACXX_CORE_TYPE_INFO_HPP

# include <toolsbox/any.hpp>
# include <luacxx/core/constant.hpp>
# include <luacxx/core/type_traits.hpp>

namespace luacxx
{
  class policy_node;

  class common_type_info
  {
    public:
      typedef toolsbox::any variable_type;

      enum class underlying_type
      {
        Class,
        Enum,
        Native,
        Unknown
      };

      common_type_info(underlying_type ut)
        : underlying_type_(ut)
      {
      }

      virtual ~common_type_info()
      {
      }

      underlying_type get_underlying_type() const
      {
        return underlying_type_;
      }

      bool valid() const
      {
        return underlying_type_ != underlying_type::Unknown;
      }

      virtual void to_lua(state_type state, variable_type& var, std::string &error_msg, const policy_node& policy) const = 0;
      virtual void from_lua(state_type state, std::size_t idx, variable_type& var, std::string& error_msg, const policy_node& policy) const = 0;

    protected:
      underlying_type underlying_type_;
  };

  template <class T> class type_info : public common_type_info
  {
    public:
      typedef T type;

      type_info(underlying_type ut)
        : common_type_info(ut)
      {
      }
  };

  template <class T> class empty_type_info : public type_info<T>
  {
    public:
      typedef T type;
      typedef common_type_info::variable_type variable_type;

      empty_type_info()
        : type_info<T>(common_type_info::underlying_type::Unknown)
      {
      }

      virtual void to_lua(state_type, variable_type&, std::string &error_msg, const policy_node&) const override
      {
        error_msg = msg_error_type_not_supported;
      }

      virtual void from_lua(state_type, std::size_t, variable_type& , std::string& error_msg, const policy_node&) const override
      {
        error_msg = msg_error_type_not_supported;
      }
  };
}

#endif
