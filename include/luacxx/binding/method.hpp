#ifndef LUACXX_BINDING_METHOD_HPP
# define LUACXX_BINDING_METHOD_HPP

# include <luacxx/binding/callable.hpp>

# include <luacxx/core/type_info.hpp>
# include <luacxx/core/request.hpp>
# include <luacxx/core/policy.hpp>

namespace luacxx
{
  template <class O, class C, class R, class ... ARGS> class method : public callable
  {
    public:
      typedef O                                       owner_type;
      typedef C                                       class_type;
      typedef std::shared_ptr<class_type>             smart_type;
      typedef std::function<R (class_type*, ARGS...)> functor_type;
      typedef policy<owner_type, R, ARGS...>          policy_type;
      typedef common_class_type_info::self_smart_type smart_common_class_type_info_type;

      method(lookup_type& lookup, owner_type& owner, const smart_common_class_type_info_type& helper, const functor_type& functor)
        : helper_(helper)
        , functor_(functor)
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
        class_type* self = helper_->get_instance<class_type>(state, 1);
        int nb_return = 1;
        if(self)
        {
          std::string msg = request_.invoke(state, [this, self](ARGS ... args) -> R
            {
              return functor_(self, std::forward<ARGS>(args)...);
            }
          );
          if(!msg.empty())
          {
            luaL_error(state, msg.c_str());
          }
          else
          {
            nb_return = request_.get_nb_return_value() + policy_.get_extra_return_arg();
          }
        }
        return nb_return;
      }

    private:
      smart_common_class_type_info_type helper_;
      functor_type                      functor_;
      policy_node                       node_;
      policy_type                       policy_;
      request_type                      request_;
  };
}

#endif
