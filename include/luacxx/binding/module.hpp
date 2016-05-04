#ifndef LUACXX_BINDING_MODULE_HPP
# define LUACXX_BINDING_MODULE_HPP

# include <luacxx/core/type_traits.hpp>
# include <luacxx/binding/bindable.hpp>

# include <memory>
# include <map>

# include <lua.hpp>

namespace luacxx
{
  constexpr auto root_module_name = "";

  class module : public bindable
  {
    public:
      typedef std::unique_ptr<bindable> bindable_ptr_type;

      inline module()
      {
      }

      inline void add(const std::string& name, bindable_ptr_type&& bindable)
      {
        bindables_[name] = std::move(bindable);
      }

    protected:
      typedef std::map<std::string, bindable_ptr_type> bindables_type;
      bindables_type bindables_;
  };

  class sub_module : public module
  {
    public:
      inline sub_module(const std::string& name)
        : name_(name)
      {
      }

      inline virtual void bind(state_type state) override
      {
        lua_newtable(state);
        for(auto& pair : bindables_)
        {
          const std::string& name     = pair.first;
          bindable_ptr_type& bindable = pair.second;

          bindable->bind(state);
          lua_setfield(state, -2, name.c_str());
        }

        luaL_getsubtable(state, LUA_REGISTRYINDEX, "_LOADED");
        lua_pushvalue(state, -2);
        lua_setfield(state, -2, name_.c_str());
        lua_pop(state, 1);
        lua_pushvalue(state, -1);
      }

    private:
      const std::string name_;
  };

  class global_module : public module
  {
    public:
      inline global_module()
      {
        modules_.insert(std::make_pair(root_module_name, this));
      }

      inline module& get_module(const std::string &name)
      {
        modules_type::iterator it = modules_.find(name);
        if(it == modules_.end())
        {
          std::unique_ptr<module> sub = std::make_unique<sub_module>(name);
          it = modules_.insert(std::make_pair(name, sub.get())).first;
          this->add(name, std::move(sub));
        }
        return *it->second;
      }

      inline virtual void bind(state_type state) override
      {
        for(auto& pair : bindables_)
        {
          const std::string& name     = pair.first;
          bindable_ptr_type& bindable = pair.second;

          bindable->bind(state);
          lua_setglobal(state, name.c_str());
        }
      }

    protected:
      typedef std::map<std::string, module*> modules_type;
      modules_type modules_;
  };
}

#endif
