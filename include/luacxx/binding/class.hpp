#ifndef LUACXX_BINDING_CLASS_HPP
# define LUACXX_BINDING_CLASS_HPP

# include <luacxx/binding/engine.hpp>
# include <luacxx/binding/bindable.hpp>
# include <luacxx/binding/ctor_method.hpp>
# include <luacxx/binding/dtor_method.hpp>
# include <luacxx/binding/method.hpp>

# include <luacxx/core/policy.hpp>
# include <luacxx/core/utility.hpp>

# include <type_traits>

# include <lua.hpp>

namespace luacxx
{
  struct default_policy {};
  struct custum_policy  {};

  namespace detail
  {
    class wrapper_inheritance
    {
      public:
        inline virtual ~wrapper_inheritance() {}

        virtual bool check_dependency() const = 0;
        virtual void link() = 0;
        virtual const std::string& get_module_name() const = 0;
        virtual const std::string& get_class_name() const = 0;
    };

    class wrapper_property
    {
      public:
        inline virtual ~wrapper_property() {}

        virtual void get(state_type state, std::size_t idx) = 0;
        virtual void set(state_type state, std::size_t idx) = 0;

        virtual bool has_getter() const = 0;
        virtual bool has_setter() const = 0;
    };
    typedef std::unique_ptr<wrapper_property>        property_ptr_type;
    typedef std::map<std::string, property_ptr_type> properties_type;

    class property_new_index : public callable
    {
      public:
        inline property_new_index(properties_type& properties)
          : properties_(properties)
        {
        }

      protected:
        inline virtual int invoke_(state_type state) override
        {
          if(lua_isstring(state, -2))
          {
            std::string name = lua_tostring(state, -2);
            lua_remove(state, -2);

            properties_type::iterator it_properties = properties_.find(name);
            if(it_properties != properties_.end())
            {
              property_ptr_type& property = it_properties->second;
              if(property->has_setter())
              {
                property->set(state, 1);
              }
              else
              {
                luaL_error(state, msg_error_property_ro);
              }
            }
            else
            {
              luaL_error(state, msg_error_invalid_property_name);
            }
          }
          return 0;
        }

      private:
        properties_type& properties_;
    };

    class propery_and_method_index : public bindable
    {
      public:
        inline propery_and_method_index(properties_type& properties)
          : properties_(properties)
        {
        }

        inline virtual bool bind(state_type state) override
        {
          lua_pushvalue(state, -1);
          if(has_one_getter_())
          {
            lua_pushlightuserdata(state, this);
            lua_pushcclosure(state, &lua_wrapper_, 2);
          }
          return true;
        }

      protected:
        static inline int lua_wrapper_(state_type state)
        {
          propery_and_method_index* self = (propery_and_method_index*) lua_touserdata(state, lua_upvalueindex(2));
          return self->invoke_(state);
        }

        inline int invoke_(state_type state)
        {
          bool found = lua_isstring(state, -1);
          if(found)
          {
            std::string name = lua_tostring(state, -1);
            lua_remove(state, -1);

            properties_type::iterator it_properties = properties_.find(name);
            if(it_properties != properties_.end())
            {
              property_ptr_type& property = it_properties->second;
              if(property->has_getter())
              {
                property->get(state, 1);
              }
              else
              {
                luaL_error(state, msg_error_property_wo);
              }
            }
            else
            {
              lua_remove(state, -1);
              lua_pushvalue(state, lua_upvalueindex(1));
              lua_getfield(state, -1, name.c_str());
            }
          }
          return found;
        }

        inline bool has_one_getter_() const
        {
          bool ret = false;
          for(const properties_type::value_type& val: properties_)
          {
            ret = val.second->has_getter();
            if(ret)
            {
              break;
            }
          }
          return ret;
        }

      private:
        properties_type&   properties_;
    };

    class inheritance_index : public callable
    {
      public:
        typedef std::unique_ptr<wrapper_inheritance> inheritance_ptr_type;
        typedef std::vector<inheritance_ptr_type>    inheritances_type;

        inline inheritance_index(engine& e, inheritances_type& inheritances)
          : engine_(e)
          , inheritances_(inheritances)
        {
        }

        inline virtual bool bind(state_type state) override
        {
          if(inheritances_.size() == 1)
          {
            inheritance_ptr_type& inheritance = *inheritances_.begin();
            get_symbol_(state, *inheritance);
          }
          else
          {
            callable::bind(state);
          }
          return 1;
        }

      protected:
        inline virtual int invoke_(state_type state) override
        {
          bool found = false;
          if(lua_isstring(state, -1))
          {
            std::string name = lua_tostring(state, -1);
            lua_pop(state, 2);

            for(inheritance_ptr_type& inheritance : inheritances_)
            {
              get_symbol_(state, *inheritance);
              lua_getfield(state, -1, name.c_str());
              if(!lua_isnil(state, -1))
              {
                lua_remove(state, -2);
                lua_remove(state, -2);
                found = true;
                break;
              }
              else
              {
                lua_pop(state, 2);
              }
            }
          }
          return found;
        }

        inline void get_symbol_(state_type state, wrapper_inheritance& inheritance)
        {
          engine_.get_module(inheritance.get_module_name()).get_symbol(state, inheritance.get_class_name());
        }

      private:
        engine&            engine_;
        inheritances_type& inheritances_;
    };

    template <class C> class class_type_info : public type_info<C>
    {
      public:
        typedef common_type_info::variable_type        variable_type;
        typedef common_class_type_info::class_ptr_type class_ptr_type;
        typedef common_class_type_info::class_field    class_field;

        typedef C                            class_type;
        typedef std::shared_ptr<class_type>  smart_type;

        class_type_info(const std::string& module_name, const std::string& class_name, const std::string& class_meta_name)
          : type_info<class_type>(&class_type_info::to_any_type_)
          , class_meta_name_(class_meta_name)
          , class_name_(class_name)
          , module_name_(module_name)
        {
        }

        virtual void to_lua(state_type state, variable_type& var, std::string &error_msg, const policy_node& policy) const override
        {
          to_lua_<class_type>(state, var, error_msg, policy);
        }

        virtual void from_lua(state_type state, std::size_t idx, variable_type& var, std::string& error_msg, const policy_node&) const override
        {
          std::pair<bool, class_field> ret = this->get_class_field(state, idx);
          if(ret.first)
          {
            var = this->get_instance(ret.second);
            if(var.empty())
            {
              error_msg = msg_error_invalid_object;
            }
          }
          else
          {
            error_msg = msg_error_object_corrupted;
          }
        }

        virtual const std::string& get_module_name() const override
        {
          return module_name_;
        }

        virtual const std::string& get_class_name() const override
        {
          return class_name_;
        }

      protected:
        template <class T> typename std::enable_if<!is_incomplete<T>::value>::type to_lua_(state_type state, variable_type& var, std::string &error_msg, const policy_node& policy) const
        {
          if(!check_arg_call<const smart_type>(error_msg, var))
          {
            to_lua_impl_<const smart_type, smart_type>(state, var, class_ptr_type::smart_ptr);
          }
          else if(!check_arg_call<class_type*>(error_msg, var))
          {
            error_msg.clear();
            if(policy.has_parameter() && policy.get_parameter().is_delegate_owner())
            {
              smart_type        smart(cast_arg_call<class_type*>(var));
              variable_type var_smart = smart;

              to_lua_impl_<const smart_type, smart_type>(state, var_smart, class_ptr_type::smart_ptr);
            }
            else
            {
              to_lua_impl_<class_type*, class_type*>(state, var, class_ptr_type::raw_ptr);
            }
          }
        }

        template <class T> typename std::enable_if< is_incomplete<T>::value>::type to_lua_(state_type state, variable_type& var, std::string &error_msg, const policy_node&) const
        {
          if(!check_arg_call<const smart_type>(error_msg, var))
          {
            to_lua_impl_<const smart_type, smart_type>(state, var, class_ptr_type::smart_ptr);
          }
          else if(!check_arg_call<class_type*>(error_msg, var))
          {
            error_msg.clear();
            to_lua_impl_<class_type*, class_type*>(state, var, class_ptr_type::raw_ptr);
          }
        }

        template <class Cast, class Store> void to_lua_impl_(state_type state, variable_type& var, class_ptr_type type) const
        {
          lua_newtable(state);

          lua_pushstring(state, detail::lua_field_id);
          lua_pushinteger(state, toolsbox::type_uid::get<class_type>());
          lua_settable(state, -3);

          lua_pushstring(state, detail::lua_field_type);
          lua_pushinteger(state, static_cast<typename std::underlying_type<class_ptr_type>::type>(type));
          lua_settable(state, -3);

          lua_pushstring(state, detail::lua_field_ptr);
          Store* ptr = (Store*) lua_newuserdata(state, sizeof(Store));
          new (ptr) Store(cast_arg_call<Cast>(var));
          lua_settable(state, -3);

          if(*ptr == nullptr)
          {
            lua_pop(state, 1);
            lua_pushnil(state);
          }
          else
          {
            luaL_getmetatable(state, class_meta_name_.c_str());
            lua_setmetatable(state,  -2);
          }
        }

        static toolsbox::any to_any_type_(const class_field& c)
        {
          toolsbox::any ret;
          if(!c.ptr)
          {
            ret = smart_type();
          }
          else
          {
            switch(c.type)
            {
              case class_ptr_type::smart_ptr:
                ret = *(smart_type*)c.ptr;
                break;

              case class_ptr_type::raw_ptr:
                ret = *(class_type**)c.ptr;
                break;
            }
          }
          return ret;
        }

      private:
        const std::string  class_meta_name_;
        const std::string& class_name_;
        const std::string& module_name_;
    };

    template <class T, class I> class wrapper_inheritance_impl : public wrapper_inheritance
    {
      public:
        typedef T                                   class_type;
        typedef I                                   inheritance_class;
        typedef std::shared_ptr<class_type>         smart_type;
        typedef std::shared_ptr<inheritance_class>  smart_inheritance_class;
        typedef std::shared_ptr<class_type_info<T>> class_info_smart_type;

        wrapper_inheritance_impl(lookup_type& lookup, const class_info_smart_type& class_type_info)
          : lookup_(lookup)
          , class_type_info_(class_type_info)
        {
          static_assert(std::is_base_of<inheritance_class, class_type>::value, "invalid inheritance");
        }

        virtual bool check_dependency() const override
        {
          return lookup_.template exist<inheritance_class>();
        }

        virtual void link() override
        {
          type_info<inheritance_class>& in = lookup_.template get<inheritance_class>();
          in.add_base(&wrapper_inheritance_impl::cast_type_, class_type_info_);
        }

        virtual const std::string& get_module_name() const override
        {
          type_info<inheritance_class>& in = lookup_.template get<inheritance_class>();
          return in.get_module_name();
        }

        virtual const std::string& get_class_name() const override
        {
          type_info<inheritance_class>& in = lookup_.template get<inheritance_class>();
          return in.get_class_name();
        }

      protected:
        static void cast_type_(toolsbox::any& a)
        {
          assert(a.is<class_type*>() || a.is<smart_type>());
          static const toolsbox::type_uid::id_type class_id = toolsbox::type_uid::get<class_type*>();
          static const toolsbox::type_uid::id_type smart_id = toolsbox::type_uid::get<smart_type>();

          toolsbox::type_uid::id_type id = a.get_id();
          toolsbox::any new_a;

          if(id == class_id)
          {
            inheritance_class* tmp = a.as<class_type*>();
            new_a = std::move(tmp);
          }
          else if(id == smart_id)
          {
            smart_inheritance_class tmp = a.as<smart_type>();
            new_a = std::move(tmp);
          }

          std::swap(a, new_a);
        }

      private:
        lookup_type&          lookup_;
        class_info_smart_type class_type_info_;
    };

    template <class O, class T, class F> class wrapper_property_impl : public wrapper_property
    {
      public:
        typedef F field_type;
        typedef T class_type;
        typedef O owner_type;
        typedef std::function<field_type (const class_type&)> getter_type;
        typedef std::function<void (class_type&, field_type)> setter_type;
        typedef arg_policy<owner_type> policy_type;

        wrapper_property_impl(lookup_type& lookup, owner_type& owner, const getter_type& getter, const setter_type& setter)
          : lookup_(lookup)
          , getter_(getter)
          , setter_(setter)
          , policy_(make_arg_policy<owner_type, field_type>(owner, node_))
        {
        }

        virtual void get(state_type state, std::size_t idx) override
        {
          type_info<class_type>& info = lookup_.get<class_type>();
          class_type* self            = info.template get_instance<class_type>(state, idx);
          if(self)
          {
            field_type field  = getter_(*self);
            toolsbox::any var = std::ref(field);
            std::string   error_msg;
            convert_to<field_type&>(state, lookup_, var, error_msg, node_);
            if(!error_msg.empty())
            {
              luaL_error(state, error_msg.c_str());
            }
          }
        }

        virtual void set(state_type state, std::size_t idx) override
        {
          type_info<class_type>& info = lookup_.get<class_type>();
          class_type* self            = info.template get_instance<class_type>(state, idx);
          if(self)
          {
            toolsbox::any var;
            std::string   error_msg;
            convert_from<field_type&>(state, lookup_, idx, var, error_msg, node_);
            if(error_msg.empty() && !check_arg_call<field_type>(error_msg, var))
            {
              setter_(*self, cast_arg_call<field_type>(var));
            }
            else
            {
              luaL_error(state, error_msg.c_str());
            }
          }
        }

        virtual bool has_getter() const
        {
          return (bool)getter_;
        }

        virtual bool has_setter() const
        {
          return (bool)setter_;
        }

        policy_type& get_policy()
        {
          return policy_;
        }

      private:
        lookup_type& lookup_;
        getter_type  getter_;
        setter_type  setter_;
        policy_node  node_;
        policy_type  policy_;
    };
  }  

  template <class C> class wrapper_class : public bindable
  {
    public:
      typedef C                                   class_type;
      typedef detail::class_type_info<class_type> class_info_type;
      typedef std::shared_ptr<class_type>         smart_type;
      typedef wrapper_class                       self_type;

      wrapper_class(engine& e, lookup_type& lookup, const std::string& module_name, const std::string& name)
        : engine_(e)
        , lookup_(lookup)
        , module_name_(module_name)
        , name_(name)
        , class_meta_name_(module_name_    + "|" + name_ + "|class_meta")
        , instance_meta_name_(module_name_ + "|" + name_ + "|instance_meta")
      {
        class_type_info_ = std::make_shared<class_info_type>(module_name_, name_, class_meta_name_);
        default_mth_();
      }

      ~wrapper_class()
      {
        lookup_.reset<class_type>();
      }

      virtual bool bind(state_type state) override
      {
        bool ret = (check_dependency() && bind_(state, class_meta_name_, class_bindables_) && bind_(state, instance_meta_name_, instance_bindables_));
        if(ret)
        {
          //
          // attach table with meta table
          lua_setmetatable(state,  -2);

          //
          // link with other
          assert(!lookup_.exist<class_type>());
          lookup_.set<class_type>(class_type_info_);

          for(auto& type : inheritances_)
          {
            type->link();
          }
        }
        return ret;
      }

      template <class Base> self_type& inheritance()
      {
        if(inheritances_.empty())
        {
          instance_bindables_[detail::lua_field_index] = std::make_unique<detail::inheritance_index>(engine_, inheritances_);
        }
        inheritances_.push_back(std::make_unique<detail::wrapper_inheritance_impl<class_type, Base>>(lookup_, class_type_info_));
        return *this;
      }

      template <class ... ARGS> auto& ctor(const default_policy& = default_policy())
      {
        this->ctor<ARGS...>(custum_policy());
        return *this;
      }

      template <class ... ARGS> auto& ctor(const custum_policy&)
      {
        typedef ctor_method<self_type, class_type, ARGS...> ctor_method_type;
        typedef typename ctor_method_type::policy_type      policy_type;

        std::unique_ptr<ctor_method_type> ctor = std::make_unique<ctor_method_type>(lookup_, *this);
        policy_type &policy = ctor->get_policy();
        instance_bindables_[detail::lua_field_call] = std::move(ctor);
        return policy;
      }

      template <class R, class ... ARGS> auto& method(const std::string& name, R (class_type::*mth)(ARGS ...), const default_policy& = default_policy())
      {
        this->extern_method<R, ARGS...>(name, std::function<R (class_type*, ARGS ...)>(mth), custum_policy());
        return *this;
      }

      template <class R, class ... ARGS> auto& method(const std::string& name, R (class_type::*mth)(ARGS ...), const custum_policy&)
      {
        return this->extern_method<R, ARGS...>(name, std::function<R (class_type*, ARGS ...)>(mth), custum_policy());
      }

      template <class R, class ... ARGS> auto& const_method(const std::string& name, R (class_type::*mth)(ARGS ...) const, const default_policy& = default_policy())
      {
        this->extern_method<R, ARGS...>(name, std::function<R (class_type*, ARGS ...)>(mth), custum_policy());
        return *this;
      }

      template <class R, class ... ARGS> auto& const_method(const std::string& name, R (class_type::*mth)(ARGS ...) const, const custum_policy&)
      {
        return this->extern_method<R, ARGS...>(name, std::function<R (class_type*, ARGS ...)>(mth), custum_policy());
      }

      template <class R, class ... ARGS> auto& extern_method(const std::string& name, R (*mth)(class_type*, ARGS ...), const default_policy& = default_policy())
      {
        this->extern_method<R, ARGS...>(name, std::function<R (class_type*, ARGS ...)>(mth), custum_policy());
        return *this;
      }

      template <class R, class ... ARGS> auto& extern_method(const std::string& name, R (*mth)(class_type*, ARGS ...), const custum_policy&)
      {
        return this->extern_method<R, ARGS...>(name, std::function<R (class_type*, ARGS ...)>(mth), custum_policy());
      }

      template <class R, class ... ARGS> auto& extern_method(const std::string& name, const std::function<R (class_type*, ARGS ...)>& functor, const default_policy& = default_policy())
      {
        this->extern_method<R, ARGS...>(name, functor, custum_policy());
        return *this;
      }

      template <class R, class ... ARGS> auto& extern_method(const std::string& name, const std::function<R (class_type*, ARGS ...)>& functor, const custum_policy&)
      {
        typedef luacxx::method<self_type, class_type, R, ARGS...> method_type;
        typedef typename method_type::policy_type                 policy_type;

        std::unique_ptr<method_type> mth = std::make_unique<method_type>(lookup_, *this, class_type_info_, functor);
        policy_type &policy = mth->get_policy();
        class_bindables_[name] = std::move(mth);
        return policy;
      }

      template <class A> auto& property(const std::string& name, A (class_type::*attr), const default_policy& = default_policy())
      {
        this->property<A>(name, attr, custum_policy());
        return *this;
      }

      template <class A> auto& property(const std::string& name, A (class_type::*attr), const custum_policy& p)
      {
        std::function<const A& (const class_type&)> getter = [attr](const class_type&t) -> const A&
        {
          return t.*attr;
        };

        std::function<void (class_type&, const A&)> setter = [attr](class_type&t, const A& a)
        {
          t.*attr = a;
        };

        return this->property<const A&>(name, getter, setter, p);
      }            

      template <class A> auto& property_readonly(const std::string& name, const A (class_type::*attr), const default_policy& = default_policy())
      {
        this->property<A>(name, attr, custum_policy());
        return *this;
      }

      template <class A> auto& property_readonly(const std::string& name, const A (class_type::*attr), const custum_policy& p)
      {
        std::function<const A& (const class_type&)> getter = [attr](const class_type&t) -> const A&
        {
          return t.*attr;
        };
        std::function<void (class_type&, const A&)> setter;

        return this->property<const A&>(name, getter, setter, p);
      }

      template <class A> auto& property_writeonly(const std::string& name, A (class_type::*attr), const default_policy& = default_policy())
      {
        this->property<A>(name, attr, custum_policy());
        return *this;
      }

      template <class A> auto& property_writeonly(const std::string& name, A (class_type::*attr), const custum_policy& p)
      {
        std::function<const A& (const class_type&)> getter;
        std::function<void (class_type&, const A&)> setter = [attr](class_type&t, const A& a)
        {
          t.*attr = a;
        };

        return this->property<const A&>(name, getter, setter, p);
      }

      template <class A> auto& property(const std::string& name, A (class_type::*g_attr)() const, void (class_type::*s_attr)(A), const default_policy& = default_policy())
      {
        this->property<A>(name, g_attr, s_attr, custum_policy());
        return *this;
      }

      template <class A> auto& property(const std::string& name, A (class_type::*g_attr)() const, void (class_type::*s_attr)(A), const custum_policy& p)
      {
        std::function<A (const class_type&)> getter;
        if(g_attr)
        {
          getter = [g_attr](const class_type&t) -> A
          {
            return (t.*g_attr)();
          };
        }

        std::function<void (class_type&, A)> setter;
        if(s_attr)
        {
          setter = [s_attr](class_type&t, A a)
          {
            (t.*s_attr)(a);
          };
        }

        return this->property<A>(name, getter, setter, p);
      }

      template <class A> auto& property(const std::string& name, const std::function<A (const class_type&)>& getter, const std::function<void (class_type&, A)>& setter, const default_policy& = default_policy())
      {
        this->property<A>(name, getter, setter, custum_policy());
        return *this;
      }

      template <class A> auto& property(const std::string& name, const std::function<A (const class_type&)>& getter, const std::function<void (class_type&, A)>& setter, const custum_policy&)
      {
        typedef detail::wrapper_property_impl<self_type, class_type, A> wrapper_type;
        typedef typename wrapper_type::policy_type                      policy_type;

        if(setter && class_bindables_.find(detail::lua_field_newindex) == class_bindables_.end())
        {
          class_bindables_[detail::lua_field_newindex] = std::make_unique<detail::property_new_index>(properties_);
        }
        std::unique_ptr<wrapper_type> wrapper = std::make_unique<wrapper_type>(lookup_, *this, getter, setter);
        policy_type &policy = wrapper->get_policy();
        properties_.insert(std::make_pair(name, std::move(wrapper)));
        return policy;
      }

    protected:           
      typedef detail::inheritance_index::inheritances_type inheritances_type;
      typedef detail::properties_type                      properties_type;
      typedef std::unique_ptr<bindable>                    bindable_ptr_type;
      typedef std::map<std::string, bindable_ptr_type>     bindables_type;
      typedef std::shared_ptr<class_info_type>             class_info_smart_type;

      bool check_dependency() const
      {
        bool ret = true;
        for(auto& type : inheritances_)
        {
          ret = ret && type->check_dependency();
        }
        return ret;
      }

      bool bind_(state_type state, const std::string& name, const bindables_type& bindables)
      {
        luaL_newmetatable(state, name.c_str());
        for(auto& pair : bindables)
        {
          const std::string& name           = pair.first;
          const bindable_ptr_type& bindable = pair.second;
          if(bindable->bind(state))
          {
            lua_setfield(state, -2, name.c_str());
          }
          else
          {
            return false;
          }
        }
        return true;
      }

      void default_mth_()
      {
        typedef dtor_method<class_type> dtor_method_type;

        class_bindables_[detail::lua_field_gc]    = std::make_unique<dtor_method_type>(class_type_info_);
        class_bindables_[detail::lua_field_index] = std::make_unique<detail::propery_and_method_index>(properties_);
      }            

    private:
      engine&               engine_;
      lookup_type&          lookup_;
      const std::string     module_name_;
      const std::string     name_;
      inheritances_type     inheritances_;
      properties_type       properties_;
      bindables_type        class_bindables_;
      bindables_type        instance_bindables_;
      class_info_smart_type class_type_info_;

      const std::string class_meta_name_;
      const std::string instance_meta_name_;
  };

  template <class C> auto& make_class(engine& e, const std::string& module_name, const std::string& name)
  {
    typedef wrapper_class<C> wrapper_type;
    std::unique_ptr<wrapper_type> wrapper = std::make_unique<wrapper_type>(e, e.get_lookup_type(), module_name, name);
    wrapper_type&                     ret = *wrapper;
    module &m = e.get_module(module_name);
    m.add(name, std::move(wrapper));
    return ret;
  }

  template <class C> auto& make_class(engine& e, const std::string& name)
  {
    return make_class<C>(e, root_module_name, name);
  }
}

#endif
