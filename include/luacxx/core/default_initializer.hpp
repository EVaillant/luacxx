#ifndef LUACXX_CORE_DEFAULT_INITIALIZER_HPP
# define LUACXX_CORE_DEFAULT_INITIALIZER_HPP

# include <luacxx/core/lookup_type.hpp>
# include <luacxx/core/type_traits.hpp>
# include <memory>

namespace luacxx
{
  template <class T, class D = std::decay_t<T>> struct default_initializer
  {
    typedef typename register_type<T>::type type;
    typedef std::shared_ptr<type>           shared_type;
    typedef toolsbox::any                   return_type;

    static return_type create(const lookup_type& lookup)
    {
      return_type ret;
      const type_info<type>& info = lookup.template get<type>();
      if(info.valid())
      {
        switch(info.get_underlying_type())
        {
          case common_type_info::underlying_type::Class:
            ret = std::make_shared<type>();
            break;

          default:
            ret = type();
            break;

        };
      }
      return ret;
    }

    static return_type shared()
    {
      return std::make_shared<type>();
    }
  };

  template <class T, class U> struct default_initializer<T, std::shared_ptr<U>>
  {
    typedef toolsbox::any return_type;

    static return_type create(const lookup_type&)
    {
      return default_initializer<U>::shared();
    }

    static return_type shared()
    {
      return std::make_shared<std::shared_ptr<U>>();
    }
  };
}

#endif
