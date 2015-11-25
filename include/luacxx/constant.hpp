#ifndef LUACXX_CONSTANT_HH
# define LUACXX_CONSTANT_HH

# include <luacxx/bindable.hpp>
# include <luacxx/parameter_policy.hpp>
# include <luacxx/detail/convert.hpp>

namespace luacxx
{
  template <class C, class T> class Constant : public Bindable
  {
    public:
      typedef C container_t;
      typedef T type_t;
      typedef Constant<C, T> self_t;

      Constant(const RegisterType& registry, container_t& container, const std::string &name, const T &val)
        : registry_(registry)
        , container_(container)
        , name_(name)
        , value_(val)
      {
      }

      virtual void bind(lua_State *state) override
      {
        std::string error_msg;
        output_parameter_policy<self_t, type_t> policy(this);
        detail::convert_to_lua(state, registry_, value_, error_msg, policy);
        lua_setfield(state, -2, name_.c_str());

        assert(error_msg.empty());
      }

    private:
      const RegisterType& registry_;
      container_t&        container_;
      const std::string   name_;
      const type_t        value_;
  };
}

#endif
