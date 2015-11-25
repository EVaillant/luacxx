#ifndef LUACXX_DTOR_METHOD_HH
# define LUACXX_DTOR_METHOD_HH

# include <luacxx/fwd.hpp>
# include <luacxx/basic_bindable.hpp>

namespace luacxx
{
  template <class T> class DtorMethod : public BasicBindable
  {
    public:
      typedef T type_t;
      typedef Class<type_t> class_t;
      typedef typename class_t::ptr_t ptr_t;
      typedef typename class_t::raw_ptr_t raw_ptr_t;
      typedef DtorMethod<T> self_t;

      DtorMethod(class_t& cls, const std::string& name)
        : BasicBindable(name)
        , cls_(cls)
      {
      }

    protected:
      virtual int invoke_(lua_State *state) override
      {
        lua_pushstring(state, class_t::PVT_SMART_PTR);
        lua_gettable(state, -2);
        if(lua_isuserdata(state, -1))
        {
          ptr_t* ptr = (ptr_t*)lua_touserdata(state, -1);
          ptr->~ptr_t();
        }
        else
        {
          lua_pop(state, 1);
          lua_pushstring(state, class_t::PVT_RAW_PTR);
          lua_gettable(state, 1);
          if(lua_islightuserdata(state, -1))
          {
            raw_ptr_t ptr = (raw_ptr_t)lua_touserdata(state, -1);
            lua_pop(state, 1);
            lua_pushstring(state, class_t::PVT_DELEGATE_OWNER);
            lua_gettable(state, 1);
            if(lua_isboolean(state, -1) && lua_toboolean(state, -1))
            {
              delete ptr;
            }
          }
          else
          {
            luaL_error(state, "unable to invoke destructor because private field is corrupted.");
          }
        }
        return 0;
      }

    private:
      class_t& cls_;
  };
}

#endif
