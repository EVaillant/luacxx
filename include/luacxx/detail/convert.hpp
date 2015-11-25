#ifndef LUACXX_DETAIL_CONVERT_CONVERT_HH
# define LUACXX_DETAIL_CONVERT_CONVERT_HH

# include <lua.hpp>

# include <type_traits>
# include <cassert>
# include <iterator>

# include <vector>
# include <list>
# include <set>
# include <map>

# include <luacxx/fwd.hpp>
# include <luacxx/register_type.hpp>
# include <luacxx/detail/common.hpp>
# include <luacxx/detail/auto_unref.hpp>

# include <luacxx/detail/request_to_lua.hpp>
# include <luacxx/detail/request_from_lua.hpp>

# include <luacxx/debug.hpp>

namespace luacxx
{
  namespace detail
  {
    template <class E> class convert_enum_helper;

    template <class R, int I, class ... ARGS> struct request_to_lua;
    template <class R, int I, class ... ARGS> struct request_from_lua;

    template <class T, class Policy> int convert_to_lua(lua_State *state, const RegisterType& registry, const T& t, std::string& error_msg, const Policy& policy);
    template <class T, class Policy> void convert_from_lua(lua_State *state, const RegisterType& registry, int idx, T &t, std::string &error_msg, const Policy& policy);

    //
    // generic (class)
    template <class T, class D = void> struct convert
    {
        typedef T type;

        typedef typename register_type<type>::type register_type;
        typedef Class<register_type> class_t;
        typedef typename class_t::convert_helper_t converter_t;

        template <class Policy> static void from(lua_State *state, const RegisterType& registry, int idx, type &t, std::string &error_msg, const Policy& policy)
        {
          const TypeInfo &type_info = registry.template get_type_info<register_type>();
          if(type_info.valid())
          {
            if(type_info.is_class())
            {
              assert(type_info.converter.is<converter_t*>());
              const converter_t *converter = *std::get<converter_t*>(type_info.converter);
              converter -> from_lua(state, idx, t, error_msg, policy);
            }
          }
          else
          {
            error_msg = "type not supported";
            assert(! "type not supported !");
          }
        }

        template <class Policy> static int to(lua_State *state, const RegisterType& registry, const type& t, std::string& error_msg, const Policy& policy)
        {
          int ret;

          const TypeInfo &type_info = registry.template get_type_info<register_type>();
          if(type_info.valid())
          {
            if(type_info.is_class())
            {
              assert(type_info.converter.is<converter_t*>());
              const converter_t *converter = *std::get<converter_t*>(type_info.converter);
              ret = converter -> to_lua(state, t, error_msg, policy);
            }
          }
          else
          {
            error_msg = "type not supported";
            ret       = 0;
            assert(! "type not supported !");
          }

          return ret;
        }
    };

    //
    // integral
    template <class T> struct convert<T, typename std::enable_if<std::is_integral<T>::value>::type>
    {
        typedef T type;

        template <class Policy> static void from(lua_State *state, const RegisterType&, int idx, type& t, std::string &error_msg, const Policy&)
        {
          if(lua_isnumber(state, idx))
          {
            t = lua_tointeger(state, idx);
            lua_remove(state, idx);
          }
          else
          {
            error_msg = "unable to convert to number";
          }
        }

        template <class Policy> static int to(lua_State *state, const RegisterType&, const type& t, std::string&, const Policy&)
        {
          lua_pushinteger(state, t);
          return 1;
        }
    };

    //
    // float
    template <class T> struct convert<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
    {
        typedef T type;

        template <class Policy> static void from(lua_State *state, const RegisterType&, int idx, type& t, std::string &error_msg, const Policy&)
        {
          if(lua_isnumber(state, idx))
          {
            t = lua_tonumber(state, idx);
            lua_remove(state, idx);
          }
          else
          {
            error_msg = "unable to convert to number";
          }
        }

        template <class Policy> static int to(lua_State *state, const RegisterType&, const type& t, std::string&, const Policy&)
        {
          lua_pushnumber(state, t);
          return 1;
        }
    };

    //
    // string
    template <> struct convert<std::string>
    {
        typedef std::string type;

        template <class Policy> static void from(lua_State *state, const RegisterType&, int idx, type &t, std::string &error_msg, const Policy&)
        {
          if(lua_isstring(state, idx))
          {
            t = lua_tostring(state, idx);
            lua_remove(state, idx);
          }
          else
          {
            error_msg = "unable to convert to string";
          }
        }

        template <class Policy> static int to(lua_State *state, const RegisterType&, const type& t, std::string&, const Policy&)
        {
          lua_pushstring(state, t.c_str());
          return 1;
        }
    };

    //
    // bool
    template <> struct convert<bool>
    {
        typedef bool type;

        template <class Policy> static void from(lua_State *state, const RegisterType&, int idx, type &t, std::string &error_msg, const Policy&)
        {
          if(lua_isboolean(state, idx))
          {
            t = lua_toboolean(state, idx);
            lua_remove(state, idx);
          }
          else
          {
            error_msg = "unable to convert to boolean";
          }
        }

        template <class Policy> static int to(lua_State *state, const RegisterType&, const type& t, std::string&, const Policy&)
        {
          lua_pushboolean(state, t);
          return 1;
        }
    };

    //
    // lua_State
    template <> struct convert<lua_State*>
    {
        typedef lua_State* type;

        template <class Policy> static void from(lua_State *state, const RegisterType&, int &, type &t, std::string &, const Policy&)
        {
          t = state;
        }

        template <class Policy> static int to(lua_State *, const RegisterType&, const type&, std::string&, const Policy&)
        {
          assert(! "not supported !");
          return 0;
        }
    };

    //
    // enum
    template <class T> struct convert<T, typename std::enable_if<std::is_enum<T>::value>::type>
    {
        typedef T type;

        typedef typename register_type<type>::type register_type_t;
        typedef convert_enum_helper<register_type_t> converter_t;

        template <class Policy> static void from(lua_State *state, const RegisterType& registry, int idx, type &t, std::string &error_msg, const Policy&)
        {
          const TypeInfo &type_info = registry.template get_type_info<register_type_t>();
          if(type_info.valid())
          {
            if(type_info.is_enum())
            {
              assert(type_info.converter.is<converter_t*>());
              const converter_t *converter = *std::get<converter_t*>(type_info.converter);
              converter -> from_lua(state, registry, idx, t, error_msg );
            }
          }
          else
          {
            error_msg = "type not supported";
            assert(! "type not supported !");
          }
        }

        template <class Policy> static int to(lua_State *state, const RegisterType& registry, const type& t, std::string& error_msg, const Policy&)
        {
          int ret;

          const TypeInfo &type_info = registry.template get_type_info<register_type_t>();
          if(type_info.valid())
          {
            if(type_info.is_enum())
            {
              assert(type_info.converter.is<converter_t*>());
              const converter_t *converter = *std::get<converter_t*>(type_info.converter);
              ret = converter -> to_lua(state, registry, t, error_msg );
            }
          }
          else
          {
            error_msg = "type not supported";
            ret       = 0;
            assert(! "type not supported !");
          }
          return ret;
        }
    };

    //
    // function (void return)
    template <class ... ARGS> struct convert<std::function<void (ARGS...)>>
    {
        typedef std::function<void (ARGS...)> type;

        template <class Policy> static void from(lua_State *state, const RegisterType& registry, int idx, type &t, std::string &error_msg, const Policy& policy)
        {
          if(lua_isfunction(state, idx))
          {
            lua_pushvalue(state, idx);
            int ref = luaL_ref(state ,LUA_REGISTRYINDEX);
            lua_remove(state, idx);

            std::shared_ptr<auto_unref> unref = std::make_shared<auto_unref>(state, ref);
            t = [state, ref, &registry, unref, &policy](ARGS...args)
            {
              lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
              std::string msg_error;
              request_to_lua<void, 1, ARGS...>::call(state, registry, policy, msg_error, 0, 0, std::forward<ARGS>(args)...);
              if(!msg_error.empty())
              {
                luaL_error(state, msg_error.c_str());
              }
            };
          }
          else
          {
            error_msg = "unable to convert to function";
          }
        }

        template <class Policy> static int to(lua_State *state, const RegisterType& registry, const type& t, std::string&, const Policy& policy)
        {
          lua_newtable(state);
          lua_pushlightuserdata(state, new type(t));
          lua_setfield(state, -2,      "ptr");

          lua_pushlightuserdata(state, (void*)&policy);
          lua_setfield(state, -2,      "policy");

          lua_pushlightuserdata(state, (void*)&registry);
          lua_setfield(state, -2,      "registry");

          lua_newtable(state);
          lua_pushcfunction(state, clean_functor_);
          lua_setfield(state, -2,      "__gc");

          lua_setmetatable(state,  -2);

          lua_pushcclosure(state, call_functor_<Policy>, 1);
          return 1;
        }

        static int clean_functor_(lua_State *state)
        {
          lua_getfield(state, -1, "ptr");
          if(lua_isuserdata(state, -1))
          {
            type* ptr = (type*)lua_touserdata(state, -1);
            delete ptr;
            lua_pop(state, 1);
          }
          return 0;
        }

        template <class Policy> static int call_functor_(lua_State* state)
        {
          int nb_response;
          lua_pushvalue(state, lua_upvalueindex(1));
          lua_getfield(state, -1, "ptr");
          if(lua_isuserdata(state, -1))
          {
            type function = *(type*)lua_touserdata(state, -1);
            lua_getfield(state, -2, "policy");
            if(lua_isuserdata(state, -2))
            {
              const Policy& policy = *(const Policy*)lua_touserdata(state, -2);
              lua_getfield(state, -3, "registry");
              if(lua_isuserdata(state, -3))
              {
                const RegisterType& registry = *(const RegisterType*)lua_touserdata(state, -3);
                lua_pop(state, 4);

                std::string msg_error;
                nb_response = detail::request_from_lua<void, 1, ARGS...>::call(state, registry, policy, 1, msg_error, function);
                if(nb_response < 0)
                {
                  nb_response = 1;
                  luaL_error(state, msg_error.c_str());
                }
              }
              else
              {
                nb_response = 1;
                luaL_error(state, "internal erreur: unable to find registry policy to call fonctor");
              }
            }
            else
            {
              nb_response = 1;
              luaL_error(state, "internal erreur: unable to find field policy to call fonctor");
            }
          }
          else
          {
            nb_response = 1;
            luaL_error(state, "internal erreur: unable to find field ptr to call fonctor");
          }
          return nb_response;
        }
    };

    //
    // function (not void return)
    template <class R, class ... ARGS> struct convert<std::function<R (ARGS...)>>
    {
        typedef std::function<R (ARGS...)> type;

        template <class Policy> static void from(lua_State *state, const RegisterType& registry, int idx, type &t, std::string &error_msg, const Policy& policy)
        {
          if(lua_isfunction(state, idx))
          {
            lua_pushvalue(state, idx);
            int ref = luaL_ref(state ,LUA_REGISTRYINDEX);
            lua_remove(state, idx);

            std::shared_ptr<auto_unref> unref = std::make_shared<auto_unref>(state, ref);
            t = [state, ref, &registry, unref, &policy](ARGS...args) -> R
            {
              lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
              std::string msg_error;
              R ret = request_to_lua<R, 1, ARGS...>::call(state, registry, policy, msg_error, 0, 0, std::forward<ARGS>(args)...);
              if(!msg_error.empty())
              {
                luaL_error(state, msg_error.c_str());
              }
              return std::move(ret);
            };
          }
          else
          {
            error_msg = "unable to convert to function";
          }
        }

        template <class Policy> static int to(lua_State *state, const RegisterType& registry, const type& t, std::string&, const Policy& policy)
        {
          lua_newtable(state);
          lua_pushlightuserdata(state, new type(t));
          lua_setfield(state, -2,      "ptr");

          lua_pushlightuserdata(state, (void*)&policy);
          lua_setfield(state, -2,      "policy");

          lua_pushlightuserdata(state, (void*)&registry);
          lua_setfield(state, -2,      "registry");

          lua_newtable(state);
          lua_pushcfunction(state, clean_functor_);
          lua_setfield(state, -2,      "__gc");

          lua_setmetatable(state,  -2);

          lua_pushcclosure(state, call_functor_<Policy>, 1);
          return 1;
        }

        static int clean_functor_(lua_State *state)
        {
          lua_getfield(state, -1, "ptr");
          if(lua_isuserdata(state, -1))
          {
            type* ptr = (type*)lua_touserdata(state, -1);
            delete ptr;
            lua_pop(state, 1);
          }
          return 0;
        }

        template <class Policy> static int call_functor_(lua_State* state)
        {
          int nb_response;
          lua_pushvalue(state, lua_upvalueindex(1));
          lua_getfield(state, -1, "ptr");
          if(lua_isuserdata(state, -1))
          {
            type function = *(type*)lua_touserdata(state, -1);
            lua_getfield(state, -2, "policy");
            if(lua_isuserdata(state, -2))
            {
              const Policy& policy = *(const Policy*)lua_touserdata(state, -2);
              lua_getfield(state, -3, "registry");
              if(lua_isuserdata(state, -3))
              {
                const RegisterType& registry = *(const RegisterType*)lua_touserdata(state, -3);
                lua_pop(state, 4);

                std::string msg_error;
                nb_response = detail::request_from_lua<R, 1, ARGS...>::call(state, registry, policy, 1, msg_error, function);
                if(nb_response < 0)
                {
                  nb_response = 1;
                  luaL_error(state, msg_error.c_str());
                }
              }
              else
              {
                nb_response = 1;
                luaL_error(state, "internal erreur: unable to find registry policy to call fonctor");
              }
            }
            else
            {
              nb_response = 1;
              luaL_error(state, "internal erreur: unable to find field policy to call fonctor");
            }
          }
          else
          {
            nb_response = 1;
            luaL_error(state, "internal erreur: unable to find field ptr to call fonctor");
          }
          return nb_response;
        }
    };

    //
    // containers
    template <class T> struct convert_unary_container
    {
        typedef T type;

        template <class Policy> static void from(lua_State *state, const RegisterType& registry, int idx, type &t, std::string &error_msg, const Policy& policy)
        {
          if(lua_istable(state, idx))
          {
            std::insert_iterator<type> inserter(t, std::begin(t));
            lua_pushnil(state);
            while (lua_next(state, idx) != 0)
            {
              typename type::value_type elt;
              convert_from_lua(state, registry, lua_gettop(state), elt, error_msg, policy.get_value_policy());
              inserter = std::move(elt);
            }
            lua_remove(state, idx);
          }
          else
          {
            error_msg = "unable to convert to container";
          }
        }

        template <class Policy> static int to(lua_State *state, const RegisterType& registry, const type& t, std::string& error_msg, const Policy& policy)
        {
          int index = 1;
          lua_newtable(state);
          for(const auto & elt : t)
          {
            lua_pushinteger(state, index++);
            convert_to_lua(state, registry, elt, error_msg, policy.get_value_policy());
            lua_settable(state, -3);
          }
          return 1;
        }
    };

    template <class T> struct convert_binary_container
    {
        typedef T type;

        template <class Policy> static void from(lua_State *state, const RegisterType& registry, int idx, type &t, std::string &error_msg, const Policy& policy)
        {
          if(lua_istable(state, idx))
          {
            std::insert_iterator<type> inserter(t, std::begin(t));
            lua_pushnil(state);
            while (lua_next(state, idx) != 0)
            {
              typename type::key_type    key;
              typename type::mapped_type value;

              convert_from_lua(state, registry, lua_gettop(state), value, error_msg, policy.get_value_policy());
              lua_pushvalue(state, lua_gettop(state));
              convert_from_lua(state, registry, lua_gettop(state), key,   error_msg, policy.get_key_policy());

              inserter = std::move(std::make_pair(key, value));
            }
            lua_remove(state, idx);
          }
          else
          {
            error_msg = "unable to convert to container";
          }
        }

        template <class Policy> static int to(lua_State *state, const RegisterType& registry, const type& t, std::string& error_msg, const Policy& policy)
        {
          lua_newtable(state);
          for(const auto & elt : t)
          {
            convert_to_lua(state, registry, elt.first,  error_msg, policy.get_key_policy());
            convert_to_lua(state, registry, elt.second, error_msg, policy.get_value_policy());
            lua_settable(state, -3);
          }
          return 1;
        }
    };

    template <class T, class A> struct convert<std::list<T, A>> : public convert_unary_container<std::list<T, A>>
    {
    };

    template <class T, class A> struct convert<std::vector<T, A>> : public convert_unary_container<std::vector<T, A>>
    {
    };

    template <class T, class C, class A> struct convert<std::set<T, C, A>> : public convert_unary_container<std::set<T, C, A>>
    {
    };

    template <class K, class T, class C, class A> struct convert<std::map<K, T, C, A>> : public convert_binary_container<std::map<K, T, C, A>>
    {
    };

    //
    // helper
    template <class T, class Policy> int convert_to_lua(lua_State *state, const RegisterType& registry, const T& t, std::string& error_msg, const Policy& policy)
    {
      return convert<T>::to(state, registry, t, error_msg, policy);
    }

    template <class T, class Policy> void convert_from_lua(lua_State *state, const RegisterType& registry, int idx, T &t, std::string &error_msg, const Policy& policy)
    {
      convert<T>::from(state, registry, idx, t, error_msg, policy);
    }
  }
}

#endif
