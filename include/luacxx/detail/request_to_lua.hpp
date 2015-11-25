#ifndef LUACXX_DETAIL_REQUEST_TO_LUA_HH
# define LUACXX_DETAIL_REQUEST_TO_LUA_HH

# include <lua.hpp>

# include <type_traits>

# include <luacxx/fwd.hpp>
# include <luacxx/register_type.hpp>
# include <luacxx/detail/common.hpp>
# include <luacxx/detail/default_initializer.hpp>
# include <luacxx/detail/convert.hpp>
# include <luacxx/detail/pointer_to_ref.hpp>

namespace luacxx
{
  namespace detail
  {
    template <class T, class Policy> int convert_to_lua(lua_State *state, const RegisterType& registry, const T& t, std::string& error_msg, const Policy& policy);
    template <class T, class Policy> void convert_from_lua(lua_State *state, const RegisterType& registry, int idx, T &t, std::string &error_msg, const Policy& policy);

    template <class R, int I, class ... ARGS> struct request_to_lua
    {
    };

    template <class R, int I, class A, class ... ARGS> struct request_to_lua <R, I, A, ARGS...>
    {
      public:
        template <class Policy> static R call(lua_State *state, const RegisterType& registry, const Policy &policy, std::string& msg_error, int nb_input, int nb_output, A && arg0, ARGS && ... args)
        {
          if(msg_error.empty())
          {
            to_lua_<A, Policy>(state, registry, arg0, msg_error, policy, nb_input, nb_output);
          }
          if(msg_error.empty())
          {
            R ret = request_to_lua<R, I+1, ARGS...>::call(state, registry, policy, msg_error, nb_input, nb_output, std::forward<ARGS>(args)...);
            if(msg_error.empty())
            {
              from_lua_<A, Policy>(state, registry, arg0, msg_error, nb_output, policy);
            }
            return std::move(ret);
          }
          else
          {
            return default_initializer<R>::empty();
          }
        }

      protected:
        typedef detail::local_type<A> local_type;

        template <class T, class Policy> static typename std::enable_if< is_output_parameter<T>::value, void>::type to_lua_(lua_State *, const RegisterType&, const A &, std::string&, const Policy &, int &, int &nb_output)
        {
          --nb_output;
        }

        template <class T, class Policy> static typename std::enable_if<!is_output_parameter<T>::value, void>::type to_lua_(lua_State *state, const RegisterType& registry, const A & arg, std::string& msg_error, const Policy &policy, int &nb_input, int &)
        {
          int ret = convert_to_lua(state, registry, arg, msg_error, policy);
          if(ret > 0)
          {
            nb_input += ret;
          }
        }

        template <class T, class Policy> static typename std::enable_if< is_output_parameter<T>::value, void>::type from_lua_(lua_State *state, const RegisterType& registry, A & arg, std::string& msg_error, int nb_output, const Policy &policy)
        {
          auto& param_policy = policy.template get<I>();
          convert_from_lua(state, registry, nb_output, pointer_to_ref<A>(arg), msg_error, param_policy);
        }

        template <class T, class Policy> static typename std::enable_if<!is_output_parameter<T>::value, void>::type from_lua_(lua_State *, const RegisterType&, A &, std::string&, int, const Policy &)
        {
        }
    };


    template <int I, class A, class ... ARGS> struct request_to_lua <void, I, A, ARGS...>
    {
      public:
        template <class Policy> static void call(lua_State *state, const RegisterType& registry, const Policy &policy, std::string& msg_error, int nb_input, int nb_output, A && arg0, ARGS && ... args)
        {
          if(msg_error.empty())
          {
            to_lua_<A, Policy>(state, registry, arg0, msg_error, policy, nb_input, nb_output);
          }
          if(msg_error.empty())
          {
            request_to_lua<void, I+1, ARGS...>::call(state, registry, policy, msg_error, nb_input, nb_output, std::forward<ARGS>(args)...);
            if(msg_error.empty())
            {
              from_lua_<A, Policy>(state, registry, arg0, msg_error, nb_output, policy);
            }
          }
        }

      protected:
        typedef detail::local_type<A> local_type;

        template <class T, class Policy> static typename std::enable_if< is_output_parameter<T>::value, void>::type to_lua_(lua_State *, const RegisterType&, const A &, std::string&, const Policy &, int &, int &nb_output)
        {
          --nb_output;
        }

        template <class T, class Policy> static typename std::enable_if<!is_output_parameter<T>::value, void>::type to_lua_(lua_State *state, const RegisterType& registry, const A & arg, std::string& msg_error, const Policy &policy, int &nb_input, int &)
        {
          int ret = convert_to_lua(state, registry, arg, msg_error, policy);
          if(ret > 0)
          {
            nb_input += ret;
          }
        }

        template <class T, class Policy> static typename std::enable_if< is_output_parameter<T>::value, void>::type from_lua_(lua_State *state, const RegisterType& registry, A & arg, std::string& msg_error, int nb_output, const Policy &policy)
        {
          auto& param_policy = policy.template get<I>();
          convert_from_lua(state, registry, nb_output, pointer_to_ref<A>(arg), msg_error, param_policy);
        }

        template <class T, class Policy> static typename std::enable_if<!is_output_parameter<T>::value, void>::type from_lua_(lua_State *, const RegisterType&, A &, std::string&, int, const Policy &)
        {
        }
    };

    template <class R, int I> struct request_to_lua<R, I>
    {
      public:
        template <class Policy> static R call(lua_State *state, const RegisterType& registry, const Policy &policy, std::string& msg_error, int nb_input, int nb_output)
        {
          request_to_lua<void, I>::call(state, registry, policy, msg_error, nb_input, --nb_output);
          if(msg_error.empty())
          {
            R arg = default_initializer<R>::empty();
            auto& param_policy = policy.template get<0>();
            convert_from_lua(state, registry, nb_output, arg, msg_error, param_policy);
            return std::move(arg);
          }
          else
          {
            return default_initializer<R>::empty();
          }
        }

      private:
        typedef detail::local_type<R> local_type;
    };

    template <int I> struct request_to_lua<void, I>
    {
      public:
        template <class Policy> static void call(lua_State *state, const RegisterType&, const Policy &, std::string& msg_error, int nb_input, int nb_output)
        {
          int ret = lua_pcall(state, nb_input, -nb_output, 0);
          if(ret != LUA_OK)
          {
            switch(ret)
            {
              case LUA_ERRRUN:
                msg_error = "lua runtime error";
                break;

              case LUA_ERRMEM:
                msg_error = "lua memory allocation error";
                break;

              case LUA_ERRERR:
                msg_error = "lua error while running the message handler";
                break;

              case LUA_ERRGCMM:
                msg_error = "lua error while running a __gc metamethod";
                break;
            }
          }
        }
    };
  }
}

#endif
