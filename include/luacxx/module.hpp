#ifndef LUACXX_MODULE_HH
# define LUACXX_MODULE_HH

# include <luacxx/bindable.hpp>

# include <string>
# include <memory>
# include <map>

namespace luacxx
{
  class Module : public Bindable
  {
    public:
      void add(const std::string& name, std::unique_ptr<Bindable>&& bindable);

    protected:
      typedef std::map<std::string, std::unique_ptr<Bindable>> bindables_t;
      bindables_t bindables_;
  };

  class SubModule : public Module
  {
    public:
      SubModule(const std::string &name, bool global = true);

      // from Bindable
      virtual void bind(lua_State *state) override;

    private:
      const std::string name_;
      bool global_;
  };

  class GlobalModule : public Module
  {
    public:
      GlobalModule();

      // from Bindable
      virtual void bind(lua_State *state) override;
  };

  std::unique_ptr<Module> make_module(const std::string name);
  std::tuple<std::string /*module name*/, std::string /*type name*/> split_full_type_name(const std::string& name);
}

#endif
