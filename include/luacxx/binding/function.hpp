#ifndef LUACXX_BINDING_FUNCTION_HPP
# define LUACXX_BINDING_FUNCTION_HPP

# include <luacxx/binding/callable.hpp>
# include <luacxx/binding/engine.hpp>

# include <luacxx/core/request.hpp>
# include <luacxx/core/policy.hpp>

namespace luacxx
{
  template <class R, class ... ARGS> class function_wrapper : public callable
  {
    public:
      typedef std::function<R (ARGS...)>           functor_type;
      typedef policy<function_wrapper, R, ARGS...> policy_type;

      function_wrapper(lookup_type& lookup, const functor_type& functor)
        : functor_(functor)
        , policy_(*this, node_)
        , request_(lookup, node_, 1)
      {
      }

      policy_type& get_policy()
      {
        return policy_;
      }

    protected:
      typedef request_from_lua<R, ARGS...> request_type;

      virtual int invoke_(state_type state) override
      {
        std::string msg = request_.invoke(state, functor_);
        int   nb_return = request_.get_nb_return_value() + policy_.get_extra_return_arg();
        if(!msg.empty())
        {
          nb_return = 1;
          luaL_error(state, msg.c_str());
        }
        return nb_return;
      }

    private:
      functor_type functor_;
      policy_node  node_;
      policy_type  policy_;
      request_type request_;
  };

  template <class R, class ... ARGS> auto& make_function(engine& e, const std::string& module_name, const std::string& name, const std::function<R (ARGS...)>& functor)
  {
    typedef function_wrapper<R, ARGS...> wrapper_type;
    std::unique_ptr<wrapper_type> wrapper = std::make_unique<wrapper_type>(e.get_lookup_type(), functor);
    wrapper_type&                     ret = *wrapper;
    module &m = e.get_module(module_name);
    m.add(name, std::move(wrapper));
    return ret;
  }

  template <class R, class ... ARGS> auto& make_function(engine& e, const std::string& module_name, const std::string& name, R (*functor)(ARGS...))
  {
    return make_function<R, ARGS...>(e, module_name, name, std::function<R (ARGS...)>(functor));
  }

  template <class R, class ... ARGS> auto& make_function(engine& e, const std::string& name, const std::function<R (ARGS...)>& functor)
  {
    return make_function<R, ARGS...>(e, root_module_name, name, functor);
  }

  template <class R, class ... ARGS> auto& make_function(engine& e, const std::string& name, R (*functor)(ARGS...))
  {
    return make_function<R, ARGS...>(e, root_module_name, name, std::function<R (ARGS...)>(functor));
  }
}

#endif
