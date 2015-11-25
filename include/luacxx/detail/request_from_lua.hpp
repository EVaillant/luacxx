#ifndef LUACXX_DETAIL_REQUEST_FROM_LUA_HH
# define LUACXX_DETAIL_REQUEST_FROM_LUA_HH

# include <lua.hpp>

# include <type_traits>
# include <sstream>

# include <luacxx/fwd.hpp>
# include <luacxx/register_type.hpp>
# include <luacxx/detail/common.hpp>
# include <luacxx/detail/default_initializer.hpp>
# include <luacxx/detail/convert.hpp>
# include <luacxx/detail/forward_converter.hpp>

namespace luacxx
{
  namespace detail
  {
    template <class T, class Policy> int convert_to_lua(lua_State *state, const RegisterType& registry, const T& t, std::string& error_msg, const Policy& policy);
    template <class T, class Policy> void convert_from_lua(lua_State *state, const RegisterType& registry, int idx, T &t, std::string &error_msg, const Policy& policy);

    template <class R, int I, class ... ARGS> struct request_from_lua
    {
    };

    template <class R, int I, class A, class ... ARGS> struct request_from_lua<R, I, A, ARGS...>
    {
      public:
        template <class Policy, class Caller, class ... CallerArg> static int call(lua_State *state, const RegisterType& registry, const Policy& policy, int idx, std::string& errorMsg, const Caller &caller, CallerArg &&... args)
        {
          int nb_response;
          local_type local_var = initialize<A>(policy, state, registry, idx, errorMsg);
          if(errorMsg.empty())
          {
            nb_response = request_from_lua<R, I+1, ARGS...>::call(state, registry, policy, idx, errorMsg, caller, std::forward<CallerArg>(args)..., forward_converter<local_type, A>(local_var));
            if(nb_response >= 0)
            {
              nb_response += append_output_value<A>(state, registry, policy, local_var, errorMsg);
              if(!errorMsg.empty())
              {
                nb_response = -1;
              }
            }
          }
          else
          {
            nb_response = -1;
            std::stringstream stream;
            stream << errorMsg << " (pos:" << I << ")";
            errorMsg = stream.str();
          }

          return nb_response;
        }

      private:
        typedef detail::local_type<A> local_type;

        template <class T, class Policy> static typename std::enable_if< is_output_parameter<T>::value, local_type>::type initialize(const Policy&, lua_State *, const RegisterType&, int, std::string &)
        {
          return default_initializer<local_type>::create();
        }

        template <class T, class Policy> static typename std::enable_if<!is_output_parameter<T>::value, local_type>::type initialize(const Policy& policy, lua_State *state, const RegisterType& registry, int idx, std::string &errorMsg)
        {
          local_type var = default_initializer<local_type>::empty();
          auto& param_policy = policy.template get<I>();
          if(!is_lua_state<T>::value && lua_isnone(state, idx))
          {
            if(param_policy.has_default_value())
            {
              var = param_policy.get_default_value();
            }
            else
            {
              errorMsg = "miss arg";
            }
          }
          else
          {
            convert_from_lua(state, registry, idx, var, errorMsg, param_policy);
          }
          return std::move(var);
        }

        template <class T, class Policy> static typename std::enable_if< is_output_parameter<T>::value, int>::type append_output_value(lua_State *state, const RegisterType &registry, const Policy& policy, local_type &val, std::string& msgError)
        {
          auto& param_policy = policy.template get<I>();
          int nb_ret = ((param_policy.has_return()) ? convert_to_lua(state, registry, val, msgError, param_policy) : 0);
          return (msgError.empty() ? nb_ret: -1);
        }

        template <class T, class Policy> static typename std::enable_if<!is_output_parameter<T>::value, int>::type append_output_value(lua_State *, const RegisterType &, const Policy&, local_type &, std::string&)
        {
          return 0;
        }
    };

    template <class R, int I> struct request_from_lua<R, I>
    {
      template <class Policy, class Caller, class ... CallerArg> static int call(lua_State *state, const RegisterType& registry, const Policy& policy, int, std::string& errorMsg, const Caller &caller, CallerArg && ... args)
      {
        int nb_ret;
        try
        {
          R ret  = caller(std::forward<CallerArg>(args)...);
          auto& param_policy = policy.template get<0>();
          nb_ret = ((param_policy.has_return()) ? convert_to_lua(state, registry, ret, errorMsg, param_policy) : 0);
        }
        catch(const std::exception &e)
        {
          errorMsg = e.what();
        }
        catch(...)
        {
          errorMsg = "unkwown exception";
        }
        return (errorMsg.empty() ? nb_ret: -1);
      }
    };

    template <int I> struct request_from_lua<void, I>
    {
      template <class Policy, class Caller, class ... CallerArg> static int call(lua_State *, const RegisterType &, const Policy &, int, std::string &errorMsg, const Caller &caller, CallerArg && ... args)
      {
        int nb_ret = 0;
        try
        {
          caller(std::forward<CallerArg>(args)...);
        }
        catch(const std::exception &e)
        {
          errorMsg = e.what();
          nb_ret   = -1;
        }
        catch(...)
        {
          errorMsg = "unkwown exception";
          nb_ret   = -1;
        }
        return nb_ret;
      }
    };
  }
}

#endif
