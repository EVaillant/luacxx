#ifndef LUACXX_HELPER_CLASS_HPP
# define LUACXX_HELPER_CLASS_HPP

# include <luacxx/binding/function.hpp>

namespace luacxx
{
  namespace detail
  {
    inline bool is_same_class_type(engine& e)
    {
      state_type state = e.get_state();
      std::pair<bool, common_class_type_info::class_field> left  = common_class_type_info::get_class_field(state, 1);
      std::pair<bool, common_class_type_info::class_field> right = common_class_type_info::get_class_field(state, 2);
      return(left.first && right.first) && (left.second.id ==  right.second.id);
    }

    inline bool is_same_class_object(engine& e)
    {
      state_type state = e.get_state();
      std::pair<bool, common_class_type_info::class_field> left  = common_class_type_info::get_class_field(state, 1);
      std::pair<bool, common_class_type_info::class_field> right = common_class_type_info::get_class_field(state, 2);
      return(left.first && right.first) && (left.second.id ==  right.second.id) && (left.second.ptr ==  right.second.ptr);
    }
  }

  inline auto& make_is_same_class_type(engine& e, const std::string& module_name, const std::string& name)
  {
    return make_function(e, module_name, name, std::function<bool ()>(std::bind(detail::is_same_class_type, std::ref(e))));
  }

  inline auto& make_is_same_class_type(engine& e, const std::string& name)
  {
    return make_is_same_class_type(e, root_module_name, name);
  }

  inline auto& make_is_same_class_object(engine& e, const std::string& module_name, const std::string& name)
  {
    return make_function(e, module_name, name, std::function<bool ()>(std::bind(detail::is_same_class_object, std::ref(e))));
  }

  inline auto& make_is_same_class_object(engine& e, const std::string& name)
  {
    return make_is_same_class_object(e, root_module_name, name);
  }
}

#endif
