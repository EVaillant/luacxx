#ifndef LUACXX_CONSTANT_HPP
# define LUACXX_CONSTANT_HPP

namespace luacxx
{
  //
  // msg error
  constexpr auto msg_error_type_not_supported    = "type not supported";
  constexpr auto msg_error_missing_argument      = "missing argument";
  constexpr auto msg_error_invalid_integer       = "invalid integer";
  constexpr auto msg_error_invalid_number        = "invalid number";
  constexpr auto msg_error_invalid_boolean       = "invalid boolean";
  constexpr auto msg_error_invalid_string        = "invalid string";
  constexpr auto msg_error_invalid_container     = "invalid container";
  constexpr auto msg_error_invalid_tranformation = "invalid transformation";

  //
  // field
  constexpr auto node_container_unary        = "data";
  constexpr auto node_container_binary_key   = "key";
  constexpr auto node_container_binary_value = "value";
}

#endif
