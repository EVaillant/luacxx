#ifndef LUACXX_BINDABLE_HH
# define LUACXX_BINDABLE_HH

# include <lua.hpp>

namespace luacxx
{
  class Bindable
  {
    public:
      inline Bindable()
      {
      }

      inline virtual ~Bindable()
      {
      }

      Bindable(const Bindable&) = delete;
      Bindable(Bindable&&) = delete;
      Bindable& operator=(const Bindable&) = delete;

      virtual void bind(lua_State *state) = 0;
  };
}

#endif
