#ifndef LUACXX_POLICY_HPP
# define LUACXX_POLICY_HPP

# include <luacxx/detail/arg_policy.hpp>
# include <tuple>

namespace luacxx
{
  template <class O, class R, class ... Args> class policy
  {
    public:
      typedef O owner_type;
      typedef std::tuple<detail::arg_policy<O, R>, arg_policy<O, Args>...> args_type;

      policy(owner_type& owner, policy_node& node)
        : owner_(owner)
        , node_(node)
        , args_(detail::make_arg_policy<O, R, Args...>(owner, node))
      {
      }

      owner_type& get_owner()
      {
        return owner_;
      }

      template <int I> typename std::tuple_element<I, args_type>::type& get()
      {
        return std::get<I>(args_);
      }

    private:
      owner_type&  owner_;
      policy_node& node_;
      args_type    args_;
  };
}

#endif

