#ifndef LUACXX_CLASS_HH
# define LUACXX_CLASS_HH

# include <string>
# include <memory>
# include <map>

# include <luacxx/register_type.hpp>
# include <luacxx/detail/convert_class_helper.hpp>

# include <luacxx/method.hpp>
# include <luacxx/ctor_method.hpp>
# include <luacxx/dtor_method.hpp>

namespace luacxx
{
  struct default_policy {};
  struct custum_policy  {};

  template <class T> class Class : public Bindable
  {
    public:
      typedef T                       type_t;
      typedef Class<T>                self_t;
      typedef std::shared_ptr<type_t> ptr_t;
      typedef type_t*                 raw_ptr_t;

      typedef detail::convert_class_helper<T> convert_helper_t;

      static const std::string META_CLASS_PREFIX;

      static const std::string MTH_CALL;
      static const std::string MTH_GC;
      static const char* MTH_INDEX;

      static const char* PVT_SMART_PTR;
      static const char* PVT_RAW_PTR;
      static const char* PVT_TYPE_ID;
      static const char* PVT_DELEGATE_OWNER;

      Class(RegisterType& registry, const std::string &name)
        : registry_(registry)
        , class_name_(name)
        , meta_class_name_(name + META_CLASS_PREFIX)
        , converter_(*this, registry)
      {
        //
        // append destroy
        std::unique_ptr<Bindable> dtor(new DtorMethod<T>(*this, MTH_GC));
        method_[MTH_GC] = std::move(dtor);

        //
        // init registry
        TypeInfo &type_info = registry_.get_type_info<T>();
        assert(!type_info.valid());
        type_info.type = TypeInfo::Type::Class;
        type_info.converter = &converter_;
      }

      ~Class()
      {
        //
        // reset registry
        TypeInfo &type_info = registry_.get_type_info<T>();
        assert(type_info.valid());
        type_info.reset();
      }

      template <class ... ARGS, class P = default_policy> typename std::enable_if< std::is_same<P, default_policy>::value, self_t>::type& ctor(const P & = P())
      {
        return this->ctor<ARGS...>(custum_policy()).get_owner();
      }

      template <class ... ARGS, class P = default_policy> typename std::enable_if<!std::is_same<P, default_policy>::value, Policy<self_t, void, ARGS...>>::type& ctor(const P & = P())
      {
        typedef CtorMethod<T, ARGS...> method_t;
        typedef typename method_t::policy_t policy_t;

        std::unique_ptr<method_t> ctor(new method_t(registry_, *this, converter_, MTH_CALL));
        policy_t &policy = ctor->get_policy();
        meta_method_[MTH_CALL] = std::move(ctor);
        return policy;
      }

      template <class R, class ... ARGS, class P = default_policy> typename std::enable_if< std::is_same<P, default_policy>::value, self_t>::type& method(const std::string &name, R (type_t::*mth)(ARGS ... args), const P & = P())
      {
        return this->method(name, mth, custum_policy()).get_owner();
      }

      template <class R, class ... ARGS, class P = default_policy> typename std::enable_if<!std::is_same<P, default_policy>::value, Policy<self_t, R, ARGS...>>::type& method(const std::string &name, R (type_t::*mth)(ARGS ... args), const P & = P())
      {
        typedef Method<detail::MethodTypeStyle::CXX, T, R, ARGS...> method_t;
        typedef typename method_t::policy_t policy_t;

        std::unique_ptr<method_t> method(new method_t(registry_, *this, name, mth));
        policy_t &policy = method->get_policy();
        method_[name] = std::move(method);
        return policy;
      }

      template <class R, class ... ARGS, class P = default_policy> typename std::enable_if< std::is_same<P, default_policy>::value, self_t>::type& const_method(const std::string &name, R (type_t::*mth)(ARGS ... args) const, const P & = P())
      {
        return this->const_method(name, mth, custum_policy()).get_owner();
      }

      template <class R, class ... ARGS, class P = default_policy> typename std::enable_if<!std::is_same<P, default_policy>::value, Policy<self_t, R, ARGS...>>::type& const_method(const std::string &name, R (type_t::*mth)(ARGS ... args) const, const P & = P())
      {
        typedef Method<detail::MethodTypeStyle::CONST_CXX, T, R, ARGS...> method_t;
        typedef typename method_t::policy_t policy_t;

        std::unique_ptr<method_t> method(new method_t(registry_, *this, name, mth));
        policy_t &policy = method->get_policy();
        method_[name] = std::move(method);
        return policy;
      }

      template <class R, class ... ARGS, class P = default_policy> typename std::enable_if< std::is_same<P, default_policy>::value, self_t>::type& method(const std::string &name, R (*mth)(T& instance, ARGS ... args), const P & = P())
      {
        return this->method(name, mth, custum_policy()).get_owner();
      }

      template <class R, class ... ARGS, class P = default_policy> typename std::enable_if<!std::is_same<P, default_policy>::value, Policy<self_t, R, ARGS...>>::type& method(const std::string &name, R (*mth)(T& instance, ARGS ... args), const P & = P())
      {
        typedef Method<detail::MethodTypeStyle::C, T, R, ARGS...> method_t;
        typedef typename method_t::policy_t policy_t;

        std::unique_ptr<method_t> method(new method_t(registry_, *this, name, mth));
        policy_t &policy = method->get_policy();
        method_[name] = std::move(method);
        return policy;
      }

      virtual void bind(lua_State *state) override
      {
        //
        // create table
        luaL_newmetatable(state, class_name_.c_str());
        for(auto & bindable : method_)
        {
          bindable.second->bind(state);
        }
        lua_pushvalue(state, -1);
        lua_setfield(state,  -2, MTH_INDEX);
        lua_pushstring(state, PVT_TYPE_ID);
        lua_pushinteger(state, registry_.get_type_id<T>());
        lua_settable(state, -3);

        //
        // create meta table
        luaL_newmetatable(state, meta_class_name_.c_str());
        for(auto & bindable : meta_method_)
        {
          bindable.second->bind(state);
        }

        //
        // attach table with meta table
        lua_setmetatable(state,  -2);
      }

      const std::string& get_class_name() const
      {
        return class_name_;
      }

    protected:
      typedef std::map<std::string, std::unique_ptr<Bindable>> bindable_t;

    private:
      RegisterType &registry_;

      const std::string class_name_;
      const std::string meta_class_name_;

      convert_helper_t converter_;

      bindable_t method_;
      bindable_t meta_method_;
  };

  template<class C> const std::string Class<C>::MTH_CALL  = "__call";
  template<class C> const std::string Class<C>::MTH_GC    = "__gc";
  template<class C> const char* Class<C>::MTH_INDEX       = "__index";
  template<class C> const char* Class<C>::PVT_SMART_PTR   = "cxx_smart_ptr";
  template<class C> const char* Class<C>::PVT_RAW_PTR     = "cxx_raw_ptr";
  template<class C> const char* Class<C>::PVT_TYPE_ID     = "cxx_type_id";
  template<class C> const char* Class<C>::PVT_DELEGATE_OWNER = "cxx_delegate_owner";
  template<class C> const std::string Class<C>::META_CLASS_PREFIX = "_Meta";
}

#endif
