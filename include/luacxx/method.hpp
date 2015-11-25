#ifndef LUACXX_METHOD_HH
# define LUACXX_METHOD_HH

# include <luacxx/fwd.hpp>
# include <luacxx/basic_bindable.hpp>
# include <luacxx/register_type.hpp>
# include <luacxx/policy.hpp>
# include <luacxx/detail/method_type.hpp>
# include <luacxx/detail/request_from_lua.hpp>
# include <luacxx/debug.hpp>

namespace luacxx
{
  template <detail::MethodTypeStyle S, class T, class R, class ... ARGS> class Method : public BasicBindable
  {
    public:
      typedef T type_t;
      typedef Class<type_t> class_t;
      typedef typename class_t::ptr_t ptr_t;
      typedef typename class_t::raw_ptr_t raw_ptr_t;
      typedef Method<S, T, R, ARGS...> self_t;
      typedef Policy<class_t, R, ARGS...> policy_t;

      typedef detail::MethodType<S, T, R, ARGS...> MethodType;
      typedef typename MethodType::method_ptr_t method_ptr_t;

      Method(const RegisterType& registry, class_t& cls, const std::string& name, method_ptr_t ptr)
        : BasicBindable(name)
        , policy_(cls)
        , registry_(registry)
        , method_ptr_(ptr)
      {
      }

      policy_t& get_policy()
      {
        return policy_;
      }

    protected:
      virtual int invoke_(lua_State *state) override
      {
        int nb_response;
        raw_ptr_t ptr = get_ptr_(state);
        if(ptr)
        {
          lua_remove(state, 1);
          std::string msg_error;
          nb_response = detail::request_from_lua<R, 1, ARGS...>::call(state, registry_, policy_, 1, msg_error, MethodType::build_functor(ptr, method_ptr_));
          if(nb_response < 0)
          {
            nb_response = 1;
            luaL_error(state, msg_error.c_str());
          }
        }
        else
        {
          nb_response = 1;
          luaL_error(state, "unable to invoke method because private field is corrupted.");
        }

        return nb_response;
      }

      raw_ptr_t get_ptr_(lua_State *state)
      {
        raw_ptr_t ret = nullptr;
        lua_pushstring(state, class_t::PVT_SMART_PTR);
        lua_gettable(state, 1);
        if(lua_isuserdata(state, -1))
        {
          ptr_t* ptr = (ptr_t*)lua_touserdata(state, -1);
          ret = ptr->get();
          lua_pop(state, 1);
        }
        else
        {
          lua_pop(state, 1);
          lua_pushstring(state, class_t::PVT_RAW_PTR);
          lua_gettable(state, 1);
          if(lua_islightuserdata(state, -1))
          {
            ret = (raw_ptr_t)lua_touserdata(state, -1);
            lua_pop(state, 1);
          }
        }
        return ret;
      }

    private:
      policy_t            policy_;
      const RegisterType& registry_;
      method_ptr_t        method_ptr_;
  };
}

#endif
