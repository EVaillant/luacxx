#ifndef LUACXX_TEST_HELPER_HPP
# define  LUACXX_TEST_HELPER_HPP

# include <lua.hpp>

class lua_state_guard
{
  public:
    inline lua_state_guard()
    {
      state_ = luaL_newstate();
    }

    inline operator lua_State*()
    {
      return state_;
    }

    inline ~lua_state_guard()
    {
      lua_close(state_);
    }
  private:
    lua_State* state_;
};

#endif
