#ifndef LUACXX_BINDING_STATIC_METHOD_HPP
# define LUACXX_BINDING_STATIC_METHOD_HPP

# include <luacxx/binding/callable.hpp>

# include <luacxx/core/type_info.hpp>
# include <luacxx/core/request.hpp>
# include <luacxx/core/policy.hpp>

namespace luacxx
{
  template <class O, class C, class R, class ... ARGS> class static_method : public callable
  {
    public:
      typedef O                                       owner_type;
      typedef C                                       class_type;
      typedef std::shared_ptr<class_type>             smart_type;
      typedef std::function<R (ARGS...)>              functor_type;
      typedef policy<owner_type, R, ARGS...>          policy_type;
      typedef common_class_type_info::self_smart_type smart_common_class_type_info_type;

      static_method(lookup_type& lookup, owner_type& owner, const functor_type& functor)
        : functor_(functor)
        , policy_(owner, node_)
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
        int nb_return = 1;
        std::string msg = request_.invoke(state, [this](ARGS ... args) -> R
        {
          return functor_(std::forward<ARGS>(args)...);
        });
        if(!msg.empty())
        {
          luaL_error(state, msg.c_str());
        }
        else
        {
          nb_return = request_.get_nb_return_value();
        }
        return nb_return;
      }

    private:
      functor_type                      functor_;
      policy_node                       node_;
      policy_type                       policy_;
      request_type                      request_;
  };
}

#endif
