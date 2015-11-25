#ifndef LUACXX_FUNCTION_HH
# define LUACXX_FUNCTION_HH

# include <luacxx/fwd.hpp>
# include <luacxx/bindable.hpp>
# include <luacxx/register_type.hpp>
# include <luacxx/policy.hpp>

# include <luacxx/detail/request_from_lua.hpp>
# include <luacxx/debug.hpp>

# include <functional>

namespace luacxx
{
  template <class R, class ... ARGS> class Function : public Bindable
  {
    public:
      typedef Function<R, ARGS...> self_t;
      typedef std::function<R (ARGS...)> function_t;
      typedef Policy<self_t, R, ARGS...> policy_t;

      Function(RegisterType& registry, const std::string &name, const function_t& function)
        : name_(name)
        , registry_(registry)
        , function_(function)
        , policy_(*this)
      {
      }

      policy_t& get_policy()
      {
        return policy_;
      }

      inline const std::string& get_name() const
      {
        return name_;
      }

      virtual void bind(lua_State *state) override
      {
        //
        // bind fonction
        lua_pushlightuserdata(state, this);
        lua_pushcclosure(state, &lua_wrapper_, 1);
      }

    protected:
      virtual int invoke_(lua_State *state)
      {
        std::string msg_error;
        int nb_response = detail::request_from_lua<R, 1, ARGS...>::call(state, registry_, policy_, 1, msg_error, function_);
        if(nb_response < 0)
        {
          nb_response = 1;
          luaL_error(state, msg_error.c_str());
        }
        return nb_response;
      }

      static inline int lua_wrapper_(lua_State *state)
      {
        self_t* self = (self_t*) lua_touserdata(state, lua_upvalueindex(1));
        return self->invoke_(state);
      }

    private:
      const std::string name_;
      RegisterType&     registry_;
      function_t        function_;
      policy_t          policy_;
  };
}

#endif
