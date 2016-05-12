#ifndef LUACXX_BINDING_CTOR_METHOD_HPP
# define LUACXX_BINDING_CTOR_METHOD_HPP

# include <luacxx/binding/callable.hpp>

# include <luacxx/core/request.hpp>
# include <luacxx/core/policy.hpp>

namespace luacxx
{
  template <class C, class ... ARGS> class ctor_method : public callable
  {
    public:
      typedef C                                         class_type;
      typedef std::shared_ptr<class_type>               return_type;
      typedef policy<ctor_method, return_type, ARGS...> policy_type;

      ctor_method(lookup_type& lookup)
        : policy_(*this, node_)
        , request_(lookup, node_, 1)
      {
      }

      policy_type& get_policy()
      {
        return policy_;
      }

    protected:
      typedef request_from_lua<return_type, ARGS...> request_type;

      virtual int invoke_(state_type state) override
      {
        lua_remove(state, 1);
        std::string msg = request_.invoke(state, [] (ARGS ... args) -> return_type
        {
          return std::make_shared<class_type>(std::forward<ARGS>(args)...);
        });
        int   nb_return = request_.get_nb_return_value();
        if(!msg.empty())
        {
          nb_return = 1;
          luaL_error(state, msg.c_str());
        }
        return nb_return;
      }

    private:
      policy_node  node_;
      policy_type  policy_;
      request_type request_;
  };
}

#endif
