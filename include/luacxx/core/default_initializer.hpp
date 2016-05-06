#ifndef LUACXX_CORE_DEFAULT_INITIALIZER_HPP
# define LUACXX_CORE_DEFAULT_INITIALIZER_HPP

# include <memory>

namespace luacxx
{
  template <class T> struct default_initializer
  {
    typedef std::decay_t<T>       type;
    typedef std::shared_ptr<type> shared_type;

    static type create()
    {
      return type();
    }

    static type empty()
    {
      return type();
    }

    static shared_type shared()
    {
      return std::make_shared<type>();
    }
  };

  template <class T> struct default_initializer<std::shared_ptr<T>>
  {
    typedef std::decay_t<T>       type;
    typedef std::shared_ptr<type> shared_type;

    static shared_type create()
    {
      return default_initializer<type>::shared();
    }

    static shared_type empty()
    {
      return shared_type();
    }
  };

  template <class T> struct default_initializer<T*>
  {
    typedef std::decay_t<T>       type;
    typedef std::shared_ptr<type> shared_type;

    static auto create()
    {
      return default_initializer<type>::create();
    }

    static auto empty()
    {
      return default_initializer<type>::empty();
    }
  };
}

#endif
