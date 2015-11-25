#ifndef LUACXX_POLICY_HH
# define LUACXX_POLICY_HH

# include <tuple>

# include <luacxx/fwd.hpp>
# include <luacxx/parameter_policy.hpp>
# include <luacxx/detail/common.hpp>

namespace luacxx
{
  template <class O, class R, class ... ARGS> class Policy
  {
    public:
      typedef O owner_t;
      typedef Policy<O, R, ARGS...> self_t;
      template <class P> using parameter_policy = typename std::conditional<detail::is_output_parameter<P>::value, output_parameter_policy<self_t, detail::remove_const_ref_ptr<P>>, input_parameter_policy<self_t,  detail::remove_const_ref_ptr<P>>>::type;
      typedef output_parameter_policy<self_t, detail::remove_const_ref_ptr<R>> result_parameter_policy;
      typedef std::tuple<result_parameter_policy, parameter_policy<ARGS>...> parameters_t;

      Policy(owner_t& owner)
        : owner_(owner)
        , parameters_(result_parameter_policy(this), parameter_policy<ARGS>(this)...)
      {
      }

      owner_t& get_owner()
      {
        return owner_;
      }

      template <int I> typename std::tuple_element<I,parameters_t>::type& get()
      {
        return std::get<I>(parameters_);
      }

      template <int I> const typename std::tuple_element<I,parameters_t>::type& get() const
      {
        return std::get<I>(parameters_);
      }

    private:
      owner_t& owner_;
      parameters_t parameters_;
  };
}

#endif
