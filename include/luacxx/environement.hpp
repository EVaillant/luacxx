#ifndef LUACXX_ENVIRONEMENT_HH
# define LUACXX_ENVIRONEMENT_HH

# include <luacxx/register_type.hpp>

# include <luacxx/class.hpp>
# include <luacxx/enum.hpp>
# include <luacxx/function.hpp>
# include <luacxx/caller.hpp>
# include <luacxx/module.hpp>

# include <map>

namespace luacxx
{
  class Environement
  {
    public:
      Environement();
      ~Environement();

      void open_standard_libs();

      template <class T> Class<T>& new_class(const std::string &name);
      template <class E, class U = typename std::underlying_type<E>::type, class C = detail::enum_to_native_value<E, U>> Enum<E, U, C>& new_enum(const std::string &name);
      template <class R, class ... ARGS> Function<R, ARGS...>& new_function(const std::string &name, const std::function<R (ARGS...)>& fct);
      template <class R, class ... ARGS> Caller<R, ARGS...> new_caller(const std::string &name);

      bool loadFromString(const std::string &str);

      void bind();

    protected:
      template <class R, class ... ARGS> R& new_bindable_(const std::string &name, ARGS &&... args);

    private:
      typedef std::map<std::string, std::unique_ptr<Module>> modules_t;

      lua_State *state_;
      RegisterType register_type_;
      modules_t modules_;
  };

  template <class R, class ... ARGS> R& Environement::new_bindable_(const std::string &name, ARGS &&... args)
  {
    std::string module_name, type_name;
    std::tie(module_name, type_name) = split_full_type_name(name);
    modules_t::iterator it_module = modules_.find(module_name);
    if(it_module == modules_.end())
    {
      std::unique_ptr<Module> module = make_module(module_name);
      it_module = modules_.insert(std::make_pair(module_name, std::move(module))).first;
    }
    std::unique_ptr<Module> &module = it_module->second;

    std::unique_ptr<R> val(new R(register_type_, name, std::forward<ARGS>(args)...));
    R& ret = *val;
    module->add(type_name, std::move(val));

    return ret;
  }

  template <class T> Class<T>& Environement::new_class(const std::string &name)
  {
    typedef Class<T> class_t;
    return new_bindable_<class_t>(name);
  }

  template <class E, class U, class C> Enum<E, U, C>& Environement::new_enum(const std::string &name)
  {
    typedef Enum<E, U, C> enum_t;
    return new_bindable_<enum_t>(name);
  }

  template <class R, class ... ARGS> Function<R, ARGS...>& Environement::new_function(const std::string &name, const std::function<R (ARGS...)>& fct)
  {
    typedef Function<R, ARGS...> function_t;
    return new_bindable_<function_t>(name, fct);
  }

  template <class R, class ... ARGS> Caller<R, ARGS...> Environement::new_caller(const std::string &name)
  {
    typedef Caller<R, ARGS...> caller_t;
    return caller_t(register_type_, name, state_);
  }
}

#endif
