#ifndef LUACXX_DEBUG_HH
# define LUACXX_DEBUG_HH

# include <lua.hpp>
# include <iostream>

namespace luacxx
{
  inline void lua_stack_dump(lua_State *L)
  {
    int i;
    int top = lua_gettop(L);
    for (i = 1; i <= top; i++)
    {
      /* repeat for each level */
      int t = lua_type(L, i);
      switch (t)
      {
        case LUA_TSTRING:  /* strings */
          std::cout << i << ":'" << lua_tostring(L, i) << "'";
          break;

        case LUA_TBOOLEAN:  /* booleans */
          std::cout << i << (lua_toboolean(L, i) ? ":true" : ":false");
          break;

        case LUA_TNUMBER:  /* numbers */
          std::cout << i << ":" << lua_tonumber(L, i);
          break;

        case LUA_TTABLE:  /* table */
          std::cout << i << ":table:" << lua_topointer(L, i);
          break;

        default:  /* other values */
          std::cout << i << ":" << lua_typename(L, t);
          break;

      }
      std::cout << " ";
    }
    std::cout << std::endl;
  }
}

#endif
