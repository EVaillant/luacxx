#ifndef LUACXX_BINDING_GLOBAL_HPP
# define LUACXX_BINDING_GLOBAL_HPP

# include <luacxx/binding/engine.hpp>
# include <luacxx/binding/bindable.hpp>

# include <luacxx/core/policy.hpp>
# include <luacxx/core/utility.hpp>

namespace luacxx
{
  template <class T> class wrapper_global : public bindable
  {
    public:
      typedef T type;

      template <class U> wrapper_global(lookup_type& lookup, U&& value)
        : value_(std::forward<U>(value))
        , lookup_(lookup)
      {
      }

      virtual bool bind(state_type state) override
      {
        toolsbox::any var = std::ref(value_);
        std::string   error_msg;
        return convert_to<type&>(state, lookup_, var, error_msg, policy_);
      }

      policy_node& get_policy()
      {
        return policy_;
      }

    private:
      type         value_;
      policy_node  policy_;
      lookup_type& lookup_;
  };

  template <class T> auto& make_global(engine& e, const std::string& module_name, const std::string& name, T&& value)
  {
    typedef std::decay_t<T>                 underlying_type;
    typedef wrapper_global<underlying_type> wrapper_type;
    std::unique_ptr<wrapper_type> wrapper = std::make_unique<wrapper_type>(e.get_lookup_type(), std::forward<T>(value));
    wrapper_type&                     ret = *wrapper;
    module &m = e.get_module(module_name);
    m.add(name, std::move(wrapper));
    return ret;
  }

  template <class T> auto& make_global(engine& e, const std::string& name, T&& value)
  {
    return make_global<T>(e, root_module_name, name, std::forward<T>(value));
  }
}

#endif
