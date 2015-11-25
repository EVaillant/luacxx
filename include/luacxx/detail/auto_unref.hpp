#ifndef LUACXX_DETAIL_AUTO_UNREF_HH
# define LUACXX_DETAIL_AUTO_UNREF_HH

# include <lua.hpp>

namespace luacxx
{
  namespace detail
  {
    struct auto_unref
    {
        auto_unref(lua_State *s, int r)
          : state(s)
          , ref(r)
        {
        }

        ~auto_unref()
        {
          luaL_unref(state, LUA_REGISTRYINDEX, ref);
        }

        lua_State *state;
        int ref;
    };
  }
}

#endif
