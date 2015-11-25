#ifndef TEST_LUACXX_HH
# define TEST_LUACXX_HH

# include <luacxx/enum.hpp>
# include <luacxx/class.hpp>
# include <luacxx/function.hpp>

# include <luacxx/module.hpp>

namespace helpers
{
  template <class T> luacxx::Class<T>& make_class(luacxx::RegisterType &registry, luacxx::Module& module, const std::string &name)
  {
    typedef luacxx::Class<T> class_t;
    std::unique_ptr<class_t> cls(new class_t(registry, name));
    class_t &ret = *cls;
    module.add(name, std::move(cls));
    return ret;
  }

  template <class ... T> luacxx::Enum<T...>& make_enum(luacxx::RegisterType &registry, luacxx::Module& module, const std::string &name)
  {
    typedef luacxx::Enum<T...> enum_t;
    std::unique_ptr<enum_t> enm(new enum_t(registry, name));
    enum_t &ret = *enm;
    module.add(name, std::move(enm));
    return ret;
  }

  template <class R, class ... ARGS> luacxx::Function<R, ARGS...>& make_function(luacxx::RegisterType& registry,luacxx::Module& module, const std::string &name, R (*fct)(ARGS...))
  {
    typedef luacxx::Function<R, ARGS...> function_t;
    std::unique_ptr<function_t>  f(new function_t(registry, name, fct));
    function_t &ret = *f;
    module.add(name, std::move(f));
    return ret;
  }
}

#endif
