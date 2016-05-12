#ifndef LUACXX_BINDING_CLASS_HPP
# define LUACXX_BINDING_CLASS_HPP

# include <luacxx/binding/engine.hpp>
# include <luacxx/binding/bindable.hpp>
# include <luacxx/binding/ctor_method.hpp>
# include <luacxx/binding/dtor_method.hpp>
# include <luacxx/binding/utility.hpp>

# include <luacxx/core/policy.hpp>
# include <luacxx/core/utility.hpp>

# include <type_traits>

# include <lua.hpp>

namespace luacxx
{  
  template <class C> class wrapper_class : public bindable
  {
    public:
      typedef C             class_type;
      typedef wrapper_class self_type;

      wrapper_class(lookup_type& lookup, const std::string& module_name, const std::string& name)
        : lookup_(lookup)
        , module_name_(module_name)
        , name_(name)
        , class_meta_name_(module_name_    + "|" + name_ + "|class_meta")
        , instance_meta_name_(module_name_ + "|" + name_ + "|instance_meta")
      {
        dtor_();
      }

      ~wrapper_class()
      {
        lookup_.reset<class_type>();
      }

      virtual bool bind(state_type state) override
      {
        // check
        if(!check_dependency())
        {
          return false;
        }

        // class
        if(!bind_(state, class_meta_name_, class_bindables_))
        {
          return false;
        }

        // instance
        if(!bind_(state, instance_meta_name_, instance_bindables_))
        {
          return false;
        }

        //
        // attach table with meta table
        lua_setmetatable(state,  -2);

        lookup_.set<class_type>(std::make_shared<class_type_info>(*this));
        return true;
      }

      template <class Base> self_type& inheritance()
      {
        inheritances_.push_back(std::make_unique<wrapper_inheritance_impl<Base>>(*this));
        return *this;
      }

      template <class ... ARGS> auto& ctor(const default_policy& = default_policy())
      {
        auto& policy = this->ctor<ARGS...>(custum_policy());
        return policy.get_owner();
      }

      template <class ... ARGS> auto& ctor(const custum_policy&)
      {
        typedef ctor_method<class_type, ARGS...>       ctor_method_type;
        typedef typename ctor_method_type::policy_type policy_type;

        std::unique_ptr<ctor_method_type> ctor = std::make_unique<ctor_method_type>(lookup_);
        policy_type &policy = ctor->get_policy();
        instance_bindables_[detail::lua_field_call] = std::move(ctor);
        return policy;
      }

    protected:
      class class_type_info : public type_info<class_type>
      {
        public:
          typedef common_type_info::variable_type variable_type;
          typedef std::shared_ptr<class_type>     smart_type;

          class_type_info(wrapper_class& owner)
            : type_info<class_type>(common_type_info::underlying_type::Class)
            , owner_(owner)
          {
          }

          virtual void to_lua(state_type state, variable_type& var, std::string &error_msg, const policy_node& policy) const override
          {
            if(!check_arg_call<const smart_type>(error_msg, var))
            {
              to_lua_impl_<const smart_type, smart_type>(state, var, detail::class_ptr_type::smart_ptr);
            }
            else if(!check_arg_call<class_type*>(error_msg, var))
            {
              error_msg.clear();
              detail::class_ptr_type type = (policy.has_parameter() && policy.get_parameter().is_delegate_owner() ? detail::class_ptr_type::raw_ptr_with_delegate_owner : detail::class_ptr_type::raw_ptr);
              to_lua_impl_<class_type*, class_type*>(state, var, type);
            }
          }

          virtual void from_lua(state_type state, std::size_t idx, variable_type& var, std::string& error_msg, const policy_node&) const override
          {
            std::pair<bool, detail::class_ptr> ret = detail::get_class_ptr<class_type>(state, idx);
            if(ret.first)
            {
              if(ret.second.id != toolsbox::type_uid::get<class_type>())
              {
                // need cast
                // TODO
              }

              if(ret.second.id != toolsbox::type_uid::get<class_type>())
              {
                error_msg = msg_error_invalid_cast_object;
              }
              else
              {
                if(ret.second.ptr)
                {                  
                  switch(ret.second.type)
                  {
                    case detail::class_ptr_type::smart_ptr:
                      var = *(smart_type*)ret.second.ptr;
                      break;

                    case detail::class_ptr_type::raw_ptr_with_delegate_owner:
                    case detail::class_ptr_type::raw_ptr:
                      var = *(class_type**)ret.second.ptr;
                      break;
                  }
                }
                else
                {
                  var = smart_type();
                }

                lua_remove(state, idx);
              }
            }
            else
            {
              error_msg = msg_error_invalid_object;
            }
          }

        protected:
          template <class Cast, class Store> void to_lua_impl_(state_type state, variable_type& var, detail::class_ptr_type type) const
          {
            lua_newtable(state);

            lua_pushstring(state, detail::lua_field_id);
            lua_pushinteger(state, toolsbox::type_uid::get<class_type>());
            lua_settable(state, -3);

            lua_pushstring(state, detail::lua_field_type);
            lua_pushinteger(state, static_cast<typename std::underlying_type<detail::class_ptr_type>::type>(type));
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
              luaL_getmetatable(state, owner_.class_meta_name_.c_str());
              lua_setmetatable(state,  -2);
            }
          }        

        private:
          wrapper_class& owner_;
      };

      class wrapper_inheritance
      {
        public:
          virtual ~wrapper_inheritance() {}
          virtual bool check_dependency() const = 0;
      };

      template <class I> class wrapper_inheritance_impl : public wrapper_inheritance
      {
        public:
          typedef I inheritance_class;

          wrapper_inheritance_impl(wrapper_class& owner)
            : owner_(owner)
          {
            static_assert(std::is_base_of<inheritance_class, class_type>::value, "invalid inheritance");
          }

          virtual bool check_dependency() const override
          {
            return owner_.lookup_.template exist<inheritance_class>();
          }

        private:
          wrapper_class& owner_;
      };

      typedef std::unique_ptr<wrapper_inheritance>     inheritance_ptr_type;
      typedef std::vector<inheritance_ptr_type>        inheritances_type;
      typedef std::unique_ptr<bindable>                bindable_ptr_type;
      typedef std::map<std::string, bindable_ptr_type> bindables_type;

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

      void dtor_()
      {
        typedef dtor_method<class_type> dtor_method_type;
        std::unique_ptr<dtor_method_type> dtor = std::make_unique<dtor_method_type>();
        class_bindables_[detail::lua_field_gc] = std::move(dtor);
      }

    private:
      lookup_type&      lookup_;
      const std::string module_name_;
      const std::string name_;
      inheritances_type inheritances_;
      bindables_type    class_bindables_;
      bindables_type    instance_bindables_;

      const std::string class_meta_name_;
      const std::string instance_meta_name_;
  };

  template <class C> auto& make_class(engine& e, const std::string& module_name, const std::string& name)
  {
    typedef wrapper_class<C> wrapper_type;
    std::unique_ptr<wrapper_type> wrapper = std::make_unique<wrapper_type>(e.get_lookup_type(), module_name, name);
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
