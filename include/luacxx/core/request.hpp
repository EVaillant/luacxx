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
            if(lua_isnone(state, idx_) && !std::is_same<arg_type, state_type>::value)
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
          }
          else
          {
            ret = default_initializer<arg_type>::create(lookup_);
            if(ret.empty())
            {
              msg = msg_error_type_not_supported;
            }
          }

          if(!msg.empty())
          {
            msg += " (" + std::to_string(I+1) + ")";
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

  template <class R, class ... ARGS> class request_to_lua
  {
    public:
      request_to_lua(const lookup_type& lookup, const policy_node& policy, std::size_t idx)
        : lookup_(lookup)
        , policy_(policy)
        , idx_(idx)
      {
      }

      std::string invoke(state_type state, ARGS&&... args)
      {
        return invoke_(state, std::forward_as_tuple(std::forward<ARGS>(args)...), std::index_sequence_for<ARGS...>{});
      }

      R get_return() const
      {
        return return_value_.as<R>();
      }

    protected:
      template <class Tuple, std::size_t ... I> std::string invoke_(state_type state, Tuple&& args, std::index_sequence<I...>)
      {
        nb_input_  = 0;
        nb_output_ = (std::is_same<R, void>::value ? 0 : 1);
        return_value_.reset();

        std::string msg = initialize_<Tuple, I...>(state, args);
        if(msg.empty())
        {
          msg = call_(state);
          if(msg.empty())
          {
            msg = output_arg_<Tuple, I...>(state, args);
          }
        }

        return msg;
      }

      std::string call_(state_type state)
      {
        std::string msg;
        int ret = lua_pcall(state, nb_input_, nb_output_, 0);
        if(ret != LUA_OK)
        {
          msg = msg_error_call;
          switch(ret)
          {
            case LUA_ERRRUN:
              msg += "(runtime error)";
              break;

            case LUA_ERRMEM:
              msg += "(memory allocation error)";
              break;

            case LUA_ERRERR:
              msg += "(while running the message handler)";
              break;

            case LUA_ERRGCMM:
              msg += "(while running a __gc metamethod)";
              break;
          }
        }
        if(msg.empty())
        {
          msg = output_return_<R>(state);
        }
        return msg;
      }

      template <class Return> typename std::enable_if<!std::is_same<Return, void>::value, std::string>::type output_return_(state_type state)
      {
        std::string        ret;
        const policy_node& policy = policy_.get_sub_node( "0" );
        convert_from<Return>(state, lookup_, -nb_output_, return_value_, ret, policy);
        --nb_output_;
        return ret;
      }

      template <class Return> typename std::enable_if<std::is_same<Return, void>::value, std::string>::type output_return_(state_type)
      {
        static const std::string empty;
        return empty;
      }

      template <class Tuple, std::size_t I0, std::size_t ... I> std::string output_arg_(state_type state, Tuple& args, typename std::enable_if<(sizeof...(I) != 0), int>::type = 0)
      {
        std::string ret = output_arg_<Tuple, I0>(state, args);
        if(ret.empty())
        {
          ret = output_arg_<Tuple, I...>(state, args);
        }
        return ret;
      }

      template <class Tuple, std::size_t I> std::string output_arg_(state_type state, Tuple& args)
      {
        std::string ret;

        typedef typename std::tuple_element<I, Tuple>::type arg_type;

        const policy_node&      policy    = policy_.get_sub_node( std::to_string(I+1) );
        const parameter_policy& parameter = policy.get_parameter();

        if(parameter.is_output())
        {
          toolsbox::any var;
          if(convert_from<arg_type>(state, lookup_, -nb_output_, var, ret, policy))
          {
            --nb_output_;
            std::get<I>(args) = cast_arg_call<arg_type>(var);
          }
        }

        return ret;
      }

      template <class Tuple> std::string output_arg_(state_type, Tuple&)
      {
        static const std::string empty;
        return empty;
      }

      template <class Tuple, std::size_t I0, std::size_t ... I> std::string initialize_(state_type state, Tuple& args, typename std::enable_if<(sizeof...(I) != 0), int>::type = 0)
      {
        std::string ret = initialize_<Tuple, I0>(state, args);
        if(ret.empty())
        {
          ret = initialize_<Tuple, I...>(state, args);
        }
        return ret;
      }

      template <class Tuple, std::size_t I> std::string initialize_(state_type state, Tuple& args)
      {
        std::string ret;

        typedef typename std::tuple_element<I, Tuple>::type arg_type;

        const policy_node&      policy    = policy_.get_sub_node( std::to_string(I+1) );
        const parameter_policy& parameter = policy.get_parameter();

        if(parameter.is_input())
        {
          ++nb_input_;
          toolsbox::any var = std::ref(std::get<I>(args));
          convert_to<arg_type>(state, lookup_, var, ret, policy);
        }
        else
        {
          ++nb_output_;
        }

        return ret;
      }

      template <class Tuple> std::string initialize_(state_type, Tuple&)
      {
        static const std::string empty;
        return empty;
      }

    private:
      std::size_t        nb_input_;
      std::size_t        nb_output_;
      const lookup_type& lookup_;
      const policy_node& policy_;
      std::size_t        idx_;
      toolsbox::any      return_value_;
  };
}

#endif
