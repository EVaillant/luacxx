#ifndef LUACXX_CORE_REQUEST_HPP
# define LUACXX_CORE_REQUEST_HPP

# include <luacxx/core/utility.hpp>
# include <luacxx/core/policy.hpp>
# include <luacxx/core/default_initializer.hpp>

# include <utility>
# include <tuple>
# include <functional>
# include <type_traits>

namespace luacxx
{
  template <class R, class ... ARGS> class request_from_lua
  {
    public:
      typedef std::tuple<R, ARGS...>           args_type;
      typedef std::function<R (ARGS ... args)> invoke_type;

      request_from_lua(const lookup_type& lookup, const policy_node& policy, std::size_t idx)
        : lookup_(lookup)
        , policy_(policy)
        , idx_(idx)
        , nb_return_(-1)
      {
      }

      std::string invoke(state_type state, const invoke_type& delegate)
      {
        nb_return_ = 0;
        return invoke_(state, delegate, std::index_sequence_for<ARGS...>{});
      }

      int get_nb_return_value()
      {
        return nb_return_;
      }

    protected:
      template <std::size_t ... I> std::string invoke_(state_type state, const invoke_type& delegate, std::index_sequence<I...>)
      {
        std::string msg;
        auto tuple_arg = initialize_<I...>(state, msg);
        if(msg.empty())
        {
          msg = call_<R>(state, delegate, tuple_arg, std::index_sequence<I...>{});
          append_output_arg_<decltype(tuple_arg), I...>(state, tuple_arg, msg);
        }
        return msg;
      }

      template <class Return, class ArgsAsTuple, std::size_t ... I> typename std::enable_if< std::is_same<Return, void>::value, std::string>::type call_(state_type, const invoke_type& delegate, ArgsAsTuple& args, std::index_sequence<I...>)
      {
        std::string msg;
        try
        {
          delegate(cast_arg_call<typename std::tuple_element<I+1, args_type>::type>(std::get<I>(args))...);
        }
        catch(const std::exception& e)
        {
          msg = "exception raise : " + std::string(e.what());
        }
        catch(...)
        {
          msg = "ellipsis exception";
        }
        return msg;
      }

      template <class Return, class ArgsAsTuple, std::size_t ... I> typename std::enable_if<!std::is_same<Return, void>::value, std::string>::type call_(state_type state, const invoke_type& delegate, ArgsAsTuple& args, std::index_sequence<I...>)
      {
        std::string msg;
        try
        {
          toolsbox::any ret = delegate(cast_arg_call<typename std::tuple_element<I+1, args_type>::type>(std::get<I>(args))...);
          append_output_<0>(state, ret, msg);
        }
        catch(const std::exception& e)
        {
          msg = "exception raise : " + std::string(e.what());
        }
        catch(...)
        {
          msg = "ellipsis exception";
        }
        return msg;
      }

      template <size_t I0, size_t ...I> auto initialize_(state_type state, std::string& msg, typename std::enable_if<(sizeof...(I) != 0), int>::type = 0)
      {
        auto left  = initialize_<I0>(state, msg);
        auto right = initialize_<I...>(state, msg);
        return std::tuple_cat(std::move(left), std::move(right));
      }

      template <std::size_t I> auto initialize_(state_type state, std::string& msg)
      {
        toolsbox::any ret;
        if(msg.empty())
        {
          typedef typename std::tuple_element<I+1, args_type>::type arg_type;

          const policy_node&      policy    = policy_.get_sub_node( std::to_string(I+1) );
          const parameter_policy& parameter = policy.get_parameter();
          if(parameter.is_input())
          {
            if(lua_isnone(state, idx_))
            {
              if(parameter.has_default_value())
              {
                ret = parameter.get_default_value();
              }
              else
              {
                msg = msg_error_missing_argument;
              }
            }
            else
            {
              convert_from<arg_type>(state, lookup_, idx_, ret, msg, policy);
              if(msg.empty())
              {
                check_arg_call<arg_type>(msg, ret);
              }
            }
            if(!msg.empty())
            {
              msg += " (" + std::to_string(I+1) + ")";
            }
          }
          else
          {
            ret = default_initializer<arg_type>::create(lookup_);
          }
        }
        return std::make_tuple(ret);
      }

      template <class T = void> auto initialize_(state_type, std::string&)
      {
        return std::make_tuple();
      }

      template <class ArgsAsTuple, size_t I0, size_t ...I> void append_output_arg_(state_type state, ArgsAsTuple& args, std::string& msg, typename std::enable_if<(sizeof...(I) != 0), int>::type = 0)
      {
        append_output_arg_<ArgsAsTuple, I0>(state, args, msg);
        append_output_arg_<ArgsAsTuple, I...>(state, args, msg);
      }

      template <class ArgsAsTuple, size_t I> void append_output_arg_(state_type state, ArgsAsTuple& args, std::string& msg)
      {
        append_output_<I+1>(state, std::get<I>(args), msg);
      }

      template <class ArgsAsTuple, class T = void> void append_output_arg_(state_type, ArgsAsTuple&, std::string&)
      {
      }

      template <std::size_t I> void append_output_(state_type state, toolsbox::any& value, std::string& msg)
      {
        if(msg.empty())
        {
          typedef typename std::tuple_element<I, args_type>::type arg_type;

          const policy_node&      policy    = policy_.get_sub_node( std::to_string(I) );
          const parameter_policy& parameter = policy.get_parameter();

          if((parameter.is_output() && I > 0) || (!parameter.has_return() && I == 0))
          {            
            convert_to<arg_type>(state, lookup_, value, msg, policy);
            if(!msg.empty())
            {
              msg += " (" + std::to_string(I) + ")";
            }
            else
            {
              ++nb_return_;
            }
          }
        }
      }

    private:
      const lookup_type& lookup_;
      const policy_node& policy_;
      std::size_t        idx_;
      int                nb_return_;
  };
}

#endif
