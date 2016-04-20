#ifndef LUACXX_CONVERT_HPP
# define LUACXX_CONVERT_HPP

# include <luacxx/state.hpp>

# include <luacxx/detail/type_traits.hpp>
# include <luacxx/detail/convert.hpp>

namespace toolsbox
{
  class any;
}

namespace luacxx
{
  class lookup_type;
  class policy_node;

  template <class T> bool convert_from(state_type state, const lookup_type& registry, std::size_t idx, toolsbox::any &var, std::string& error_msg, const policy_node& policy)
  {
    typedef typename detail::register_type<T>::type  register_type;
    detail::convert<register_type>::from(state, registry, idx, var, error_msg, policy);
    return error_msg.empty();
  }

  template <class T> bool convert_to(state_type state, const lookup_type& registry, toolsbox::any& var, std::string& error_msg, const policy_node& policy)
  {
    typedef typename detail::register_type<T>::type  register_type;
    detail::convert<register_type>::to(state, registry, var, error_msg, policy);
    return error_msg.empty();
  }
}

#endif
