#ifndef LUACXX_BINDING_CALLABLE_HPP
# define LUACXX_BINDING_CALLABLE_HPP

# include <luacxx/binding/bindable.hpp>
# include <lua.hpp>

namespace luacxx
{
  class callable : public bindable
  {
    public:
      inline virtual bool bind(state_type state) override
      {
        lua_pushlightuserdata(state, this);
        lua_pushcclosure(state, &lua_wrapper_, 1);
        return true;
      }

    protected:
      virtual int invoke_(state_type state) = 0;

      static inline int lua_wrapper_(state_type state)
      {
        callable* self = (callable*) lua_touserdata(state, lua_upvalueindex(1));
        return self->invoke_(state);
      }
  };
}

#endif
