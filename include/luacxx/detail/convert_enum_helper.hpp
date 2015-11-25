#ifndef LUACXX_DETAIL_CONVERT_ENUM_HELPER_HH
# define LUACXX_DETAIL_CONVERT_ENUM_HELPER_HH

# include <lua.hpp>

# include <string>
# include <map>

# include <luacxx/fwd.hpp>
# include <luacxx/register_type.hpp>
# include <luacxx/parameter_policy.hpp>

# include <luacxx/detail/convert.hpp>

namespace luacxx
{
  namespace detail
  {
    template <class E> class convert_enum_helper
    {
      public:
        typedef E enum_t;

        virtual ~convert_enum_helper()
        {
        }

        virtual int to_lua(lua_State *state, const RegisterType& registry, const enum_t &t, std::string& msg_error) const = 0;
        virtual void from_lua(lua_State *state, const RegisterType& registry, int idx, enum_t &t, std::string& msg_error) const = 0;
    };

    template <class E, class U> class convert_enum_helper_impl : public convert_enum_helper<E>
    {
      public:
        typedef E enum_t;
        typedef U underlying_t;
        typedef convert_enum_helper_impl<enum_t, underlying_t> self_t;
        typedef convert_enum_helper<enum_t> parent_t;

        typedef std::map<enum_t, underlying_t> enum_to_underlying_t;
        typedef std::map<underlying_t, enum_t> underlying_to_enum_t;

        virtual int to_lua(lua_State *state, const RegisterType& registry, const enum_t &t, std::string& msg_error) const override
        {
          typename enum_to_underlying_t::const_iterator it = to_.find(t);
          if(it == to_.end())
          {
            msg_error = "no lua match";
            return 0;
          }
          else
          {
            input_parameter_policy<const self_t, enum_t> policy(this);
            return convert_to_lua(state, registry, it->second, msg_error, policy);
          }
        }

        virtual void from_lua(lua_State *state, const RegisterType& registry, int idx, enum_t &t, std::string& msg_error) const override
        {
          underlying_t und_val;
          output_parameter_policy<const self_t, enum_t> policy(this);
          convert_from_lua(state, registry, idx, und_val, msg_error, policy);
          if(msg_error.empty())
          {
            typename underlying_to_enum_t::const_iterator it = from_.find(und_val);
            if(it == from_.end())
            {
              msg_error = "no match";
            }
            else
            {
              t = it -> second;
            }
          }
        }

        void append_value(const enum_t&e, const underlying_t &u)
        {
          to_[e] = u;
          from_[u] = e;
        }

      private:
        enum_to_underlying_t to_;
        underlying_to_enum_t from_;
    };
  }
}

#endif
