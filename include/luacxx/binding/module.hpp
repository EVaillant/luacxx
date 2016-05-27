#ifndef LUACXX_BINDING_MODULE_HPP
# define LUACXX_BINDING_MODULE_HPP

# include <luacxx/core/type_traits.hpp>
# include <luacxx/binding/bindable.hpp>

# include <memory>
# include <map>
# include <vector>

# include <lua.hpp>

namespace luacxx
{
  constexpr auto root_module_name = "";

  class module
  {
    public:
      typedef std::unique_ptr<bindable> bindable_ptr_type;

      inline module(const std::string prefix)
        : prefix_(prefix)
      {
      }

      inline virtual ~module()
      {
      }

      inline virtual void add(const std::string& name, bindable_ptr_type&& bindable)
      {
        bindables_.insert(std::make_pair(name, bindable_data{std::move(bindable)}));
      }

      inline virtual std::vector<std::string> need_to_bind()
      {
        std::vector<std::string> ret;
        for(auto& pair : bindables_)
        {
          const std::string& name = pair.first;
          bindable_data& data     = pair.second;
          if(!data.status)
          {
            ret.push_back(prefix_ + name);
          }
        }
        return ret;
      }

      virtual void get_symbol(state_type state, const std::string& name) = 0;

    protected:
      struct bindable_data
      {
        bindable_data(bindable_ptr_type&& b)
          : status(false)
          , bindable(std::move(b))
        {
        }

        bool              status;
        bindable_ptr_type bindable;
      };

      typedef std::map<std::string, bindable_data> bindables_type;
      bindables_type    bindables_;
      const std::string prefix_;
  };


  class global_module : public module
  {
    public:      
      inline global_module()
        : module(root_module_name)
      {
        modules_.insert(std::make_pair(root_module_name, this));
      }

      inline module& get_module(const std::string &name)
      {
        modules_type::iterator it = modules_.find(name);
        if(it == modules_.end())
        {
          std::unique_ptr<sub_module> sub = std::make_unique<sub_module>(*this, name);
          it = modules_.insert(std::make_pair(name, sub.get())).first;
          this->add(name, std::move(sub));
        }
        return *it->second;
      }

      inline void bind(state_type state)
      {
        for(auto& pair : bindables_)
        {
          const std::string&     name = pair.first;
          bindable_data&         data = pair.second;
          bindable_ptr_type& bindable = data.bindable;

          if(!data.status && bindable->bind(state))
          {
            data.status = true;
            lua_setglobal(state, name.c_str());
          }
        }
      }

      inline virtual std::vector<std::string> need_to_bind() override
      {
        std::vector<std::string> ret = module::need_to_bind();
        for(auto& pair : modules_)
        {
          module* mod  = pair.second;

          if(mod == this)
          {
            continue;
          }

          std::vector<std::string> tmp = mod->need_to_bind();
          ret.insert(ret.end(), tmp.begin(), tmp.end());
        }
        return ret;
      }

      inline virtual void get_symbol(state_type state, const std::string& name) override
      {
        lua_getglobal(state, name.c_str());
      }

    private:
      class sub_module : public module,  public bindable
      {
        public:
          inline sub_module(global_module& owner, const std::string& name)
            : module(name + ".")
            , name_(name)
            , owner_(owner)
            , binded_(false)
          {
          }

          inline virtual bool bind(state_type state) override
          {
            if(! binded_)
            {
              lua_newtable(state);
            }
            else
            {
              luaL_getsubtable(state, LUA_REGISTRYINDEX, "_LOADED");
              lua_getfield(state, -1, name_.c_str());
            }

            for(auto& pair : bindables_)
            {
              const std::string&     name = pair.first;
              bindable_data&         data = pair.second;
              bindable_ptr_type& bindable = data.bindable;

              if(!data.status && bindable->bind(state))
              {
                data.status = true;
                lua_setfield(state, -2, name.c_str());
              }
            }

            if(!binded_)
            {
              luaL_getsubtable(state, LUA_REGISTRYINDEX, "_LOADED");
              lua_pushvalue(state, -2);
              lua_setfield(state, -2, name_.c_str());
              lua_pop(state, 1);
              lua_pushvalue(state, -1);

              binded_ = true;
            }
            else
            {
              lua_pop(state, 2);
            }

            return true;
          }

          inline virtual void add(const std::string& name, bindable_ptr_type&& bindable) override
          {
            module::add(name, std::move(bindable));
            owner_.reset_loaded_status_(name_);
          }

          inline virtual void get_symbol(state_type state, const std::string& name) override
          {
            luaL_getsubtable(state, LUA_REGISTRYINDEX, "_LOADED");
            lua_getfield(state, -1, name_.c_str());
            lua_getfield(state, -1, name.c_str());
            lua_remove(state, -2);
            lua_remove(state, -2);
          }

        private:
          const std::string name_;
          global_module&    owner_;
          bool              binded_;
      };

      inline void reset_loaded_status_(const std::string& name)
      {
        bindables_type::iterator it = bindables_.find(name);
        if(it != bindables_.end())
        {
          bindable_data& data = it->second;
          data.status = false;
        }
      }

      typedef std::map<std::string, module*> modules_type;
      modules_type modules_;
  };
}

#endif
