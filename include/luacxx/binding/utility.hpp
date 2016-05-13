#ifndef LUACXX_BINDING_UTILITY_HPP
# define LUACXX_BINDING_UTILITY_HPP

# include <luacxx/core/utility.hpp>
# include <toolsbox/type_uid.hpp>
# include <lua.hpp>

namespace luacxx
{
  struct default_policy {};
  struct custum_policy  {};

  namespace detail
  {
    enum class class_ptr_type
    {
      smart_ptr = 0,
      raw_ptr   = 1
    };

    struct class_ptr
    {
      void*                       ptr;
      class_ptr_type              type;
      toolsbox::type_uid::id_type id;
    };

    typedef class_ptr (*class_cast)(const class_ptr&);

    constexpr auto lua_field_call    = "__call";
    constexpr auto lua_field_index   = "__index";
    constexpr auto lua_field_gc      = "__gc";
    constexpr auto lua_field_id      = "__id_private";
    constexpr auto lua_field_ptr     = "__ptr_private";
    constexpr auto lua_field_type    = "__type_private";
    constexpr auto lua_field_convert = "__convert_private";

    template <class C> std::pair<bool, class_ptr> get_class_ptr(state_type state, std::size_t idx)
    {
      class_ptr ptr {nullptr, class_ptr_type::raw_ptr, toolsbox::type_uid::get<C>()};
      bool status = false;

      if(lua_istable(state, idx))
      {
        int nb_pop = 0;

        lua_pushstring(state, lua_field_id);
        lua_gettable(state, idx);
        ++nb_pop;

        if(lua_isinteger(state, -1))
        {
          ptr.id = lua_tointeger(state, -1);

          lua_pushstring(state, lua_field_type);
          lua_gettable(state, idx);
          ++nb_pop;

          if(lua_isinteger(state, -1))
          {
            bool error = false;
            switch(lua_tointeger(state, -1))
            {
              case 0:
                ptr.type =  class_ptr_type::smart_ptr;
                break;

              case 1:
                ptr.type =  class_ptr_type::raw_ptr;
                break;

              default:
                error = true;
                break;
            }

            if(!error)
            {
              lua_pushstring(state, lua_field_ptr);
              lua_gettable(state, idx);
              ++nb_pop;

              if(lua_isuserdata(state, -1))
              {
                ptr.ptr = lua_touserdata(state, -1);
                status  = true;
              }
            }
          }
        }
        lua_pop(state, nb_pop);
      }
      else if(lua_isnil(state, idx))
      {
        status = true;
      }

      return std::make_pair(status, ptr);
    }
  }
}

#endif
