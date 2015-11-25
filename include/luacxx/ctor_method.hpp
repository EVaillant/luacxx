#ifndef LUACXX_CTOR_METHOD_HH
# define LUACXX_CTOR_METHOD_HH

# include <luacxx/fwd.hpp>
# include <luacxx/basic_bindable.hpp>
# include <luacxx/register_type.hpp>
# include <luacxx/policy.hpp>

# include <luacxx/detail/request_from_lua.hpp>
# include <luacxx/debug.hpp>

namespace luacxx
{
  template <class T, class ... ARGS> class CtorMethod : public BasicBindable
  {
    public:
      typedef T type_t;
      typedef Class<type_t> class_t;
      typedef typename class_t::convert_helper_t convert_helper_t;
      typedef typename class_t::ptr_t ptr_t;
      typedef CtorMethod<T, ARGS...> self_t;
      typedef Policy<class_t, void, ARGS...> policy_t;

      CtorMethod(const RegisterType& registry, class_t& cls, convert_helper_t& converter, const std::string& name)
        : BasicBindable(name)
        , converter_(converter)
        , policy_(cls)
        , registry_(registry)
      {
      }

      policy_t& get_policy()
      {
        return policy_;
      }

    protected:
      virtual int invoke_(lua_State *state) override
      {
        lua_remove(state, 1);
        std::string msg_error;
        int nb_response = detail::request_from_lua<void, 1, ARGS...>::call(state, registry_, policy_, 1, msg_error,
          [state, this](ARGS ... args)
          {
            this->converter_.new_shared_ptr_instance(state, new T(args...));
          }
        );

        if(nb_response < 0)
        {
          nb_response = 0;
          luaL_error(state, msg_error.c_str());
        }

        return nb_response + 1;
      }

    private:
      convert_helper_t&   converter_;
      policy_t            policy_;
      const RegisterType& registry_;
  };
}

#endif
