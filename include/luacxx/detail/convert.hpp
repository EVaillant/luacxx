#ifndef LUACXX_DETAIL_CONVERT_HPP
# define LUACXX_DETAIL_CONVERT_HPP

# include <toolsbox/any.hpp>

# include <luacxx/parameter_policy.hpp>
# include <luacxx/error_msg.hpp>

# include <luacxx/detail/lookup_type.hpp>
# include <luacxx/detail/type_traits.hpp>

namespace luacxx
{
  namespace detail
  {
    template <class T> struct convert
    {
      typedef T type;
      typedef typename register_type<type>::type register_type;
      typedef toolsbox::any                      variable_type;

      static void from(lua_State *state, const lookup_type& registry, std::size_t idx, variable_type &var, std::string& error_msg, const policy_node& policy)
      {
        const type_info<register_type>& type_info = registry.template get<register_type>();
        if(type_info.valid())
        {
          type_info.from_lua(state, idx, var, error_msg, policy);
        }
        else
        {
          error_msg = msg_error_type_not_supported;
        }
      }

      static void to(lua_State *state, const lookup_type& registry, const variable_type& var, std::string& error_msg, const policy_node& policy)
      {
        const type_info<register_type>& type_info = registry.template get<register_type>();
        if(type_info.valid())
        {
          type_info.to_lua(state, var, error_msg, policy);
        }
        else
        {
          error_msg = msg_error_type_not_supported;
        }
      }
    };
  }
}

#endif

