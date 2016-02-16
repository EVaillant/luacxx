#ifndef LUACXX_PARAMETER_POLICY_HPP
# define LUACXX_PARAMETER_POLICY_HPP

# include <luacxx/detail/type_traits.hpp>
# include <toolsbox/any.hpp>

# include <cassert>

namespace luacxx
{
  template <class T> class default_parameter_policy
  {
    public:
      typedef T             type;
      typedef toolsbox::any variable_type;

      default_parameter_policy(bool pi, bool po, bool di)
        : discard_return_(false)
        , delegate_owner_(false)
        , input_(di)
        , output_(!di)
        , permission_input_(pi)
        , permission_output_(po)
      {
      }

      ~default_parameter_policy()
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

      variable_type& get_default_value() const
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

  template <class T> class parameter_policy : public default_parameter_policy<T>
  {
    public:
      using default_parameter_policy<T>::default_parameter_policy;

      bool operator==(const parameter_policy& p) const
      {
        // TODO
        return false;
      }

      bool operator!=(const parameter_policy& p) const
      {
        // TODO
        return false;
      }

      bool operator<(const parameter_policy& p) const
      {
        // TODO
        return false;
      }

      bool operator<=(const parameter_policy& p) const
      {
        // TODO
        return false;
      }

      bool operator>(const parameter_policy& p) const
      {
        // TODO
        return false;
      }

      bool operator>=(const parameter_policy& p) const
      {
        // TODO
        return false;
      }

      std::size_t hash() const
      {
        // TODO
        return 1;
      }
  };

  template <class T> auto make_parameter_policy()
  {
    typedef typename detail::register_type<T>::type register_type;
    return parameter_policy<register_type>(detail::in_out_permission<T>::allow_in, detail::in_out_permission<T>::allow_out, detail::in_out_permission<T>::default_in);
  }
}

namespace std
{
  template<class T> struct hash<luacxx::parameter_policy<T>>
  {
    typedef luacxx::parameter_policy<T> argument_type;
    typedef std::size_t                 result_type;

    result_type operator()(argument_type const& s) const
    {
      return s.hash();
    }
  };
}

#endif

