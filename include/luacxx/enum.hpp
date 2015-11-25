#ifndef LUACXX_ENUM_HH
# define LUACXX_ENUM_HH

# include <type_traits>
# include <memory>
# include <map>

# include <luacxx/register_type.hpp>
# include <luacxx/bindable.hpp>
# include <luacxx/constant.hpp>

# include <luacxx/detail/enum_to_native_value.hpp>
# include <luacxx/detail/convert_enum_helper.hpp>

namespace luacxx
{
  template <class E, class U = typename std::underlying_type<E>::type, class C = detail::enum_to_native_value<E, U>> class Enum : public Bindable
  {
    public:
      typedef E enum_t;
      typedef U underlying_t;
      typedef C enum_to_native_value_t;

      typedef Enum<enum_t, underlying_t>  self_t;
      typedef typename std::underlying_type<enum_t>::type native_underlying_t;
      typedef detail::convert_enum_helper_impl<enum_t, underlying_t> impl_convert_helper_t;
      typedef typename impl_convert_helper_t::parent_t convert_helper_t;

      Enum(RegisterType& registry, const std::string &name)
        : registry_(registry)
        , enum_name_(name)
      {
        //
        // init registry
        TypeInfo &type_info = registry_.get_type_info<enum_t>();
        assert(!type_info.valid());
        type_info.type = TypeInfo::Type::Enum;
        type_info.converter = (convert_helper_t*)&converter_;
      }

      ~Enum()
      {
        //
        // reset registry
        TypeInfo &type_info = registry_.get_type_info<enum_t>();
        assert(type_info.valid());
        type_info.reset();
      }

      self_t& value(const std::string &name, const enum_t& val)
      {
        return value(name, val, enum_to_native_value_t::convert(val));
      }

      self_t& value(const std::string &name, const enum_t& val, const underlying_t & dval)
      {
        values_[name].reset(new Constant<self_t, U>(registry_, *this, name, dval));
        converter_.append_value(val, dval);
        return *this;
      }

      virtual void bind(lua_State *state) override
      {
        //
        // create table
        luaL_newmetatable(state, enum_name_.c_str());
        for(auto & bindable : values_)
        {
          bindable.second->bind(state);
        }
      }

    private:
      typedef std::map<std::string, std::unique_ptr<Bindable>> bindable_t;

      RegisterType &registry_;
      const std::string enum_name_;

      bindable_t values_;
      impl_convert_helper_t converter_;
  };
}

#endif
