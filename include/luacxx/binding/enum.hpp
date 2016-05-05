#ifndef LUACXX_BINDING_GLOBAL_ENUM_HPP
# define LUACXX_BINDING_GLOBAL_ENUM_HPP

# include <luacxx/binding/engine.hpp>
# include <luacxx/binding/bindable.hpp>
# include <luacxx/binding/global.hpp>

# include <luacxx/core/policy.hpp>
# include <luacxx/core/utility.hpp>

# include <type_traits>

namespace luacxx
{
  namespace detail
  {
    template <class E, class U> struct enum_to_underlying
    {
      typedef U return_type;
      typedef E enum_type;

      static return_type convert(const enum_type& e)
      {
        return static_cast<return_type>(e);
      }
    };

    template <class E> struct enum_to_underlying<E, std::string>
    {
      typedef std::string return_type;
      typedef E           enum_type;

      static return_type convert(const enum_type& e)
      {
        std::ostringstream stream;
        stream << e;
        return stream.str();
      }
    };
  }

  template <class E, class U, class C> class enum_wrapper : public bindable
  {
    public:
      typedef enum_wrapper self_type;
      typedef E            enum_type;
      typedef U            underlying_type;
      typedef C            enum_to_underlying;

      enum_wrapper(lookup_type& lookup, const std::string& name)
        : lookup_(lookup)
        , enum_name_(name)
      {
        lookup_.set<enum_type>(std::make_shared<enum_type_info>(*this));
      }

      ~enum_wrapper()
      {
        lookup_.reset<enum_type>();
      }

      virtual bool bind(state_type state) override
      {
        luaL_newmetatable(state, enum_name_.c_str());
        bool ret = true;
        for(auto & pair : values_)
        {
          const std::string&              name  = pair.first;
          std::unique_ptr<wrapper_value>& value = pair.second;

          if((ret = value->bind(state)))
          {
            lua_setfield(state, -2, name.c_str());
          }
          else
          {
            break;
          }
        }
        return ret;
      }

      self_type& value(const std::string &name, const enum_type& e)
      {
        underlying_type u = enum_to_underlying::convert(e);
        values_[name].reset(new wrapper_value(lookup_, u));
        reverse_values_[u] = e;
        return *this;
      }

    protected:
      typedef wrapper_global<underlying_type>                       wrapper_value;
      typedef std::map<std::string, std::unique_ptr<wrapper_value>> values;
      typedef std::map<underlying_type, enum_type>                  reverse_values;

      class enum_type_info : public type_info<enum_type>
      {
        public:
          typedef common_type_info::variable_type variable_type;

          enum_type_info(enum_wrapper& owner)
            : type_info<enum_type>(common_type_info::underlying_type::Enum)
            , owner_(owner)
          {
          }

          virtual void to_lua(state_type state, variable_type& var, std::string &error_msg, const policy_node& policy) const override
          {
            if(check_arg_call<enum_type>(error_msg, var))
            {
              enum_type           e = cast_arg_call<enum_type>(var);
              variable_type udl_var =  enum_to_underlying::convert(e);
              convert_to<underlying_type>(state, owner_.lookup_, udl_var, error_msg, policy);
            }
          }

          virtual void from_lua(state_type state, std::size_t idx, variable_type& var, std::string& error_msg, const policy_node& policy) const override
          {
            variable_type udl_var;
            if(convert_from<underlying_type>(state, owner_.lookup_, idx, udl_var, error_msg, policy) && check_arg_call<underlying_type>(error_msg, udl_var))
            {
              underlying_type udl = cast_arg_call<underlying_type>(udl_var);
              typename reverse_values::const_iterator it = owner_.reverse_values_.find(udl);
              if(it != owner_.reverse_values_.end())
              {
                var = it->second;
              }
              else
              {
                error_msg = msg_error_invalid_enum_value;
              }
            }
          }

        private:
          enum_wrapper& owner_;          
      };

    private:
      lookup_type&      lookup_;
      const std::string enum_name_;
      values            values_;
      reverse_values    reverse_values_;
  };

  template <class E, class U = std::underlying_type_t<E>, class C = detail::enum_to_underlying<E,U>> auto& make_enum(engine& e, const std::string& module_name, const std::string& name)
  {
    typedef enum_wrapper<E, U, C> wrapper_type;
    std::unique_ptr<wrapper_type> wrapper = std::make_unique<wrapper_type>(e.get_lookup_type(), name);
    wrapper_type&                     ret = *wrapper;
    module &m = e.get_module(module_name);
    m.add(name, std::move(wrapper));
    return ret;
  }

  template <class E, class U = std::underlying_type_t<E>, class C = detail::enum_to_underlying<E,U>> auto& make_enum(engine& e, const std::string& name)
  {
    return make_enum<E, U, C>(e, root_module_name, name);
  }
}

#endif
