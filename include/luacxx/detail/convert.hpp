#ifndef LUACXX_DETAIL_CONVERT_HPP
# define LUACXX_DETAIL_CONVERT_HPP

# include <toolsbox/any.hpp>

# include <luacxx/constant.hpp>
# include <luacxx/lookup_type.hpp>
# include <luacxx/state.hpp>
# include <luacxx/utility.hpp>

# include <list>
# include <vector>
# include <set>

# include <lua.hpp>

namespace luacxx
{
  template <class T> bool convert_from(state_type state, const lookup_type& registry, std::size_t idx, toolsbox::any &var, std::string& error_msg, const policy_node& policy);
  template <class T> bool convert_to(state_type state, const lookup_type& registry, toolsbox::any& var, std::string& error_msg, const policy_node& policy);

  namespace detail
  {
    //
    // default case
    template <class T> struct convert
    {
      typedef T             type;
      typedef toolsbox::any variable_type;

      static void from(state_type state, const lookup_type& registry, std::size_t idx, variable_type &var, std::string& error_msg, const policy_node& policy)
      {
        const type_info<type>& type_info = registry.template get<type>();
        if(type_info.valid())
        {
          type_info.from_lua(state, idx, var, error_msg, policy);
        }
        else
        {
          error_msg = msg_error_type_not_supported;
        }
      }

      static void to(state_type state, const lookup_type& registry, variable_type& var, std::string& error_msg, const policy_node& policy)
      {
        const type_info<type>& type_info = registry.template get<type>();
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

    //
    // container unary
    template <class C, class I = typename C::value_type> struct container_unary_convert
    {
      typedef C             type;
      typedef I             value_type;
      typedef toolsbox::any variable_type;

      static void from(state_type state, const lookup_type& registry, std::size_t idx, variable_type &var, std::string& error_msg, const policy_node& policy)
      {
        if(lua_istable(state, idx))
        {
          var       = type();
          type &tmp = var.as<type>();
          std::insert_iterator<type> inserter(tmp, std::begin(tmp));
          lua_pushnil(state);
          const policy_node& sub_node = policy.get_sub_node(node_container_unary);
          while (lua_next(state, idx) && error_msg.empty())
          {
            variable_type elt;
            convert_from<value_type>(state, registry, lua_gettop(state), elt, error_msg, sub_node);
            if(error_msg.empty() && !check_arg_call<value_type>(error_msg, elt))
            {
              inserter = cast_arg_call<value_type>(elt);
            }
          }
          lua_remove(state, idx);
        }
        else
        {
          error_msg = msg_error_invalid_container;
        }
      }

      static void to(state_type state, const lookup_type& registry, variable_type& var, std::string& error_msg, const policy_node& policy)
      {
        std::size_t index = 1;
        lua_newtable(state);
        if(!check_arg_call<type>(error_msg, var))
        {
          const policy_node& sub_node = policy.get_sub_node(node_container_unary);
          for(value_type elt : cast_arg_call<type>(var))
          {
            lua_pushinteger(state, index++);
            variable_type elt_any  = std::ref(elt);
            convert_to<value_type>(state, registry, elt_any, error_msg, sub_node);
            if(error_msg.empty())
            {
              lua_settable(state, -3);
            }
            else
            {
              break;
            }
          }
        }
      }
    };

    template <class T, class A> struct convert<std::list<T, A>>   : container_unary_convert<std::list<T, A>> {};
    template <class T, class A> struct convert<std::vector<T, A>> : container_unary_convert<std::vector<T, A>> {};
    template <class T, class A> struct convert<std::set<T, A>>    : container_unary_convert<std::set<T, A>> {};

    //
    // container binary
    template <class C, class K = typename C::key_type, class V = typename C::mapped_type> struct container_binary_convert
    {
      typedef C             type;
      typedef K             key_type;
      typedef V             value_type;
      typedef toolsbox::any variable_type;

      static void from(state_type state, const lookup_type& registry, std::size_t idx, variable_type &var, std::string& error_msg, const policy_node& policy)
      {
        if(lua_istable(state, idx))
        {
          var       = type();
          type &tmp = var.as<type>();
          std::insert_iterator<type> inserter(tmp, std::begin(tmp));
          const policy_node& key_node   = policy.get_sub_node(node_container_binary_key);
          const policy_node& value_node = policy.get_sub_node(node_container_binary_value);
          lua_pushnil(state);
          while (lua_next(state, idx) && error_msg.empty())
          {
            key_type   key;
            value_type value;

            if(!convert_from<value_type>(state, registry, lua_gettop(state), value, error_msg, value_node))
            {
              lua_pushvalue(state, lua_gettop(state));
              convert_from<key_type>(state, registry, lua_gettop(state), key,   error_msg, key_node);
            }

            if(error_msg.empty() && !check_arg_call<key_type>(error_msg, key) && !check_arg_call<value_type>(error_msg, value))
            {
              inserter = std::make_pair(cast_arg_call<key_type>(key), cast_arg_call<value_type>(value));
            }
          }
          lua_remove(state, idx);
        }
        else
        {
          error_msg = msg_error_invalid_container;
        }
      }

      static void to(state_type state, const lookup_type& registry, variable_type& var, std::string& error_msg, const policy_node& policy)
      {
        lua_newtable(state);
        if(!check_arg_call<type>(error_msg, var))
        {
          const policy_node& key_node   = policy.get_sub_node(node_container_binary_key);
          const policy_node& value_node = policy.get_sub_node(node_container_binary_value);
          for(auto& elt : cast_arg_call<type>(var))
          {
            variable_type first_any  = std::ref(elt.first);
            variable_type second_any = std::ref(elt.second);
            convert_to<key_type>  (state, registry, first_any,  error_msg, key_node);
            convert_to<value_type>(state, registry, second_any, error_msg, value_node);
            lua_settable(state, -3);
          }
        }
      }
    };

    template <class K, class V, class C, class A> struct convert<std::map<K, V, C, A>> : container_binary_convert<std::map<K, V, C, A>> {};
  }
}

#endif
