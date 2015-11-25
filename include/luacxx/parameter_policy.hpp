#ifndef LUACXX_PARAMETER_POLICY_HH
# define LUACXX_PARAMETER_POLICY_HH

# include <functional>
# include <tuple>

# include <luacxx/fwd.hpp>
# include <luacxx/detail/common.hpp>
# include <luacxx/detail/default_initializer.hpp>

namespace luacxx
{
  //
  //
  // output_parameter_policy
  template <class P, class T> class default_output_parameter_policy
  {
    public:
      typedef P policy_t;
      typedef T type_t;
      typedef output_parameter_policy<policy_t, type_t> self_t;

      default_output_parameter_policy(policy_t* policy)
        : policy_(policy)
        , discard_(false)
        , delegate_owner_(false)
      {
      }

      policy_t& discard_return()
      {
        discard_ = true;
        return *policy_;
      }

      bool has_return() const
      {
        return !discard_;
      }

      policy_t& delegate_owner()
      {
        delegate_owner_ = true;
        return *policy_;
      }

      bool is_delegate_owner() const
      {
        return delegate_owner_;
      }

    private:
      policy_t* policy_;
      bool      discard_;
      bool      delegate_owner_;
  };

  template <class P, class T> class output_parameter_policy : public default_output_parameter_policy<P, T>
  {
    public:
      using default_output_parameter_policy<P, T>::default_output_parameter_policy;
  };

  template <class P, class C> class unary_container_output_parameter_policy : public default_output_parameter_policy<P, C>
  {
    public:
      typedef unary_container_output_parameter_policy<P, C> self_t;
      typedef default_output_parameter_policy<P, C> parent_t;
      typedef typename parent_t::policy_t policy_t;
      typedef output_parameter_policy<P, typename C::value_type> value_policy_t;

      unary_container_output_parameter_policy(policy_t* policy)
        : parent_t(policy)
        , value_policy_(policy)
      {
      }

      value_policy_t& get_value_policy()
      {
        return value_policy_;
      }

      const value_policy_t& get_value_policy() const
      {
        return value_policy_;
      }

    private:
      value_policy_t value_policy_;
  };

  template <class P, class C> class binary_container_output_parameter_policy : public default_output_parameter_policy<P, C>
  {
    public:
      typedef binary_container_output_parameter_policy<P, C> self_t;
      typedef default_output_parameter_policy<P, C> parent_t;
      typedef typename parent_t::policy_t policy_t;
      typedef output_parameter_policy<P, typename C::mapped_type> value_policy_t;
      typedef output_parameter_policy<P, typename C::key_type>    key_policy_t;

      binary_container_output_parameter_policy(policy_t* policy)
        : parent_t(policy)
        , key_policy_(policy)
        , value_policy_(policy)
      {
      }

      key_policy_t& get_key_policy()
      {
        return key_policy_;
      }

      const key_policy_t& get_key_policy() const
      {
        return key_policy_;
      }

      value_policy_t& get_value_policy()
      {
        return value_policy_;
      }

      const value_policy_t& get_value_policy() const
      {
        return value_policy_;
      }

    private:
      key_policy_t   key_policy_;
      value_policy_t value_policy_;
  };

  template <class P, class T, class A> class output_parameter_policy<P, std::list<T, A>> : public unary_container_output_parameter_policy<P, std::list<T, A>>
  {
    public:
      using unary_container_output_parameter_policy<P, std::list<T, A>>::unary_container_output_parameter_policy;
  };

  template <class P, class T, class A> class output_parameter_policy<P, std::vector<T, A>> : public unary_container_output_parameter_policy<P, std::vector<T, A>>
  {
    public:
      using unary_container_output_parameter_policy<P, std::vector<T, A>>::unary_container_output_parameter_policy;
  };

  template <class P, class T, class A, class C> class output_parameter_policy<P, std::set<T, A, C>> : public unary_container_output_parameter_policy<P, std::set<T, A, C>>
  {
    public:
      using unary_container_output_parameter_policy<P, std::set<T, A, C>>::unary_container_output_parameter_policy;
  };

  template <class P, class K, class T, class C, class A> class output_parameter_policy<P, std::map<K, T, C, A>> : public binary_container_output_parameter_policy<P, std::map<K, T, C, A>>
  {
    public:
      using binary_container_output_parameter_policy<P, std::map<K, T, C, A>>::binary_container_output_parameter_policy;
  };

  template <class P, class R, class ... ARGS> class output_parameter_policy<P, std::function<R (ARGS...)>> : public default_output_parameter_policy<P, std::function<R (ARGS...)>>
  {
    public:
      typedef output_parameter_policy<P, std::function<R (ARGS...)>> self_t;
      typedef default_output_parameter_policy<P, std::function<R (ARGS...)>> parent_t;
      typedef typename parent_t::policy_t policy_t;
      typedef std::function<R(ARGS...)> type_t;
      template <class A> using parameter_policy = typename std::conditional<detail::is_output_parameter<A>::value, output_parameter_policy<policy_t, detail::remove_const_ref_ptr<A>>, input_parameter_policy<policy_t,  detail::remove_const_ref_ptr<A>>>::type;
      typedef output_parameter_policy<policy_t, detail::remove_const_ref_ptr<R>> result_parameter_policy;
      typedef std::tuple<result_parameter_policy, parameter_policy<ARGS>...> parameters_t;

      output_parameter_policy(policy_t* policy)
        : parent_t(policy)
        , parameters_(result_parameter_policy(policy), parameter_policy<ARGS>(policy)...)
      {
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
        parameters_t parameters_;
  };

  //
  //
  // input_parameter_policy
  template <class P, class T> class default_input_parameter_policy
  {
    public:
      typedef P policy_t;
      typedef T type_t;
      typedef default_input_parameter_policy<policy_t, type_t> self_t;
      typedef detail::local_type<type_t> local_type_t;

      default_input_parameter_policy(policy_t* policy)
        : policy_(policy)
        , has_default_value_(false)
        , default_value_(detail::default_initializer<local_type_t>::empty())
      {
      }

      bool has_default_value() const
      {
        return has_default_value_;
      }

      const local_type_t& get_default_value() const
      {
        return default_value_;
      }

      policy_t& set_default_value(const local_type_t& v)
      {
        has_default_value_ = true;
        default_value_ = v;
        return *policy_;
      }

    protected:
      policy_t*    policy_;
      bool         has_default_value_;
      local_type_t default_value_;
  };

  template <class P, class T> class input_parameter_policy : public default_input_parameter_policy<P, T>
  {
    public:
      using default_input_parameter_policy<P, T>::default_input_parameter_policy;
  };

  template <class P, class C> class unary_container_input_parameter_policy : public default_input_parameter_policy<P, C>
  {
    public:
      typedef unary_container_input_parameter_policy<P, C> self_t;
      typedef default_input_parameter_policy<P, C> parent_t;
      typedef typename parent_t::policy_t policy_t;
      typedef input_parameter_policy<P, typename C::value_type> value_policy_t;

      unary_container_input_parameter_policy(policy_t* policy)
        : parent_t(policy)
        , value_policy_(policy)
      {
      }

      value_policy_t& get_value_policy()
      {
        return value_policy_;
      }

      const value_policy_t& get_value_policy() const
      {
        return value_policy_;
      }

    private:
      value_policy_t value_policy_;
  };

  template <class P, class C> class binary_container_input_parameter_policy : public default_input_parameter_policy<P, C>
  {
    public:
      typedef binary_container_input_parameter_policy<P, C> self_t;
      typedef default_input_parameter_policy<P, C> parent_t;
      typedef typename parent_t::policy_t policy_t;
      typedef input_parameter_policy<P, typename C::mapped_type> value_policy_t;
      typedef input_parameter_policy<P, typename C::key_type>    key_policy_t;

      binary_container_input_parameter_policy(policy_t* policy)
        : parent_t(policy)
        , key_policy_(policy)
        , value_policy_(policy)
      {
      }

      key_policy_t& get_key_policy()
      {
        return key_policy_;
      }

      const key_policy_t& get_key_policy() const
      {
        return key_policy_;
      }

      value_policy_t& get_value_policy()
      {
        return value_policy_;
      }

      const value_policy_t& get_value_policy() const
      {
        return value_policy_;
      }

    private:
      key_policy_t   key_policy_;
      value_policy_t value_policy_;
  };

  template <class P, class T, class A> class input_parameter_policy<P, std::list<T, A>> : public unary_container_input_parameter_policy<P, std::list<T, A>>
  {
    public:
      using unary_container_input_parameter_policy<P, std::list<T, A>>::unary_container_input_parameter_policy;
  };

  template <class P, class T, class A> class input_parameter_policy<P, std::vector<T, A>> : public unary_container_input_parameter_policy<P, std::vector<T, A>>
  {
    public:
      using unary_container_input_parameter_policy<P, std::vector<T, A>>::unary_container_input_parameter_policy;
  };

  template <class P, class T, class A, class C> class input_parameter_policy<P, std::set<T, A, C>> : public unary_container_input_parameter_policy<P, std::set<T, A, C>>
  {
    public:
      using unary_container_input_parameter_policy<P, std::set<T, A, C>>::unary_container_input_parameter_policy;
  };

  template <class P, class K, class T, class C, class A> class input_parameter_policy<P, std::map<K, T, C, A>> : public binary_container_input_parameter_policy<P, std::map<K, T, C, A>>
  {
    public:
      using binary_container_input_parameter_policy<P, std::map<K, T, C, A>>::binary_container_input_parameter_policy;
  };

  template <class P, class R, class ... ARGS> class input_parameter_policy<P, std::function<R (ARGS...)>> : public default_input_parameter_policy<P, std::function<R (ARGS...)>>
  {
    public:
      typedef default_input_parameter_policy<P, std::function<R (ARGS...)>> parent_t;
      typedef typename parent_t::policy_t policy_t;

      template <class A> using parameter_policy = typename std::conditional<detail::is_output_parameter<A>::value, output_parameter_policy<policy_t, detail::remove_const_ref_ptr<A>>, input_parameter_policy<policy_t,  detail::remove_const_ref_ptr<A>>>::type;
      typedef output_parameter_policy<policy_t, detail::remove_const_ref_ptr<R>> result_parameter_policy;
      typedef std::tuple<result_parameter_policy, parameter_policy<ARGS>...> parameters_t;

      input_parameter_policy(policy_t* policy)
        : parent_t(policy)
        , parameters_(result_parameter_policy(policy), parameter_policy<ARGS>(policy)...)
      {
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
      parameters_t parameters_;
  };
}

#endif
