#ifndef LUACXX_BINDING_CALLER_HPP
# define LUACXX_BINDING_CALLER_HPP

# include <luacxx/binding/engine.hpp>
# include <luacxx/core/request.hpp>

namespace luacxx
{
  namespace detail
  {
    template <class R, class O> class caller_wrapper_return
    {
      public:
        typedef detail::request_to_lua_return<R> request_type;
        auto get_return() const -> decltype(((const request_type*)(nullptr))->get_return())
        {
          return static_cast<const O*>(this)->request_.get_return();
        }
    };

    template <class O> class caller_wrapper_return<void, O>
    {
    };
  }

  template <class R, class ... ARGS> class caller_wrapper : public detail::caller_wrapper_return<R, caller_wrapper<R, ARGS...>>
  {
    public:
      typedef request_to_lua<R, ARGS...> request_type;
      typedef policy<caller_wrapper, R, ARGS...> policy_type;

      friend class detail::caller_wrapper_return<R, caller_wrapper>;

      caller_wrapper(engine& e, const std::string& module_name, const std::string& name)
        : engine_(e)
        , module_name_(module_name)
        , name_(name)
        , policy_(*this, node_)
        , request_(engine_.get_lookup_type(), node_, 1)
      {
      }

      policy_type& get_policy()
      {
        return policy_;
      }

      std::string operator()(ARGS && ... args)
      {
        engine_.get_module(module_name_).get_symbol(engine_.get_state(), name_);
        return request_.invoke(engine_.get_state(), std::forward<ARGS>(args)...);
      }

    private:
      engine&      engine_;

      std::string  module_name_;
      std::string  name_;

      policy_node  node_;
      policy_type  policy_;

      request_type request_;
  };

  template <class R, class ... ARGS> auto make_caller(engine& e, const std::string& module_name, const std::string& name)
  {
    typedef caller_wrapper<R, ARGS...> wrapper_type;
    return wrapper_type(e, module_name, name);
  }

  template <class R, class ... ARGS> auto make_caller(engine& e, const std::string& name)
  {
    return make_caller<R, ARGS...>(e, root_module_name, name);
  }
}

#endif
