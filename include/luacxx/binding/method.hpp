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
        class_type* self = get_ptr_(state);
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
            nb_return = request_.get_nb_return_value();
          }
        }
        return nb_return;
      }

      class_type* get_ptr_(state_type state)
      {
        class_type* ret =  nullptr;
        const char* msg = nullptr;
        std::pair<bool, common_class_type_info::class_field> class_field = helper_->get_class_field(state, 1);
        if(class_field.first)
        {
          if(class_field.second.ptr)
          {
            toolsbox::any any = helper_->get_instance(class_field.second);
            if(any.empty())
            {
              msg = msg_error_invalid_object;
            }
            else
            {
              if(any.is<class_type*>())
              {
                ret = any.as<class_type*>();
              }
              else if(any.is<smart_type>())
              {
                ret = any.as<smart_type>().get();
              }
              else
              {
                msg = msg_error_invalid_object;
              }
            }
          }
          else
          {
            msg = msg_error_null_object;
          }
        }
        else
        {
          msg = msg_error_object_corrupted;
        }
        if(msg)
        {
          luaL_error(state, msg);
        }
        else
        {
          assert(ret);
          lua_remove(state, 1);
        }
        return ret;
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
