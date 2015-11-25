#ifndef LUACXX_CALLER_HH
# define LUACXX_CALLER_HH

# include <lua.hpp>

# include <luacxx/fwd.hpp>
# include <luacxx/register_type.hpp>
# include <luacxx/policy.hpp>
# include <luacxx/detail/request_to_lua.hpp>
# include <luacxx/module.hpp>

namespace luacxx
{
  template <class R, class ... ARGS> class Caller
  {
    public:
      typedef Caller<R, ARGS...> self_t;
      typedef Policy<self_t, R, ARGS...> policy_t;

      Caller(RegisterType& registry, const std::string& name, lua_State *state)
        : name_(name)
        , registry_(registry)
        , policy_(*this)
        , state_(state)
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

      R operator()(ARGS && ... args)
      {
        msg_error_.clear();

        if(!find_functor_())
        {
          std::ostringstream stream;
          stream << "unable to find " << name_;
          msg_error_ = stream.str();
        }

        return detail::request_to_lua<R, 1, ARGS...>::call(state_, registry_, policy_, msg_error_, 0, 0, std::forward<ARGS>(args)...);
      }

      bool has_error() const
      {
        return !msg_error_.empty();
      }

      const std::string& get_msg_error() const
      {
        return msg_error_;
      }

    protected:
      bool find_functor_()
      {
        bool ret = true;
        std::size_t current_pos = 0;
        std::size_t next_post;
        bool first = true;

        while(ret && ((next_post = name_.find('.', current_pos)) != std::string::npos))
        {
          ret = find_functor_(name_.substr(current_pos, next_post - current_pos), first);
          first = false;
          current_pos = next_post + 1;
        }

        if(ret)
        {
          ret = find_functor_(name_.substr(current_pos), first);
        }

        return ret;
      }

      bool find_functor_(const std::string &name, bool first)
      {
        bool ret = true;
        if(first)
        {
          lua_getglobal(state_, name.c_str());
          if(lua_isnil(state_, -1))
          {
            lua_pop(state_, 1); // nill
            ret = false;
          }
        }
        else if(lua_istable(state_, -1))
        {
          lua_getfield(state_, -1, name.c_str());
          if(lua_isnil(state_, -1))
          {
            lua_pop(state_, 2); // nill & table
            ret = false;
          }
          else
          {
            lua_remove(state_, -2);
          }
        }
        else
        {
          lua_pop(state_, 1);
          ret = false;
        }
        return ret;
      }

    private:
      const std::string name_;
      RegisterType&     registry_;
      policy_t          policy_;
      lua_State *       state_;
      std::string       msg_error_;
  };
}

#endif
