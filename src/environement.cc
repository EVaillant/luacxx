#include <luacxx/environement.hpp>

luacxx::Environement::Environement()
  : state_(0)
{
  state_ = luaL_newstate();
}

luacxx::Environement::~Environement()
{
  modules_.clear();

  if(state_)
  {
    lua_close(state_);
  }
}

void luacxx::Environement::open_standard_libs()
{
  if(state_)
  {
    luaL_openlibs(state_);
  }
}

bool luacxx::Environement::loadFromString(const std::string &str)
{
  return (state_ && (luaL_dostring(state_, str.c_str()) == LUA_OK));
}

void luacxx::Environement::bind()
{
  if(state_)
  {
    for(auto &pair : modules_)
    {
      std::unique_ptr<Module> &module = pair.second;
      module->bind(state_);
    }
  }
}
