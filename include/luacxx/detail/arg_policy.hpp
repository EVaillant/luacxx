#ifndef LUACXX_DETAIL_ARG_POLICY_HPP
# define LUACXX_DETAIL_ARG_POLICY_HPP

# include <luacxx/detail/type_traits.hpp>
# include <utility>

namespace luacxx
{
  namespace detail
  {
    template <class O, class T> class arg_policy
    {
      public:
        typedef O owner_type;
        typedef T type;
        typedef typename register_type<type>::type register_type;

        arg_policy(owner_type& owner, policy_node& node)
          : owner_(owner)
          , node_(node)
          , parameter_policy_(node.get_or_create_parameter<register_type>())
        {
        }

        owner_type& discard_return()
        {
          parameter_policy_.discard_return();
          return owner_;
        }

        owner_type& delegate_owner()
        {
          parameter_policy_.delegate_owner();
          return owner_;
        }

        owner_type& input(bool val)
        {
          parameter_policy_.input(val);
          return owner_;
        }

        owner_type& output(bool val)
        {
          parameter_policy_.output(val);
          return owner_;
        }

        template <class U> owner_type& set_default_value(U&& value)
        {
          parameter_policy_.set_default_value(std::forward<U>(value));
          return owner_;
        }

      private:
        owner_type&  owner_;
        policy_node& node_;

        parameter_policy<register_type>& parameter_policy_;
    };

    template <class O, class Tuple, std::size_t ... I> auto make_arg_policy_impl(O& owner, policy_node& node, std::index_sequence<I...>)
    {
      return std::make_tuple(arg_policy<O, typename std::tuple_element<I, Tuple>::type>(owner, node.get_or_create_sub_node(std::to_string(I)))...);
    }

    template <class O, class ... Args> auto make_arg_policy(O& owner, policy_node& node)
    {
      typedef std::tuple<Args...> args_type;
      return make_arg_policy_impl<O, args_type>(owner, node, std::index_sequence_for<Args...>{});
    }
  }
}

#endif

