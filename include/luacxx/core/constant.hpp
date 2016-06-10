#ifndef LUACXX_CORE_CONSTANT_HPP
# define LUACXX_CORE_CONSTANT_HPP

namespace luacxx
{
  //
  // msg error
  constexpr auto msg_error_type_not_supported    = "type not supported";
  constexpr auto msg_error_missing_argument      = "missing argument";
  constexpr auto msg_error_invalid_integer       = "invalid integer";
  constexpr auto msg_error_invalid_number        = "invalid number";
  constexpr auto msg_error_invalid_boolean       = "invalid boolean";
  constexpr auto msg_error_invalid_function      = "invalid function";
  constexpr auto msg_error_invalid_string        = "invalid string";
  constexpr auto msg_error_invalid_container     = "invalid container";
  constexpr auto msg_error_invalid_tranformation = "invalid transformation";
  constexpr auto msg_error_invalid_enum_value    = "invalid enum value";
  constexpr auto msg_error_invalid_object        = "invalid object";
  constexpr auto msg_error_invalid_cast_object   = "invalid object cast";
  constexpr auto msg_error_object_corrupted      = "object corrupted";
  constexpr auto msg_error_null_object           = "null object";
  constexpr auto msg_error_call                  = "error during the call";
  constexpr auto msg_error_property_ro           = "error read only propertiy";
  constexpr auto msg_error_property_wo           = "error write only propertiy";
  constexpr auto msg_error_invalid_property_name = "invalid property name";

  //
  // field
  constexpr auto node_container_unary        = "data";
  constexpr auto node_container_binary_key   = "key";
  constexpr auto node_container_binary_value = "value";

  namespace detail
  {
    constexpr auto lua_field_call     = "__call";
    constexpr auto lua_field_index    = "__index";
    constexpr auto lua_field_newindex = "__newindex";
    constexpr auto lua_field_gc       = "__gc";
    constexpr auto lua_field_id       = "__id_private";
    constexpr auto lua_field_ptr      = "__ptr_private";
    constexpr auto lua_field_type     = "__type_private";
    constexpr auto lua_field_convert  = "__convert_private";
  }
}

#endif
