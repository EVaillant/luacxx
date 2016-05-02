#ifndef LUACXX_CORE_POLICY_HPP
# define LUACXX_CORE_POLICY_HPP

# include <luacxx/core/type_traits.hpp>

# include <toolsbox/any.hpp>

# include <cassert>
# include <utility>
# include <unordered_map>
# include <map>
# include <array>
# include <tuple>
# include <memory>

namespace luacxx
{
  /**
   * @brief The parameter_policy class
   */
  class parameter_policy
  {
    public:
      typedef toolsbox::any variable_type;

      parameter_policy(bool p_i, bool p_o, bool d_i, bool d_o)
        : discard_return_(false)
        , delegate_owner_(false)
        , input_(d_i)
        , output_(d_o)
        , permission_input_(p_i)
        , permission_output_(p_o)
      {
      }

      ~parameter_policy()
      {
      }

      void discard_return()
      {
        discard_return_ = true;
      }

      bool has_return() const
      {
        return discard_return_;
      }

      void delegate_owner()
      {
        delegate_owner_ = true;
      }

      bool is_delegate_owner() const
      {
        return delegate_owner_;
      }

      void input(bool val)
      {
        assert(permission_input_);
        if(permission_input_)
        {
          input_ = val;
        }
      }

      bool is_input() const
      {
        return input_;
      }

      void output(bool val)
      {
        assert(permission_output_);
        if(permission_output_)
        {
          output_ = val;
        }
      }

      bool is_output() const
      {
        return output_;
      }

      bool has_default_value() const
      {
        return !default_.empty();
      }

      template <class U> void set_default_value(U&& value)
      {
        default_ = std::forward<U>(value);
      }

      const variable_type& get_default_value() const
      {
        return default_;
      }

    private:
      bool discard_return_;
      bool delegate_owner_;
      bool input_;
      bool output_;

      const bool permission_input_;
      const bool permission_output_;

      variable_type default_;
  };

  template <class T> std::unique_ptr<parameter_policy> make_parameter_policy()
  {
    return std::unique_ptr<parameter_policy>(new parameter_policy(in_out_permission<T>::allow_in, in_out_permission<T>::allow_out, in_out_permission<T>::default_in, in_out_permission<T>::default_out));
  }

  /**
   * @brief The policy_node class
   */
  class policy_node
  {
    public:
      const policy_node& get_sub_node(const std::string& name) const
      {
        sub_node_type::const_iterator it = nodes_.find(name);
        assert(it != nodes_.end());
        return *it->second;
      }

      const parameter_policy& get_parameter() const
      {
        assert(parameter_);
        return *parameter_;
      }

      bool has_parameter() const
      {
        return parameter_ != nullptr;
      }

      bool has_node(const std::string& name) const
      {
        return nodes_.find(name) != nodes_.end();
      }

      policy_node& get_or_create_sub_node(const std::string& name)
      {
        sub_node_type::iterator it = nodes_.find(name);
        if(it == nodes_.end())
        {
          it = nodes_.insert(std::make_pair(name, std::unique_ptr<policy_node>(new policy_node))).first;
        }
        return *it->second;
      }

      template <class T> parameter_policy& get_or_create_parameter()
      {
        if(!parameter_)
        {
          parameter_ = make_parameter_policy<T>();
        }
        return *parameter_;
      }

    private:
      typedef std::unordered_map<std::string, std::unique_ptr<policy_node>> sub_node_type;

      sub_node_type                     nodes_;
      std::unique_ptr<parameter_policy> parameter_;
  };

  /**
   * @brief the arg_policy class
   */
  template <class O> class arg_policy
  {
    public:
      typedef O                      owner_type;
      typedef arg_policy<owner_type> self_type;

      arg_policy(owner_type& owner, policy_node& node)
        : owner_(owner)
        , node_(node)
        , policy_(node_.get_or_create_parameter<void*>())
      {
      }

      self_type& get_attr(const std::string& name)
      {
        typename attrs_type::iterator it = attrs_.find(name);
        if(it == attrs_.end())
        {
          it = attrs_.insert(std::make_pair(name, self_type(owner_, node_.get_or_create_sub_node(name)))).first;
        }
        return it->second;
      }

      owner_type& discard_return()
      {
        policy_.discard_return();
        return owner_;
      }

      owner_type& delegate_owner()
      {
        policy_.delegate_owner();
        return owner_;
      }

      owner_type& input(bool val)
      {
        policy_.input(val);
        return owner_;
      }

      owner_type& output(bool val)
      {
        policy_.output(val);
        return owner_;
      }

      template <class U> owner_type& set_default_value(U&& value)
      {
        policy_.set_default_value(std::forward<U>(value));
        return owner_;
      }

    protected:
      typedef std::map<std::string, self_type> attrs_type;

      attrs_type        attrs_;
      owner_type&       owner_;
      policy_node&      node_;
      parameter_policy& policy_;
  };

  template <class O, class Arg> arg_policy<O> make_arg_policy(O& owner, policy_node& node, std::size_t i)
  {
    policy_node& sub_node = node.get_or_create_sub_node(std::to_string(i));
    sub_node.get_or_create_parameter<Arg>();
    return arg_policy<O>(owner, sub_node);
  }

  namespace detail
  {
    template <class O, class Tuple, std::size_t ... I> std::array<arg_policy<O>, sizeof...(I)> make_arg_policies_(O& owner, policy_node& node, std::index_sequence<I...>)
    {
      return { make_arg_policy<O, typename std::tuple_element<I, Tuple>::type>(owner, node, I)... };
    }
  }

  template <class O, class ... Args> std::array<arg_policy<O>, sizeof...(Args)> make_arg_policies(O& owner, policy_node& node)
  {
    return detail::make_arg_policies_<O, std::tuple<Args...>>(owner, node, std::make_index_sequence<sizeof...(Args)>{});
  }

  /**
   * @brief the policy class
   */
  template <class O, class R, class ... Args> class policy
  {
    public:
      typedef O                                         owner_type;
      typedef policy<owner_type, R, Args...>            self_type;
      typedef arg_policy<self_type>                     arg_policy_type;
      static const size_t                               args_length = sizeof...(Args) + 1;
      typedef std::array<arg_policy_type, args_length>  args_type;

      policy(owner_type& owner, policy_node& node)
        : owner_(owner)
        , args_(make_arg_policies<self_type, R, Args...>(*this, node))
      {
      }

      owner_type& get_owner()
      {
        return owner_;
      }

      arg_policy_type& get(std::size_t i)
      {
        return args_[i];
      }

    private:
      owner_type&  owner_;
      args_type    args_;
  };
}

#endif

