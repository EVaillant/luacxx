#ifndef LUACXX_CORE_UTILITY_HPP
# define LUACXX_CORE_UTILITY_HPP

# include <toolsbox/any.hpp>

# include <luacxx/core/constant.hpp>
# include <luacxx/core/lookup_type.hpp>
# include <luacxx/core/policy.hpp>
# include <luacxx/core/type_traits.hpp>

# include <list>
# include <vector>
# include <set>
# include <map>

# include <cassert>
# include <type_traits>

# include <lua.hpp>

namespace luacxx
{
  template <class R, class ... ARGS> class request_from_lua;
  template <class R, class ... ARGS> class request_to_lua;
  namespace detail
  {
    template <class T> bool check_table(const T& tables)
    {
      for(size_t i = 0 ; i < tables.size() ; ++i)
      {
        for(size_t j = i + 1 ; j < tables.size() ; ++j)
        {
          if(tables[i].id == tables[j].id)
          {
            return false;
          }
        }
      }
      return true;
    }

    template <class T, class C = void> class arg_call
    {
      static_assert(sizeof(T) > 0, "only specialize must be instanced");
    };

    template <> class arg_call<const char*, void>
    {
      public:
        typedef const char* type;
        typedef arg_call    self_type;
        typedef type        return_type;

        static bool check(const toolsbox::any& value)
        {
          functor_type fct = (value.empty() ? nullptr : get_functor_(value.get_id()));
          return fct != nullptr;
        }

        static return_type convert(toolsbox::any& any)
        {
          assert(check(any));
          functor_type fct = get_functor_(any.get_id());
          return (*fct)(any);
        }

      private:
        typedef return_type (*functor_type)(toolsbox::any&);

        template <class U> static return_type convert_type_(toolsbox::any& any)
        {
          return any.as<U>();
        }

        template <class U> static return_type convert_string_(toolsbox::any& any)
        {
          return any.as<U>().c_str();
        }

        static functor_type get_functor_(toolsbox::type_uid::id_type id)
        {
          struct convert_table {
              toolsbox::type_uid::id_type id;
              functor_type                convert;
          };

          static const std::array<convert_table, 6> convert_tables = {{
            {toolsbox::type_uid::get<type>(),               self_type::convert_type_<type>},
            {toolsbox::type_uid::get<type&>(),              self_type::convert_type_<type&>},
            {toolsbox::type_uid::get<const type&>(),        self_type::convert_type_<const type&>},
            {toolsbox::type_uid::get<std::string>(),        self_type::convert_string_<std::string>},
            {toolsbox::type_uid::get<std::string&>(),       self_type::convert_string_<std::string&>},
            {toolsbox::type_uid::get<const std::string&>(), self_type::convert_string_<const std::string&>}
          }};

          assert(check_table(convert_tables));

          for(const convert_table& table : convert_tables)
          {
            if(id == table.id)
            {
              return table.convert;
            }
          }

          return nullptr;
        }
    };

    template <class T> class arg_call<T*, typename std::enable_if<std::is_const<T>::value>::type>
    {
      public:
        typedef std::decay_t<T*>                 type;
        typedef arg_call                         self_type;
        typedef T*                               return_type;
        typedef std::decay_t<T>                  underlying_type;
        typedef std::shared_ptr<underlying_type> shared_type;

        static bool check(const toolsbox::any& value)
        {
          functor_type fct = (value.empty() ? nullptr : get_functor_(value.get_id()));
          return fct != nullptr;
        }

        static return_type convert(toolsbox::any& any)
        {
          assert(check(any));
          functor_type fct = get_functor_(any.get_id());
          return (*fct)(any);
        }

      private:
        typedef return_type (*functor_type)(toolsbox::any&);

        template <class U> static return_type convert_type_(toolsbox::any& any)
        {
          return any.as<U>();
        }

        template <class U> static return_type convert_underlying_(toolsbox::any& any)
        {
          return &any.as<U>();
        }

        template <class U> static return_type convert_shared_(toolsbox::any& any)
        {
          return any.as<U>().get();
        }

        static functor_type get_functor_(toolsbox::type_uid::id_type id)
        {
          struct convert_table {
              toolsbox::type_uid::id_type id;
              functor_type                convert;
          };

          static const std::array<convert_table, 12> convert_tables = {{
            {toolsbox::type_uid::get<underlying_type*>(),              self_type::convert_type_<underlying_type*>},
            {toolsbox::type_uid::get<const underlying_type*>(),        self_type::convert_type_<const underlying_type*>},
            {toolsbox::type_uid::get<const underlying_type*&>(),       self_type::convert_type_<const underlying_type*&>},
            {toolsbox::type_uid::get<const underlying_type* const&>(), self_type::convert_type_<const underlying_type* const&>},
            {toolsbox::type_uid::get<underlying_type*&>(),             self_type::convert_type_<underlying_type*&>},
            {toolsbox::type_uid::get<underlying_type* const&>(),       self_type::convert_type_<underlying_type* const&>},
            {toolsbox::type_uid::get<underlying_type>(),               self_type::convert_underlying_<underlying_type>},
            {toolsbox::type_uid::get<underlying_type&>(),              self_type::convert_underlying_<underlying_type&>},
            {toolsbox::type_uid::get<const underlying_type&>(),        self_type::convert_underlying_<const underlying_type&>},
            {toolsbox::type_uid::get<shared_type>(),                   self_type::convert_shared_<shared_type>},
            {toolsbox::type_uid::get<const shared_type&>(),            self_type::convert_shared_<const shared_type&>},
            {toolsbox::type_uid::get<shared_type&>(),                  self_type::convert_shared_<shared_type&>}
          }};

          assert(check_table(convert_tables));

          for(const convert_table& table : convert_tables)
          {
            if(id == table.id)
            {
              return table.convert;
            }
          }

          return nullptr;
        }
    };

    template <class T> class arg_call<T*, typename std::enable_if<!std::is_const<T>::value  && !is_incomplete<T>::value>::type>
    {
      public:
        typedef std::decay_t<T*>                 type;
        typedef arg_call                         self_type;
        typedef T*                               return_type;
        typedef std::remove_pointer_t<type>      underlying_type;
        typedef std::shared_ptr<underlying_type> shared_type;

        static bool check(const toolsbox::any& value)
        {
          functor_type fct = (value.empty() ? nullptr : get_functor_(value.get_id()));
          return fct != nullptr;
        }

        static return_type convert(toolsbox::any& any)
        {
          assert(check(any));
          functor_type fct = get_functor_(any.get_id());
          return (*fct)(any);
        }

      private:
        typedef return_type (*functor_type)(toolsbox::any&);

        template <class U> static return_type convert_type_(toolsbox::any& any)
        {
          return any.as<U>();
        }

        template <class U> static return_type convert_underlying_(toolsbox::any& any)
        {
          return &any.as<U>();
        }

        template <class U> static return_type convert_shared_(toolsbox::any& any)
        {
          return any.as<U>().get();
        }

        static functor_type get_functor_(toolsbox::type_uid::id_type id)
        {
          struct convert_table {
              toolsbox::type_uid::id_type id;
              functor_type                convert;
          };

          static const std::array<convert_table, 8> convert_tables = {{
            {toolsbox::type_uid::get<type>(),               self_type::convert_type_<type>},
            {toolsbox::type_uid::get<type&>(),              self_type::convert_type_<type&>},
            {toolsbox::type_uid::get<const type&>(),        self_type::convert_type_<const type&>},
            {toolsbox::type_uid::get<underlying_type>(),    self_type::convert_underlying_<underlying_type>},
            {toolsbox::type_uid::get<underlying_type&>(),   self_type::convert_underlying_<underlying_type&>},
            {toolsbox::type_uid::get<shared_type>(),        self_type::convert_shared_<shared_type>},
            {toolsbox::type_uid::get<const shared_type&>(), self_type::convert_shared_<const shared_type&>},
            {toolsbox::type_uid::get<shared_type&>(),       self_type::convert_shared_<shared_type&>}
          }};

          assert(check_table(convert_tables));

          for(const convert_table& table : convert_tables)
          {
            if(id == table.id)
            {
              return table.convert;
            }
          }

          return nullptr;
        }
    };

    template <class T> class arg_call<T*, typename std::enable_if<!std::is_const<T>::value  && is_incomplete<T>::value>::type>
    {
      public:
        typedef std::decay_t<T*>                 type;
        typedef arg_call                         self_type;
        typedef T*                               return_type;

        static bool check(const toolsbox::any& value)
        {
          functor_type fct = (value.empty() ? nullptr : get_functor_(value.get_id()));
          return fct != nullptr;
        }

        static return_type convert(toolsbox::any& any)
        {
          assert(check(any));
          functor_type fct = get_functor_(any.get_id());
          return (*fct)(any);
        }

      private:
        typedef return_type (*functor_type)(toolsbox::any&);

        template <class U> static return_type convert_type_(toolsbox::any& any)
        {
          return any.as<U>();
        }

        static functor_type get_functor_(toolsbox::type_uid::id_type id)
        {
          struct convert_table {
              toolsbox::type_uid::id_type id;
              functor_type                convert;
          };

          static const std::array<convert_table, 3> convert_tables = {{
            {toolsbox::type_uid::get<type>(),               self_type::convert_type_<type>},
            {toolsbox::type_uid::get<type&>(),              self_type::convert_type_<type&>},
            {toolsbox::type_uid::get<const type&>(),        self_type::convert_type_<const type&>},
          }};

          assert(check_table(convert_tables));

          for(const convert_table& table : convert_tables)
          {
            if(id == table.id)
            {
              return table.convert;
            }
          }

          return nullptr;
        }
    };

    template <class T> class arg_call<T, typename std::enable_if<std::is_const<T>::value && !std::is_pointer<T>::value && !is_shared_ptr<T>::value>::type>
    {
      public:
        typedef T                                         type;
        typedef std::decay_t<type>                        underlying_type;
        typedef arg_call                                  self_type;
        typedef type&                                     return_type;
        typedef std::add_pointer_t<underlying_type>       ptr_type;
        typedef std::add_pointer_t<const underlying_type> const_ptr_type;
        typedef std::shared_ptr<underlying_type>          shared_type;

        static bool check(const toolsbox::any& value)
        {
          functor_type fct = (value.empty() ? nullptr : get_functor_(value.get_id()));
          return fct != nullptr;
        }

        static return_type convert(toolsbox::any& any)
        {
          assert(check(any));
          functor_type fct = get_functor_(any.get_id());
          return (*fct)(any);
        }

      private:
        typedef return_type (*functor_type)(toolsbox::any&);

        template <class U> static return_type convert_type_(toolsbox::any& any)
        {
          return any.as<U>();
        }

        template <class U> static return_type convert_ptr_(toolsbox::any& any)
        {
          return *any.as<U>();
        }

        template <class U> static return_type convert_shared_(toolsbox::any& any)
        {
          return *any.as<U>().get();
        }

        static functor_type get_functor_(toolsbox::type_uid::id_type id)
        {
          struct convert_table {
              toolsbox::type_uid::id_type id;
              functor_type                convert;
          };

          static const std::array<convert_table, 14> convert_tables = {{
            {toolsbox::type_uid::get<underlying_type>(),        self_type::convert_type_<underlying_type>},
            {toolsbox::type_uid::get<underlying_type&>(),       self_type::convert_type_<underlying_type&>},
            {toolsbox::type_uid::get<const underlying_type>(),  self_type::convert_type_<const underlying_type>},
            {toolsbox::type_uid::get<const underlying_type&>(), self_type::convert_type_<const underlying_type&>},
            {toolsbox::type_uid::get<ptr_type>(),               self_type::convert_ptr_<ptr_type>},
            {toolsbox::type_uid::get<const ptr_type>(),         self_type::convert_ptr_<const ptr_type>},
            {toolsbox::type_uid::get<ptr_type&>(),              self_type::convert_ptr_<ptr_type&>},
            {toolsbox::type_uid::get<const ptr_type&>(),        self_type::convert_ptr_<const ptr_type&>},
            {toolsbox::type_uid::get<const_ptr_type>(),         self_type::convert_ptr_<const_ptr_type>},
            {toolsbox::type_uid::get<const_ptr_type&>(),        self_type::convert_ptr_<const_ptr_type&>},
            {toolsbox::type_uid::get<const const_ptr_type&>(),  self_type::convert_ptr_<const const_ptr_type&>},
            {toolsbox::type_uid::get<shared_type>(),            self_type::convert_shared_<shared_type>},
            {toolsbox::type_uid::get<const shared_type&>(),     self_type::convert_shared_<const shared_type&>},
            {toolsbox::type_uid::get<shared_type&>(),           self_type::convert_shared_<shared_type&>}
          }};

          assert(check_table(convert_tables));

          for(const convert_table& table : convert_tables)
          {
            if(id == table.id)
            {
              return table.convert;
            }
          }

          return nullptr;
        }
    };

    template <class T> class arg_call<T, typename std::enable_if<!std::is_const<T>::value && !std::is_pointer<T>::value && !is_shared_ptr<T>::value>::type>
    {
      public:
        typedef T                                   type;
        typedef std::decay_t<type>                  underlying_type;
        typedef arg_call                            self_type;
        typedef type&                               return_type;
        typedef std::add_pointer_t<underlying_type> ptr_type;
        typedef std::shared_ptr<underlying_type>    shared_type;

        static bool check(const toolsbox::any& value)
        {
          functor_type fct = (value.empty() ? nullptr : get_functor_(value.get_id()));
          return fct != nullptr;
        }

        static return_type convert(toolsbox::any& any)
        {
          assert(check(any));
          functor_type fct = get_functor_(any.get_id());
          return (*fct)(any);
        }

      private:
        typedef return_type (*functor_type)(toolsbox::any&);

        template <class U> static return_type convert_type_(toolsbox::any& any)
        {
          return any.as<U>();
        }

        template <class U> static return_type convert_ptr_(toolsbox::any& any)
        {
          return *any.as<U>();
        }

        template <class U> static return_type convert_shared_(toolsbox::any& any)
        {
          return *any.as<U>().get();
        }

        static functor_type get_functor_(toolsbox::type_uid::id_type id)
        {
          struct convert_table {
              toolsbox::type_uid::id_type id;
              functor_type                convert;
          };

          static const std::array<convert_table, 8> convert_tables = {{
            {toolsbox::type_uid::get<underlying_type>(),    self_type::convert_type_<underlying_type>},
            {toolsbox::type_uid::get<underlying_type&>(),   self_type::convert_type_<underlying_type&>},
            {toolsbox::type_uid::get<ptr_type>(),           self_type::convert_ptr_<ptr_type>},
            {toolsbox::type_uid::get<ptr_type&>(),          self_type::convert_ptr_<ptr_type&>},
            {toolsbox::type_uid::get<ptr_type const&>(),    self_type::convert_ptr_<ptr_type const&>},
            {toolsbox::type_uid::get<shared_type>(),        self_type::convert_shared_<shared_type>},
            {toolsbox::type_uid::get<const shared_type&>(), self_type::convert_shared_<const shared_type&>},
            {toolsbox::type_uid::get<shared_type&>(),       self_type::convert_shared_<shared_type&>}
          }};

          assert(check_table(convert_tables));

          for(const convert_table& table : convert_tables)
          {
            if(id == table.id)
            {
              return table.convert;
            }
          }

          return nullptr;
        }
    };

    template <class T> class arg_call<T, typename std::enable_if<is_shared_ptr<T>::value>::type>
    {
      public:
        typedef std::decay_t<T>  type;
        typedef arg_call         self_type;
        typedef T                return_type;

        static bool check(const toolsbox::any& value)
        {
          functor_type fct = (value.empty() ? nullptr : get_functor_(value.get_id()));
          return fct != nullptr;
        }

        static return_type convert(toolsbox::any& any)
        {
          assert(check(any));
          functor_type fct = get_functor_(any.get_id());
          return (*fct)(any);
        }

      private:
        typedef return_type (*functor_type)(toolsbox::any&);

        template <class U> static return_type convert_type_(toolsbox::any& any)
        {
          return any.as<U>();
        }

        static functor_type get_functor_(toolsbox::type_uid::id_type id)
        {
          struct convert_table {
              toolsbox::type_uid::id_type id;
              functor_type                convert;
          };

          static const std::array<convert_table, 3> convert_tables = {{
            {toolsbox::type_uid::get<type>(),        self_type::convert_type_<type>},
            {toolsbox::type_uid::get<type&>(),       self_type::convert_type_<type&>},
            {toolsbox::type_uid::get<const type&>(), self_type::convert_type_<const type&>}
          }};

          assert(check_table(convert_tables));

          for(const convert_table& table : convert_tables)
          {
            if(id == table.id)
            {
              return table.convert;
            }
          }

          return nullptr;
        }
    };
  }

  //
  // cast_arg_call & check_arg_call
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

  // fwd declaration
  template <class T> bool convert_from(state_type state, const lookup_type& lookup, std::size_t idx, toolsbox::any &var, std::string& error_msg, const policy_node& policy);
  template <class T> bool convert_to(state_type state, const lookup_type& lookup, toolsbox::any& var, std::string& error_msg, const policy_node& policy);

  namespace detail
  {
    //
    // default case
    template <class T> struct convert
    {
      typedef T             type;
      typedef toolsbox::any variable_type;

      static void from(state_type state, const lookup_type& lookup, std::size_t idx, variable_type &var, std::string& error_msg, const policy_node& policy)
      {
        const type_info<type>& type_info = lookup.template get<type>();
        if(type_info.valid())
        {
          type_info.from_lua(state, idx, var, error_msg, policy);
        }
        else
        {
          error_msg = msg_error_type_not_supported;
        }
      }

      static void to(state_type state, const lookup_type& lookup, variable_type& var, std::string& error_msg, const policy_node& policy)
      {
        const type_info<type>& type_info = lookup.template get<type>();
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
    // closure
    struct auto_unref
    {
      auto_unref(state_type s, int r)
        : state(s)
        , ref(r)
      {
      }

      ~auto_unref()
      {
        luaL_unref(state, LUA_REGISTRYINDEX, ref);
      }

      state_type state;
      int        ref;
    };

    template <class R, class ... ARGS> struct convert<std::function<R (ARGS...)>>
    {
      public:
        typedef std::function<R (ARGS...)> type;
        typedef toolsbox::any              variable_type;

        struct to_data
        {
          request_from_lua<R, ARGS...> request;
          type                         functor;
        };

        static void from(state_type state, const lookup_type& lookup, std::size_t idx, variable_type &var, std::string& error_msg, const policy_node& policy)
        {
          if(lua_isfunction(state, idx))
          {
            lua_pushvalue(state, idx);
            int ref = luaL_ref(state ,LUA_REGISTRYINDEX);
            lua_remove(state, idx);

            std::shared_ptr<auto_unref> unref = std::make_shared<auto_unref>(state, ref);
            var = type([state, ref, &lookup, unref, &policy](ARGS...args) -> R
            {
              lua_rawgeti(state, LUA_REGISTRYINDEX, ref);

              request_to_lua<R, ARGS...> request(lookup, policy, 1);
              std::string msg_error = request.invoke(state, std::forward<ARGS>(args)...);

              if(!msg_error.empty())
              {
                luaL_error(state, msg_error.c_str());
              }

              return request.get_return();
            });
          }
          else
          {
            error_msg = msg_error_invalid_function;
          }
        }

        static void to(state_type state, const lookup_type& lookup, variable_type& var, std::string& error_msg, const policy_node& policy)
        {
          if(!check_arg_call<type>(error_msg, var))
          {
            type functor = cast_arg_call<type>(var);

            lua_newtable(state);
            void* data = lua_newuserdata(state, sizeof(to_data));
            new (data) to_data{{lookup, policy, 1}, functor};
            lua_setfield(state, -2,  lua_field_ptr);

            lua_newtable(state);
            lua_pushcfunction(state, gc_);
            lua_setfield(state, -2,  lua_field_gc);
            lua_setmetatable(state,  -2);

            lua_pushcclosure(state, call_, 1);
          }
        }

      protected:
        static int gc_(state_type state)
        {
          lua_getfield(state, -1, lua_field_ptr);
          if(lua_isuserdata(state, -1))
          {
            to_data* ptr = (to_data*)lua_touserdata(state, -1);
            ptr->~to_data();
            lua_pop(state, 1);
          }
          return 0;
        }

        static int call_(state_type state)
        {
          int nb_response = 1;
          lua_pushvalue(state, lua_upvalueindex(1));
          lua_getfield(state, -1, lua_field_ptr);
          if(lua_isuserdata(state, -1))
          {
            to_data* ptr = (to_data*)lua_touserdata(state, -1);
            std::string msg = ptr->request.invoke(state, ptr->functor);
            if(!msg.empty())
            {
              luaL_error(state, msg.c_str());
            }
            else
            {
              nb_response = ptr->request.get_nb_return_value();
            }
          }
          else
          {
            luaL_error(state, msg_error_object_corrupted);
          }
          return nb_response;
        }
    };

    //
    // container unary
    template <class C, class I = typename C::value_type> struct container_unary_convert
    {
      typedef C             type;
      typedef I             value_type;
      typedef toolsbox::any variable_type;

      static void from(state_type state, const lookup_type& lookup, std::size_t idx, variable_type &var, std::string& error_msg, const policy_node& policy)
      {
        if(lua_istable(state, idx))
        {
          var       = type();
          type &tmp = var.as<type>();
          std::insert_iterator<type> inserter(tmp, std::begin(tmp));
          lua_pushnil(state);
          const policy_node& data_policy = policy.get_or_default_sub_node(node_container_unary);
          while (lua_next(state, idx) && error_msg.empty())
          {
            variable_type elt;
            convert_from<value_type>(state, lookup, lua_gettop(state), elt, error_msg, data_policy);
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

      static void to(state_type state, const lookup_type& lookup, variable_type& var, std::string& error_msg, const policy_node& policy)
      {
        std::size_t index = 1;
        lua_newtable(state);
        if(!check_arg_call<type>(error_msg, var))
        {
          const policy_node& data_policy = policy.get_or_default_sub_node(node_container_unary);
          for(value_type elt : cast_arg_call<type>(var))
          {
            lua_pushinteger(state, index++);
            variable_type elt_any  = std::ref(elt);
            convert_to<value_type>(state, lookup, elt_any, error_msg, data_policy);
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

      static void from(state_type state, const lookup_type& lookup, std::size_t idx, variable_type &var, std::string& error_msg, const policy_node& policy)
      {
        if(lua_istable(state, idx))
        {
          var       = type();
          type &tmp = var.as<type>();
          std::insert_iterator<type> inserter(tmp, std::begin(tmp));
          const policy_node& key_policy   = policy.get_or_default_sub_node(node_container_binary_key);
          const policy_node& value_policy = policy.get_or_default_sub_node(node_container_binary_value);
          lua_pushnil(state);
          while (lua_next(state, idx) && error_msg.empty())
          {
            variable_type key;
            variable_type value;

            if(convert_from<value_type>(state, lookup, lua_gettop(state), value, error_msg, value_policy))
            {
              lua_pushvalue(state, lua_gettop(state));
              convert_from<key_type>(state, lookup, lua_gettop(state), key, error_msg, key_policy);
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

      static void to(state_type state, const lookup_type& lookup, variable_type& var, std::string& error_msg, const policy_node& policy)
      {
        lua_newtable(state);
        if(!check_arg_call<type>(error_msg, var))
        {
          const policy_node& key_policy   = policy.get_or_default_sub_node(node_container_binary_key);
          const policy_node& value_policy = policy.get_or_default_sub_node(node_container_binary_value);
          for(auto& elt : cast_arg_call<type>(var))
          {
            variable_type first(std::cref(elt.first));
            variable_type second(std::ref(elt.second));
            if(convert_to<key_type>  (state, lookup, first,  error_msg, key_policy)
              && convert_to<value_type>(state, lookup, second, error_msg, value_policy))
            {
              lua_settable(state, -3);
            }
            if(!error_msg.empty())
            {
              break;
            }
          }
        }
      }
    };

    template <class K, class V, class C, class A> struct convert<std::map<K, V, C, A>> : container_binary_convert<std::map<K, V, C, A>> {};
  }

  //
  // convert_from & convert_to
  template <class T> bool convert_from(state_type state, const lookup_type& lookup, std::size_t idx, toolsbox::any &var, std::string& error_msg, const policy_node& policy)
  {
    typedef typename register_type<T>::type  register_type;
    detail::convert<register_type>::from(state, lookup, idx, var, error_msg, policy);
    return error_msg.empty();
  }

  template <class T> bool convert_to(state_type state, const lookup_type& lookup, toolsbox::any& var, std::string& error_msg, const policy_node& policy)
  {
    typedef typename register_type<T>::type  register_type;
    detail::convert<register_type>::to(state, lookup, var, error_msg, policy);
    return error_msg.empty();
  }
}

#endif
