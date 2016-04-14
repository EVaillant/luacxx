#ifndef LUACXX_UTILITY_HPP
# define LUACXX_UTILITY_HPP

# include <toolsbox/any.hpp>
# include <luacxx/detail/arg_call.hpp>
# include <luacxx/error_msg.hpp>

namespace luacxx
{
  template <class T> auto cast_arg_call(toolsbox::any& value) -> decltype(detail::arg_call<T>::convert(value))
  {
    return detail::arg_call<T>::convert(value);
  }

  template <class T> void check_arg_call(std::string& msg, toolsbox::any& value)
  {
    if(! detail::arg_call<T>::check(value))
    {
      msg = msg_error_invalid_tranformation;
    }
  }
}

#endif
