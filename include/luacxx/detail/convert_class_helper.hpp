#ifndef LUACXX_DETAIL_CONVERT_CLASS_HELPER_HH
# define LUACXX_DETAIL_CONVERT_CLASS_HELPER_HH

# include <lua.hpp>

# include <luacxx/fwd.hpp>
# include <luacxx/register_type.hpp>

namespace luacxx
{
  namespace detail
  {
    template <class T> struct convert_class_helper
    {
      public:
        typedef Class<T> class_t;
        typedef typename class_t::type_t    type_t;
        typedef typename class_t::ptr_t     ptr_t;
        typedef typename class_t::raw_ptr_t raw_ptr_t;

        convert_class_helper(class_t&cls, const RegisterType& registry)
          : registry_(registry)
          , class_(cls)
        {
        }

        template <class Policy> int to_lua(lua_State *state, const type_t*t, std::string&, const Policy &policy) const
        {
          if(t)
          {
            lua_newtable(state);
            lua_pushstring(state, class_t::PVT_RAW_PTR);
            lua_pushlightuserdata(state, const_cast<type_t*>(t));
            lua_settable(state, -3);
            lua_pushstring(state, class_t::PVT_DELEGATE_OWNER);
            lua_pushboolean(state, policy.is_delegate_owner());
            lua_settable(state, -3);
            luaL_getmetatable(state, class_.get_class_name().c_str());
            lua_setmetatable(state,  -2);
          }
          else
          {
            lua_pushnil(state);
          }

          return 1;
        }

        template <class Policy> int to_lua(lua_State *state, const type_t& t, std::string& msgError, const Policy &policy) const
        {
          return this->to_lua(state, &t, msgError, policy);
        }

        template <class Policy> int to_lua(lua_State *state, const std::shared_ptr<type_t>&t, std::string&, const Policy &) const
        {
          if(t)
          {
            this->new_shared_ptr_instance(state, t);
          }
          else
          {
            lua_pushnil(state);
          }

          return 1;
        }

        template <class Policy> void from_lua(lua_State *state, int idx, std::shared_ptr<type_t>&ptr, std::string& error_msg, const Policy &) const
        {
          //
          // get type id
          lua_getmetatable(state, idx);
          lua_pushstring(state, class_t::PVT_TYPE_ID);
          lua_gettable(state, -2);
          int type_id = lua_tointeger(state, -1);
          lua_pop(state, 2);

          if(registry_.template get_type_id<T>() == type_id)
          {
            lua_pushstring(state, class_t::PVT_SMART_PTR);
            lua_gettable(state, idx);
            if(lua_isuserdata(state, -1))
            {
              ptr = *(ptr_t*)lua_touserdata(state, -1);
              lua_pop(state, 1);
              lua_remove(state, idx);
            }
            else
            {
              error_msg = "unable to convert to " + class_.get_class_name();
            }
          }
          else
          {
            error_msg = "unable to convert to " + class_.get_class_name();
          }
        }

        template <class Policy> void from_lua(lua_State *state, int idx, raw_ptr_t& raw_ptr, std::string& error_msg, const Policy &) const
        {
          //
          // get type id
          lua_getmetatable(state, idx);
          lua_pushstring(state, class_t::PVT_TYPE_ID);
          lua_gettable(state, -2);
          int type_id = lua_tointeger(state, -1);
          lua_pop(state, 2);

          if(registry_.template get_type_id<T>() == type_id)
          {
            lua_pushstring(state, class_t::PVT_SMART_PTR);
            lua_gettable(state, idx);
            if(lua_isuserdata(state, -1))
            {
              raw_ptr = ((ptr_t*)lua_touserdata(state, -1))->get();
              lua_pop(state, 1);
              lua_remove(state, idx);
            }
            else
            {
              lua_pop(state, 1);
              lua_pushstring(state, class_t::PVT_RAW_PTR);
              lua_gettable(state, idx);
              if(lua_islightuserdata(state, -1))
              {
                raw_ptr = (raw_ptr_t)lua_touserdata(state, -1);
                lua_pop(state, 1);
                lua_remove(state, idx);
              }
              else
              {
                error_msg = "unable to convert to " + class_.get_class_name();
              }
            }
          }
          else
          {
            error_msg = "unable to convert to " + class_.get_class_name();
          }
        }

        template <class A> int new_shared_ptr_instance(lua_State *state, A arg) const
        {
          lua_newtable(state);
          lua_pushstring(state, class_t::PVT_SMART_PTR);
          ptr_t* ptr = (ptr_t*) lua_newuserdata(state, sizeof(ptr_t));
          new (ptr) ptr_t(arg);
          lua_settable(state, -3);
          luaL_getmetatable(state, class_.get_class_name().c_str());
          lua_setmetatable(state,  -2);

          return 1;
        }

      private:
        const RegisterType& registry_;
        class_t&            class_;
    };
  }
}

#endif
