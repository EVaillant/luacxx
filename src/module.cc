#include <luacxx/module.hpp>

namespace
{
  std::string GLOBAL_MODULE_NAME = "";
}

namespace luacxx
{
  void Module::add(const std::string& name, std::unique_ptr<Bindable> &&bindable)
  {
    bindables_[name] = std::move(bindable);
  }

  //
  // SubModule
  SubModule::SubModule(const std::string &name, bool global /*= true*/)
    : name_(name)
    , global_(global)
  {
  }

  void SubModule::bind(lua_State *state)
  {
    lua_newtable(state);
    for(auto& pair : bindables_)
    {
      const std::string &name             = pair.first;
      std::unique_ptr<Bindable>& bindable = pair.second;

      bindable->bind(state);
      lua_setfield(state, -2, name.c_str());
    }

    luaL_getsubtable(state, LUA_REGISTRYINDEX, "_LOADED");
    lua_pushvalue(state, -2);
    lua_setfield(state, -2, name_.c_str());
    lua_pop(state, 1);
    if (global_)
    {
      lua_pushvalue(state, -1);
      lua_setglobal(state, name_.c_str());
    }
  }

  //
  // GlobalModule
  GlobalModule::GlobalModule()
  {
  }

  void GlobalModule::bind(lua_State *state)
  {
    for(auto& pair : bindables_)
    {
      const std::string &name             = pair.first;
      std::unique_ptr<Bindable>& bindable = pair.second;

      bindable->bind(state);
      lua_setglobal(state, name.c_str());
    }
  }

  std::unique_ptr<Module> make_module(const std::string name)
  {
    if(name == GLOBAL_MODULE_NAME)
    {
      return std::unique_ptr<Module>(new GlobalModule());
    }
    else
    {
      return std::unique_ptr<Module>(new SubModule(name));
    }
  }

  std::tuple<std::string /*module name*/, std::string /*type name*/> split_full_type_name(const std::string& name)
  {
    std::size_t it = name.rfind('.');
    return ((it == std::string::npos) ? std::make_tuple(GLOBAL_MODULE_NAME, name) : std::make_tuple(name.substr(0, it), name.substr(it + 1)));
  }
}
