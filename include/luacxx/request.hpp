#ifndef LUACXX_REQUEST_HPP
# define LUACXX_REQUEST_HPP

# include <luacxx/utility.hpp>
# include <luacxx/policy.hpp>
# include <luacxx/default_initializer.hpp>

# include <utility>
# include <tuple>
# include <functional>
# include <type_traits>

namespace luacxx
{
  template <class R, class ... ARGS> class request_from_lua
  {
    public:
      typedef std::tuple<ARGS...>              args_type;
      typedef std::function<R (ARGS ... args)> invoke_type;

      request_from_lua(state_type state, const lookup_type& registry, const policy_node& policy, std::size_t idx)
        : state_(state)
        , registry_(registry)
        , policy_(policy)
        , start_idx_(idx)
      {
      }

      std::string invoke(const invoke_type& delegate)
      {
        return invoke_(delegate, std::index_sequence_for<ARGS...>{});
      }

    protected:
      template <std::size_t ... I> std::string invoke_(const invoke_type& delegate, std::index_sequence<I...>)
      {
        std::string msg;
        auto tuple_arg = std::make_tuple(initialize_<I>(msg)...);
        if(msg.empty())
        {
          msg = call_<R>(delegate, tuple_arg, std::index_sequence<I...>{});
          if(msg.empty())
          {
            int tmp[] = {(append_output_arg_<I>(std::get<I>(tuple_arg), msg),0)...};
            (void) tmp;
          }
        }
        return msg;
      }

      template <class Return, class ArgsAsTuple, std::size_t ... I> typename std::enable_if< std::is_same<Return, void>::value, std::string>::type call_(const invoke_type& delegate, ArgsAsTuple& args, std::index_sequence<I...>)
      {
        std::string msg;
        try
        {
          delegate(cast_arg_call<typename std::tuple_element<I, args_type>::type>(std::get<I>(args))...);
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

      template <class Return, class ArgsAsTuple, std::size_t ... I> typename std::enable_if<!std::is_same<Return, void>::value, std::string>::type call_(const invoke_type& delegate, ArgsAsTuple& args, std::index_sequence<I...>)
      {
        std::string msg;
        try
        {
          toolsbox::any ret = delegate(cast_arg_call<typename std::tuple_element<I, args_type>::type>(std::get<I>(args))...);
          append_output_arg_<0>(ret, msg);
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

      template <std::size_t I> toolsbox::any initialize_(std::string& msg)
      {
        toolsbox::any ret;
        if(msg.empty())
        {
          typedef typename std::tuple_element<I, args_type>::type arg_type;

          const policy_node&                   policy    = policy_.get_sub_node( std::to_string(1 + I) );
          const parameter_policy&              parameter = policy.get_parameter();
          if(parameter.is_input())
          {
            convert_from<arg_type>(state_, registry_, start_idx_ + I, ret, msg, policy);
            if(msg.empty())
            {
              check_arg_call<arg_type>(msg, ret);
            }
            if(!msg.empty())
            {
              msg += " arg:" + std::to_string(I);
            }
          }
          else
          {
            ret = default_initializer<arg_type>::empty();
          }
        }
        return ret;
      }

      template <std::size_t I> void append_output_arg_(toolsbox::any& value, std::string& msg)
      {
        if(msg.empty())
        {
          typedef typename std::tuple_element<I, args_type>::type arg_type;

          const policy_node&      policy    = policy_.get_sub_node( std::to_string(1 + I) );
          const parameter_policy& parameter = policy.get_parameter();

          if((parameter.is_output() && I > 0) || (!parameter.has_return() && I == 0))
          {
            convert_to<arg_type>(state_, registry_, value, msg, policy);
            if(!msg.empty())
            {
              msg += " arg:" + std::to_string(I);
            }
          }
        }
      }

    private:
      state_type         state_;
      const lookup_type& registry_;
      const policy_node& policy_;
      std::size_t        start_idx_;
  };
}

#endif
