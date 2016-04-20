#ifndef LUACXX_UTILITY_HPP
# define LUACXX_UTILITY_HPP

# include <toolsbox/any.hpp>
# include <luacxx/detail/arg_call.hpp>
# include <luacxx/constant.hpp>

namespace luacxx
{
  template <class T> auto cast_arg_call(toolsbox::any& value) -> decltype(detail::arg_call<T>::convert(value))
  {
    return detail::arg_call<T>::convert(value);
  }

  template <class T> bool check_arg_call(std::string& msg, const toolsbox::any& value)
  {
    bool error = !detail::arg_call<T>::check(value);
    if( error )
    {
      msg = msg_error_invalid_tranformation;
    }
    return error;
  }
}

#endif
