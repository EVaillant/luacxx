#ifndef LUACXX_BASIC_BINDABLE_HH
# define LUACXX_BASIC_BINDABLE_HH

# include <string>
# include <lua.hpp>

# include <luacxx/bindable.hpp>

namespace luacxx
{
  class BasicBindable : public Bindable
  {
    public:
      inline BasicBindable(const std::string &name)
        : name_(name)
      {
      }

      inline virtual void bind(lua_State *state) override
      {
        lua_pushlightuserdata(state, this);
        lua_pushcclosure(state, &lua_wrapper_, 1);
        lua_setfield(state, -2, name_.c_str());
      }

      inline const std::string& get_name() const
      {
        return name_;
      }

    protected:
      virtual int invoke_(lua_State *state) = 0;

      static inline int lua_wrapper_(lua_State *state)
      {
        BasicBindable* self = (BasicBindable*) lua_touserdata(state, lua_upvalueindex(1));
        return self->invoke_(state);
      }

    private:
      const std::string name_;
  };
}

#endif
