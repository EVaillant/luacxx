#ifndef LUACXX_TYPE_INFO_HPP
# define LUACXX_TYPE_INFO_HPP

# include <toolsbox/any.hpp>

# include <luacxx/constant.hpp>
# include <luacxx/utility.hpp>
# include <luacxx/state.hpp>
# include <luacxx/policy.hpp>

# include <lua.hpp>

namespace luacxx
{
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

  template <class T> class integer_type_info : public type_info<T>
  {
    public:
      typedef T type;
      typedef common_type_info::variable_type variable_type;

      static_assert(std::is_integral<type>::value, "must be integer type");

      integer_type_info()
        : type_info<type>(common_type_info::underlying_type::Native)
      {
      }

      virtual void to_lua(state_type state, variable_type& var, std::string &error_msg, const policy_node&) const override
      {
        if(!check_arg_call<const type>(error_msg, var))
        {
          const type& elt = cast_arg_call<const type>(var);
          lua_pushinteger(state, elt);
        }
      }

      virtual void from_lua(state_type state, std::size_t idx, variable_type& var, std::string& error_msg, const policy_node&) const override
      {
        if(lua_isinteger(state, idx))
        {
          lua_Integer tmp = lua_tointeger(state, idx);
          var = static_cast<type>(tmp);
          lua_remove(state, idx);
        }
        else
        {
          error_msg = msg_error_invalid_integer;
        }
      }
  };

  template <class T> class number_type_info : public type_info<T>
  {
    public:
      typedef T type;
      typedef common_type_info::variable_type variable_type;

      static_assert(std::is_floating_point<type>::value, "must be number(float, double or double double) type");

      number_type_info()
        : type_info<type>(common_type_info::underlying_type::Native)
      {
      }

      virtual void to_lua(state_type state, variable_type& var, std::string &error_msg, const policy_node&) const override
      {
        if(!check_arg_call<const type>(error_msg, var))
        {
          const type& elt = cast_arg_call<const type>(var);
          lua_pushnumber(state, elt);
        }
      }

      virtual void from_lua(state_type state, std::size_t idx, variable_type& var, std::string& error_msg, const policy_node&) const override
      {
        if(lua_isnumber(state, idx))
        {
          lua_Number tmp = lua_tonumber(state, idx);
          var = static_cast<type>(tmp);
          lua_remove(state, idx);
        }
        else
        {
          error_msg = msg_error_invalid_number;
        }
      }
  };

  template <class T> class string_type_info : public type_info<T>
  {
    public:
      typedef T type;
      typedef common_type_info::variable_type variable_type;

      string_type_info()
        : type_info<type>(common_type_info::underlying_type::Native)
      {
      }

      virtual void to_lua(state_type state, variable_type& var, std::string &error_msg, const policy_node&) const override
      {
        typedef typename std::conditional<std::is_same<type, std::string>::value, const std::string, type>::type to_lua_type;
        if(!check_arg_call<to_lua_type>(error_msg, var))
        {
          const char* elt = cstr_(cast_arg_call<to_lua_type>(var));
          lua_pushstring(state, elt);
        }
      }

      virtual void from_lua(state_type state, std::size_t idx, variable_type& var, std::string& error_msg, const policy_node&) const override
      {
        if(lua_isstring(state, idx))
        {
          const char* tmp = lua_tostring(state, idx);
          var = std::string(tmp);
          lua_remove(state, idx);
        }
        else
        {
          error_msg = msg_error_invalid_string;
        }
      }

    private:
      static const char* cstr_(const std::string& var) { return var.c_str(); }
      static const char* cstr_(const char* var)        { return var; }
  };

  class bool_type_info : public type_info<bool>
  {
    public:
      typedef bool type;
      typedef common_type_info::variable_type variable_type;

      inline bool_type_info()
        : type_info<bool>(common_type_info::underlying_type::Native)
      {
      }

      inline virtual void to_lua(state_type state, variable_type& var, std::string &error_msg, const policy_node&) const override
      {
        if(!check_arg_call<const type>(error_msg, var))
        {
          const type& elt = cast_arg_call<const type>(var);
          lua_pushboolean(state, elt);
        }
      }

      inline virtual void from_lua(state_type state, std::size_t idx, variable_type& var, std::string& error_msg, const policy_node&) const override
      {
        if(lua_isboolean(state, idx))
        {
          bool tmp = lua_toboolean(state, idx);
          var = tmp;
          lua_remove(state, idx);
        }
        else
        {
          error_msg = msg_error_invalid_boolean;
        }
      }
  };
}

#endif
